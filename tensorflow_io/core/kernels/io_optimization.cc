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
#include "mlir/IR/OperationSupport.h"              // from @llvm-project
#include "mlir/Pass/PassManager.h"       // from @llvm-project
#include "mlir/Support/LogicalResult.h"  // from @llvm-project
#include "mlir/Pass/Pass.h"  // from @llvm-project
#include "mlir/Pass/PassRegistry.h"  // from @llvm-project
#include "mlir/IR/PatternMatch.h"  // from @llvm-project
#include "tensorflow/compiler/mlir/mlir_graph_optimization_pass.h"
#include "tensorflow/compiler/mlir/tensorflow/utils/error_util.h"
#include "tensorflow/compiler/mlir/tensorflow/utils/translate_utils.h"
#include "tensorflow/compiler/mlir/tensorflow/transforms/passes.h"
#include "tensorflow/compiler/mlir/tensorflow/ir/tf_ops.h"
#include "tensorflow/compiler/mlir/tensorflow/ir/tf_types.h"

namespace tensorflow {
namespace io {
namespace {

// Replace TF BatchMatMul by TF Einsum
struct BatchMatMulToEinsumPass
    : public mlir::PassWrapper<BatchMatMulToEinsumPass, mlir::FunctionPass> {
  void runOnFunction() override;
};

struct FuseParallelMapAndBatch : public mlir::OpRewritePattern<mlir::TF::AddV2Op> {
  using mlir::OpRewritePattern<mlir::TF::AddV2Op>::OpRewritePattern;

  mlir::LogicalResult matchAndRewrite(mlir::TF::AddV2Op op,
                                mlir::PatternRewriter &rewriter) const override {
std::cerr << "MATCH AND REWRITE" << std::endl;
/*
    auto batchInputDataset = op.input_dataset();

    ParallelMapDatasetOp batchInputOp = dyn_cast_or_null<ParallelMapDatasetOp>(
        batchInputDataset.getDefiningOp());
    if (!batchInputOp) return failure();

    // The type of the `num_parallel_calls` argument in ParallelMapDataset
    // and MapAndBatchDataset is different (int32 and int64 respectively)
    auto num_parallel_calls_op = rewriter.create<CastOp>(
        op.getLoc(), UnrankedTensorType::get(rewriter.getIntegerType(64)),
        batchInputOp.num_parallel_calls(), rewriter.getBoolAttr(false));

    auto fused_op = rewriter.create<MapAndBatchDatasetOp>(
        op.getLoc(), op.getType(), batchInputOp.input_dataset(),
        batchInputOp.other_arguments(), op.batch_size(),
        num_parallel_calls_op.y(), op.drop_remainder(), batchInputOp.f(),
        op.output_types(), op.output_shapes(),
        batchInputOp.preserve_cardinality());
    rewriter.replaceOp(op, {fused_op.handle()});
    return failure();
*/
return success();
  }
};

void BatchMatMulToEinsumPass::runOnFunction() {
  mlir::OwningRewritePatternList patterns;
  auto func = getFunction();

  patterns.insert<FuseParallelMapAndBatch>(
      &getContext());
  applyPatternsAndFoldGreedily(func, patterns);
}

static mlir::PassRegistration<BatchMatMulToEinsumPass> pass(
    "tf-batch-matmul-to-tf-einsum",
    "Replace TF BatchMatMul op by TF Einsum op.");

std::unique_ptr<mlir::OperationPass<mlir::FuncOp>> CreateAudioOptimizationPass() {
  return std::make_unique<BatchMatMulToEinsumPass>();
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

    pm.addPass(CreateAudioOptimizationPass());
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
