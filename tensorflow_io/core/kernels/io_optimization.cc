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

namespace tensorflow {
namespace io {
namespace {

class AudioOptimizationPass
    : public mlir::PassWrapper<AudioOptimizationPass, mlir::FunctionPass> {
 public:
  void runOnFunction() override {
std::cerr << "XXXXX runOnFunction() ENTER XXXXX" << std::endl;

 auto f = getFunction();
    f.walk([&](mlir::Operation *op) {
std::cerr << "XXXXX runOnFunction() NAME XXXXX: " << op->getName().getStringRef().str() << std::endl;
std::cerr << "XXXXX runOnFunction() DIALECT XXXXX: " << op->getName().getDialect().str() << std::endl;
    });
/*
    mlir::ConversionTarget target(getContext());

    target.addDynamicallyLegalDialect<mlir::TF::TensorFlowDialect>([](mlir::Operation *op -> bool {
std::cerr << "XXXXX DynamicallyLegal() NAME XXXXX: " << op->getName().getStringRef().str() << std::endl;
std::cerr << "XXXXX DynamicallyLegal() DIALECT XXXXX: " << op->getName().getDialect().str() << std::endl;

      return false;
    });

    OwningRewritePatternList patterns;
    patterns.insert<OptimizingAudioOp>(&getContext());
    if (failed(applyPartialConversion(getFunction(), target, patterns))) {
      signalPassFailure();
    }
*/
    // Define the dialects that are legal targets.
    //target.addLegalDialect<AffineDialect, StandardOpsDialect>();

    // Define the Foo dialect as Illegal, so all operatsions are converted.
    // Explicitly mark the Foo operations, `foo.print`, as `legal`.
    //target.addIllegalDialect<foo::FooDialect>();
    //target.addLegalOp<foo::PrintOp>();

    // Provide the set of patterns that will lower the Foo operations.
    //OwningRewritePatternList patterns;
    //patterns.insert<LoweringConstOp, LoweringReturnOp>(&getContext());

    // Signal failure if any `illegal` operations were not converted
    // successfully.
    //if (failed(applyPartialConversion(getFunction(), target, patterns))) {
    //  signalPassFailure();
    //}



std::cerr << "XXXXX runOnFunction() EXIT XXXXX" << std::endl;

  }
};

std::unique_ptr<mlir::Pass> createAudioOptimizationPass() {
  return std::make_unique<AudioOptimizationPass>();
}

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
