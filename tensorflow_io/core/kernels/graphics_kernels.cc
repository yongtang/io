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

    filament::Engine* mEngine = filament::Engine::create(filament::backend::Backend::METAL);
    filament::SwapChain* mSurface = mEngine->createSwapChain(16, 16);
    filament::Renderer* mRenderer = mEngine->createRenderer();
    filament::Scene* mScene = mEngine->createScene();
    filament::Camera* mCamera = mEngine->createCamera();
    filament::View* mView = mEngine->createView();
    mView->setViewport({0, 0, 16, 16});
    std::cerr << "COMPLETE1" << std::endl;
    mView->setScene(mScene);
    std::cerr << "COMPLETE2" << std::endl;
    mView->setCamera(mCamera);
    std::cerr << "COMPLETE3" << std::endl;

    mView->setClearColor(filament::LinearColorA{1, 0, 0, 1});
    std::cerr << "COMPLETE4" << std::endl;
    mView->setToneMapping(filament::View::ToneMapping::LINEAR);
    std::cerr << "COMPLETE5" << std::endl;
    mView->setDithering(filament::View::Dithering::NONE);
    std::cerr << "COMPLETE6" << std::endl;

    size_t size = 16 * 16 * 4;
    void* buffer = malloc(size);
    memset(buffer, 0, size);
    filament::backend::PixelBufferDescriptor pd(buffer, size,
            filament::backend::PixelDataFormat::RGBA, filament::backend::PixelDataType::UBYTE);
    std::cerr << "COMPLETE7" << std::endl;

        filament::Renderer* pRenderer = mRenderer;
    std::cerr << "COMPLETE8" << std::endl;
        pRenderer->beginFrame(mSurface);
    std::cerr << "COMPLETE9" << std::endl;
        pRenderer->render(mView);
    std::cerr << "COMPLETE10" << std::endl;
        pRenderer->readPixels(0, 0, 16, 16, std::move(pd));
    std::cerr << "COMPLETE11" << std::endl;
        pRenderer->endFrame();
    std::cerr << "COMPLETE12" << std::endl;

        // Note: this is where the runTest() callback will be called.
        mEngine->flushAndWait();

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
