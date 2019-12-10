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

#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/View.h>
#include <backend/PixelBufferDescriptor.h>


namespace tensorflow {
namespace data {
namespace {

class GraphicsRenderOp : public OpKernel {
 public:
  explicit GraphicsRenderOp(OpKernelConstruction* context) : OpKernel(context) {}

  void Compute(OpKernelContext* context) override {
    const Tensor* input_tensor;
    OP_REQUIRES_OK(context, context->input("input", &input_tensor));

    Tensor* orientation_tensor = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(0, TensorShape({}), &orientation_tensor));
    orientation_tensor->scalar<int64>()() = 0;

    const string& input = input_tensor->scalar<string>()();

  filament::Engine* pEngine = filament::Engine::create(filament::backend::Backend::METAL);
  filament::SwapChain* pSurface = pEngine->createSwapChain(16, 16);
  filament::Renderer* pRenderer = pEngine->createRenderer();
  filament::Scene* pScene = pEngine->createScene();
  filament::Camera* pCamera = pEngine->createCamera();
  filament::View* pView = pEngine->createView();

  size_t size = 16 * 16 * 4;
  void* buffer = malloc(size);
  memset(buffer, 0, size);
  filament::backend::PixelBufferDescriptor pd(
      buffer, size, filament::backend::PixelDataFormat::RGBA,
      filament::backend::PixelDataType::UBYTE);  // callback, user);

  pRenderer->beginFrame(pSurface);
  pRenderer->render(pView);
  pRenderer->readPixels(0, 0, 16, 16, std::move(pd));
  pRenderer->endFrame();

  pEngine->flushAndWait();

  std::cerr << "Hello, world!" << std::endl;

  free(buffer);

    std::cerr << "COMPLETE" << std::endl;
  }
private:
/*
    static void callback(void* buffer, size_t size, void* user) {
        closure_t* closure = (closure_t *)user;
        uint8_t const* rgba = (uint8_t const*)buffer;
        (*closure)(rgba, 16, 16);
        delete closure;
        ::free(buffer);
    }
*/
};
REGISTER_KERNEL_BUILDER(Name("IO>GraphicsRender").Device(DEVICE_CPU),
                        GraphicsRenderOp);

}  // namespace
}  // namespace data
}  // namespace tensorflow
