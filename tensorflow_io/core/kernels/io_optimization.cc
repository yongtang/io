/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "mlir/IR/Module.h"  // from @llvm-project
#include "llvm/Support/FormatVariadic.h"
#include "tensorflow/core/common_runtime/optimization_registry.h"
#include "tensorflow/core/protobuf/graph_debug_info.pb.h"
#include "tensorflow/compiler/mlir/tensorflow/translate/import_model.h"
#include "tensorflow/compiler/mlir/tensorflow/translate/export_graphdef.h"
#include "tensorflow/compiler/mlir/tensorflow/utils/dump_mlir_util.h"

namespace tensorflow {
namespace io {
namespace {

// Dumps the MLIR module to disk.
// This require the TF_DUMP_GRAPH_PREFIX to be set to a path that exist (or can
// be created).
static void DumpModule(mlir::ModuleOp module, std::string file_prefix) {
  std::string prefix = GetDumpDirFromEnvVar();
  if (prefix.empty()) return;

  auto* env = tensorflow::Env::Default();
  auto status = env->RecursivelyCreateDir(prefix);
  if (!status.ok()) {
    LOG(WARNING) << "cannot create directory '" + prefix +
                        "': " + status.error_message();
    return;
  }

  prefix += "/" + file_prefix;
  if (!tensorflow::Env::Default()->CreateUniqueFileName(&prefix, ".mlir")) {
    LOG(WARNING) << "cannot create unique filename, won't dump MLIR module.";
    return;
  }

  std::unique_ptr<WritableFile> file_writer;
  status = env->NewWritableFile(prefix, &file_writer);
  if (!status.ok()) {
    LOG(WARNING) << "cannot open file '" + prefix +
                        "': " + status.error_message();
    return;
  }

  // Print the module to a string before writing to the file.
  std::string txt_module;
  {
    llvm::raw_string_ostream os(txt_module);
    module.print(os);
  }

  status = file_writer->Append(txt_module);
  if (!status.ok()) {
    LOG(WARNING) << "error writing to file '" + prefix +
                        "': " + status.error_message();
    return;
  }
  (void)file_writer->Close();
  VLOG(1) << "Dumped MLIR module to " << prefix;
}


class IOGraphOptimizationPass : public GraphOptimizationPass {
 public:
  IOGraphOptimizationPass() {
    enable_ = (std::getenv("TFIO_GRAPH_DEBUG") != nullptr);
    if (enable_) {
      LOG(INFO) << "TFIO_GRAPH_DEBUG: [init]";
    }
  }
  virtual ~IOGraphOptimizationPass() {
    if (enable_) {
      LOG(INFO) << "TFIO_GRAPH_DEBUG: [fini]";
    }
  }
  Status Run(const GraphOptimizationPassOptions& options) override {
    if (enable_) {
      Graph* graph = options.graph->get();
      LOG(INFO) << "TFIO_GRAPH_DEBUG: [run]:"
                << graph->ToGraphDefDebug().DebugString();
    }
  GraphDebugInfo debug_info;
  mlir::MLIRContext context;
  GraphImportConfig import_config;
  import_config.upgrade_legacy = true;
  TF_ASSIGN_OR_RETURN(
      auto module_ref,
      ConvertGraphToMlir(**options.graph, debug_info, *options.flib_def,
                         import_config, &context));

  DumpModule(*module_ref, llvm::formatv("mlir_{0}", "io_graph"));

  GraphExportConfig export_config;
  TF_RETURN_WITH_CONTEXT_IF_ERROR(
      ConvertMlirToGraph(*module_ref, export_config, options.graph,
                         options.flib_def),
      "Error converting MLIR module back to graph");

    return Status::OK();
  }

 private:
  bool enable_ = false;
};

REGISTER_OPTIMIZATION(OptimizationPassRegistry::PRE_PLACEMENT, 15,
                      IOGraphOptimizationPass);

}  // namespace
}  // namespace io
}  // namespace tensorflow
