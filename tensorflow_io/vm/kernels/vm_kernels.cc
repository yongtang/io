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

#include "tensorflow/core/framework/op_kernel.h"

#include <sys/uio.h>

namespace tensorflow {
namespace data {
namespace {

class VMNumpyOp : public OpKernel {
 public:
  explicit VMNumpyOp(OpKernelConstruction* context) : OpKernel(context) {
    env_ = context->env();
  }

  void Compute(OpKernelContext* context) override {
    const Tensor& process_tensor = context->input(0);
    const int64 process = process_tensor.scalar<int64>()();

    const Tensor& address_tensor = context->input(1);
    const int64 address = address_tensor.scalar<int64>()();

    const Tensor& start_tensor = context->input(2);
    const int64 start = start_tensor.scalar<int64>()();

    const Tensor& stop_tensor = context->input(3);
    int64 count = stop_tensor.scalar<int64>()();

    TensorShape output_shape({});

    Tensor* output_tensor;
    OP_REQUIRES_OK(context, context->allocate_output(0, output_shape, &output_tensor));

    std::cerr << "CURRENT: " << getpid() << std::endl;
    std::cerr << "PROCESS: " << process << std::endl;
    std::cerr << "ADDRESS: " << address << std::endl;
    struct iovec local[1];
    struct iovec remote[1];
    char buffer[8] = {0};
    local[0].iov_base = buffer;
    local[0].iov_len = 8;
    remote[0].iov_base = (void *) address;
    remote[0].iov_len = 20;
    ssize_t nread = process_vm_readv(process, local, 1, remote, 1, 0);
    if (nread != 8) {
      int error = errno;
std::cerr << "ERRNO: " << error << std::endl;
    }
    for (ssize_t i = 0; i < nread; i++) {
      std::cerr << "DATA[" << i << "]: " << int(buffer[i]) << std::endl;
    }
    std::cerr << "RETURN: " << nread << std::endl;
    
  }
 private:
  mutex mu_;
  Env* env_ GUARDED_BY(mu_);
};

REGISTER_KERNEL_BUILDER(Name("VMNumpy").Device(DEVICE_CPU),
                        VMNumpyOp);


}  // namespace
}  // namespace data
}  // namespace tensorflow
