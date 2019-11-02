/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow_io/core/kernels/io_interface.h"
#include "tensorflow_io/core/kernels/io_stream.h"
#include "tensorflow/core/lib/io/buffered_inputstream.h"
#include "hiredis.h"

namespace tensorflow {
namespace data {

class RedisPubSubResource : public ResourceBase {
 public:
  RedisPubSubResource(Env* env)
   : env_(env)
   , index_(0)
   , redis_context_(nullptr, [](redisContext* p) { if (p != nullptr) { redisFree(p); } }) {}
  ~RedisPubSubResource() {}

  Status Init(const std::vector<string>& input, const std::vector<string>& metadata) {
    return Status::OK();
  }
  Status Read(const int64 index, int64* record_read, Tensor* value) {
    if (index < 0) {
      return Status::OK();
    }
    if (index != index_) {
      return errors::InvalidArgument("redis unable to random seek");
    }
    if (index == 0) {
      redis_context_.reset(redisConnect("127.0.0.1", 6379));
      if (redis_context_.get() == nullptr || redis_context_->err) {
        if (redis_context_.get() == nullptr) {
          return errors::InvalidArgument("redis unable to allocate");
        } else {
	  return errors::InvalidArgument("redis error: ", redis_context_->errstr);
        }
      }
    }
    return Status::OK();
  }
  string DebugString() const override {
    return "RedisPubSubResource";
  }
private:
  mutable mutex mu_;
  Env* env_ GUARDED_BY(mu_);
  int64 index_ GUARDED_BY(mu_) = 0;
  std::unique_ptr<redisContext, void(*)(redisContext*)> redis_context_;
};

class RedisPubSubInitOp : public ResourceOpKernel<RedisPubSubResource> {
 public:
  explicit RedisPubSubInitOp(OpKernelConstruction* context)
      : ResourceOpKernel<RedisPubSubResource>(context) {
    env_ = context->env();
  }
 private:
  void Compute(OpKernelContext* context) override {
    ResourceOpKernel<RedisPubSubResource>::Compute(context);

    std::vector<string> input;
    const Tensor* input_tensor;
    OP_REQUIRES_OK(context, context->input("input", &input_tensor));
    for (int64 i = 0; i < input_tensor->NumElements(); i++) {
        input.push_back(input_tensor->flat<string>()(i));
    }

    std::vector<string> metadata;
    const Tensor* metadata_tensor;
    Status status = context->input("metadata", &metadata_tensor);
    for (int64 i = 0; i < metadata_tensor->NumElements(); i++) {
      metadata.push_back(metadata_tensor->flat<string>()(i));
    }

    OP_REQUIRES_OK(context, this->resource_->Init(input, metadata));
  }
  Status CreateResource(RedisPubSubResource** resource)
      EXCLUSIVE_LOCKS_REQUIRED(mu_) override {
    *resource = new RedisPubSubResource(env_);
    return Status::OK();
  }
  mutex mu_;
  Env* env_;
};


class RedisPubSubReadOp : public OpKernel {
 public:
  explicit RedisPubSubReadOp(OpKernelConstruction* ctx)
      : OpKernel(ctx) {
  }

  void Compute(OpKernelContext* context) override {
    RedisPubSubResource* resource;
    OP_REQUIRES_OK(context, GetResourceFromContext(context, "input", &resource));
    core::ScopedUnref unref(resource);

    const Tensor* index_tensor;
    OP_REQUIRES_OK(context, context->input("index", &index_tensor));
    int64 index = index_tensor->scalar<int64>()();

    Tensor* value_tensor = nullptr;
/*
    int64 record_read = 0;
    OP_REQUIRES_OK(context, resource->Read(index, &record_read, value_tensor));
    if (record_read < stop - start) {
      context->set_output(output_index, value.Slice(0, record_read));
      output_index++;
    }
*/
  }
};

REGISTER_KERNEL_BUILDER(Name("IO>RedisPubSubInit").Device(DEVICE_CPU),
                        RedisPubSubInitOp);
REGISTER_KERNEL_BUILDER(Name("IO>RedisPubSubRead").Device(DEVICE_CPU),
                        RedisPubSubReadOp);

}  // namespace data
}  // namespace tensorflow
