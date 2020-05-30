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

#include "mlir/IR/Diagnostics.h"         // from @llvm-project
#include "mlir/IR/Module.h"              // from @llvm-project
#include "mlir/Pass/PassManager.h"       // from @llvm-project
#include "mlir/Pass/Pass.h" // from @llvm-project
#include "mlir/Transforms/DialectConversion.h"
#include "mlir/Support/LogicalResult.h"  // from @llvm-project
#include "tensorflow/compiler/mlir/mlir_graph_optimization_pass.h"
#include "tensorflow/compiler/mlir/tensorflow/utils/error_util.h"
#include "tensorflow/compiler/mlir/tensorflow/ir/tf_ops.h"

#include "mlir/InitAllDialects.h"

static bool foo_dialect_registration_once = []() {
  mlir::registerAllDialects();
  return true;
}();

namespace tensorflow {
namespace io {
namespace {

class AudioOptimizationPass
    : public mlir::PassWrapper<AudioOptimizationPass, mlir::OperationPass<mlir::ModuleOp>> {
 public:
  void runOnOperation() override {
std::cerr << "XXXXX runOnOperation() ENTER XXXXX" << std::endl;
  mlir::ModuleOp module = getOperation();
  mlir::MLIRContext* context = module.getContext();

  std::cerr << "XXXXX module XXXXX: " << module.getName()->str() << std::endl;

  //auto attr = mlir::StringAttr::get("ffmpeg", context);
  for (auto function : module.getOps<mlir::FuncOp>()) {
    std::cerr << "XXXXX function XXXXX: " << function.getOperation()->getName().getStringRef().str() << std::endl;
    //if (failed(CheckSingleBlockFunction(function))) return signalPassFailure();

    //llvm::SmallVector<std::string, 4> var_handle_shared_names;
    //PromoteVarHandlesToArguments(function, /*add_validation=*/false,
    //                             &var_handle_shared_names);

    // Add resource names for each `tf.VarHandleOp` that were promoted to
    // resource arguments.
    //const int var_handle_args_offset =
    //    function.getNumArguments() - var_handle_shared_names.size();
    //for (auto var_name_and_index : llvm::enumerate(var_handle_shared_names))
    //  function.setArgAttr(var_name_and_index.index() + var_handle_args_offset,
    //                      kResourceNameArgAttr,
    //                      StringAttr::get(var_name_and_index.value(), context));
  }

  //op->setAttr("codec", op->getAttr("codec"));

std::cerr << "XXXXX runOnOperation() EXIT XXXXX" << std::endl;

  }
};

std::unique_ptr<mlir::OperationPass<mlir::ModuleOp>> createAudioOptimizationPass() {
  return std::make_unique<AudioOptimizationPass>();
}
//std::unique_ptr<mlir::Pass> createAudioOptimizationPass() {
//  return std::make_unique<AudioOptimizationPass>();
//}

class MlirIOGraphOptimizationPass : public ::tensorflow::MlirOptimizationPass {
 public:
  llvm::StringRef name() const override { return "io_graph_optimization"; }

  bool IsEnabled(const ::tensorflow::ConfigProto& config_proto) const override {
    if (std::getenv("TFIO_GRAPH_DEBUG") == nullptr) {
      VLOG(1) << "Skipping MLIR IO Graph Optimization Pass"
              << ", TFIO_GRAPH_DEBUG not enabled";
      return false;
    }
    return true;
  }

  ::tensorflow::Status Run(const ::tensorflow::ConfigProto& config_proto,
                           mlir::ModuleOp module) override {
    if (std::getenv("TFIO_GRAPH_DEBUG") == nullptr) {
      VLOG(1) << "Skipping MLIR IO Graph Optimization Pass"
              << ", TFIO_GRAPH_DEBUG not enabled";
      return Status::OK();
    }

    VLOG(1) << "Run IO MLIR Graph Optimization Passes";
    mlir::PassManager pm(module.getContext());
    std::string str;
    llvm::raw_string_ostream os(str);
    module.print(os);
    LOG(INFO) << "IO Graph: " << os.str();

    pm.addPass(createAudioOptimizationPass());

    mlir::LogicalResult result = pm.run(module);

    return Status::OK();
  }
};

static mlir_pass_registration::MlirOptimizationPassRegistration
    register_mlir_graph_optimization_pass(
        10, std::make_unique<MlirIOGraphOptimizationPass>());

}  // namespace
}  // namespace io
}  // namespace tensorflow
