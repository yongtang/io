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

#include "tensorflow_io/core/kernels/video_kernels.h"
#include "GenTL.h"
extern "C" {
	#include <dlfcn.h>
#if defined(__APPLE__)
void* VideoCaptureInitFunction(const char* device, int64_t* bytes,
                               int64_t* width, int64_t* height);
void VideoCaptureNextFunction(void* context, void* data, int64_t size);
void VideoCaptureFiniFunction(void* context);
#elif defined(_MSC_VER)
void* VideoCaptureInitFunction(const char* device, int64_t* bytes,
                               int64_t* width, int64_t* height) {
  return NULL;
}
void VideoCaptureNextFunction(void* context, void* data, int64_t size) {}
void VideoCaptureFiniFunction(void* context) {}
#else
void* VideoCaptureInitFunction(const char* device, int64_t* bytes,
                               int64_t* width, int64_t* height) {
  tensorflow::data::VideoCaptureContext* p =
      new tensorflow::data::VideoCaptureContext();
  if (p != nullptr) {
    tensorflow::Status status = p->Init(device, bytes, width, height);
    if (status.ok()) {
      return p;
    }
    LOG(ERROR) << "unable to initialize video capture: " << status;
    delete p;
  }
  return NULL;
}
void VideoCaptureNextFunction(void* context, void* data, int64_t size) {
  tensorflow::data::VideoCaptureContext* p =
      static_cast<tensorflow::data::VideoCaptureContext*>(context);
  if (p != nullptr) {
    tensorflow::Status status = p->Read(data, size);
    if (!status.ok()) {
      LOG(ERROR) << "unable to read video capture: " << status;
    }
  }
}
void VideoCaptureFiniFunction(void* context) {
  tensorflow::data::VideoCaptureContext* p =
      static_cast<tensorflow::data::VideoCaptureContext*>(context);
  if (p != nullptr) {
    delete p;
  }
}
#endif
}
namespace tensorflow {
namespace data {
namespace {

class VideoCaptureReadableResource : public ResourceBase {
 public:
  VideoCaptureReadableResource(Env* env)
      : env_(env), context_(nullptr, [](void* p) {
          if (p != nullptr) {
            VideoCaptureFiniFunction(p);
          }
        }) {}
  ~VideoCaptureReadableResource() {}

  Status Init(const string& input) {
    mutex_lock l(mu_);

    //void *lib = dlopen("/usr/local/lib/python3.6/dist-packages/genicam/TLSimu.cti", RTLD_NOW);
    void *lib = dlopen("/opt/mvIMPACT_Acquire/lib/x86_64/libmvGenTLProducer.so.2.29.0", RTLD_NOW);
    if (lib == nullptr) {
    std::cerr << "XXXXX HANDLE XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PGCInitLib GCInitLib = (GenTL::PGCInitLib)dlsym(lib, "GCInitLib");
    if (GCInitLib == nullptr) {
    std::cerr << "XXXXX SYM GCInitLib XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PTLOpen TLOpen = (GenTL::PTLOpen)dlsym(lib, "TLOpen");
    if (TLOpen == nullptr) {
    std::cerr << "XXXXX SYM TLOpen XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PTLUpdateInterfaceList TLUpdateInterfaceList = (GenTL::PTLUpdateInterfaceList)dlsym(lib, "TLUpdateInterfaceList");
    if (TLUpdateInterfaceList == nullptr) {
    std::cerr << "XXXXX SYM TLUpdateInterfaceList XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PTLGetNumInterfaces TLGetNumInterfaces = (GenTL::PTLGetNumInterfaces)dlsym(lib, "TLGetNumInterfaces");
    if (TLGetNumInterfaces == nullptr) {
    std::cerr << "XXXXX SYM TLGetNumInterfaces XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PTLGetInterfaceID TLGetInterfaceID = (GenTL::PTLGetInterfaceID)dlsym(lib, "TLGetInterfaceID");
    if (TLGetInterfaceID == nullptr) {
    std::cerr << "XXXXX SYM TLGetInterfaceID XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PTLOpenInterface TLOpenInterface = (GenTL::PTLOpenInterface)dlsym(lib, "TLOpenInterface");
    if (TLOpenInterface == nullptr) {
    std::cerr << "XXXXX SYM TLOpenInterface XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PIFUpdateDeviceList IFUpdateDeviceList = (GenTL::PIFUpdateDeviceList)dlsym(lib, "IFUpdateDeviceList");
    if (IFUpdateDeviceList == nullptr) {
    std::cerr << "XXXXX SYM IFUpdateDeviceList XXXXX: " << dlerror() << std::endl;
    }
    GenTL::PIFGetNumDevices IFGetNumDevices = (GenTL::PIFGetNumDevices)dlsym(lib, "IFGetNumDevices(");
    if (IFGetNumDevices == nullptr) {
    std::cerr << "XXXXX SYM TLOpen XXXXX: " << dlerror() << std::endl;
    }
    //GenTL::PTLOpen TLOpen = (GenTL::PTLOpen)dlsym(lib, "TLOpen");
    //if (TLOpen == nullptr) {
    //std::cerr << "XXXXX SYM TLOpen XXXXX: " << dlerror() << std::endl;
    //}
std::cerr << "GenTL::GC_ERROR: " << 0 << std::endl;

GenTL::GC_ERROR err = GCInitLib();
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;

GenTL::TL_HANDLE hTL;
err = TLOpen(&hTL);
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;

bool8_t bChanged;
uint64_t iTimeout = 5000;
err = TLUpdateInterfaceList( hTL, &bChanged, iTimeout );
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;

uint32_t NumIfaces;
err = TLGetNumInterfaces( hTL, &NumIfaces );
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;

std::cerr << "XXXXX NumIfaces XXXXX: " << NumIfaces << std::endl;

string IfaceID;
for (int i = 0; i < NumIfaces; i++) {
size_t iSize;
err = TLGetInterfaceID( hTL, 0, NULL, &iSize );
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;

std::cerr << "XXXXX iSize XXXXX: " << iSize << std::endl;
IfaceID.resize(iSize + 1);

err = TLGetInterfaceID( hTL, 0, &IfaceID[0], &iSize );
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;
std::cerr << "XXXXX IfaceID XXXXX: " << IfaceID << std::endl;
}

GenTL::IF_HANDLE hIface;
err = TLOpenInterface( hTL, &IfaceID[0], &hIface );
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;

err = IFUpdateDeviceList(hIface, &bChanged, iTimeout);
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;
std::cerr << "XXXXXXX" << bChanged << std::endl;
uint32 NumDevices;
err = IFGetNumDevices( hIface, &NumDevices );
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;
std::cerr << "XXXXX NumDevices XXXXX: " << NumDevices << std::endl;


/*
GenTL::GC_ERROR err = GenTL::GCInitLib();
std::cerr << "GenTL::GC_ERROR: " << err << std::endl;
if (err == GenTL::GC_ERR_SUCCESS) {
  err = GenTL::GCCloseLib();
}
*/
//TLOpen( hTL );
//TLUpdateInterfaceList( hTL );
//TLGetNumInterfaces( hTL, NumInterfaces );
//for (i = 0; i < NumInterfaces; i++) {
//TLGetInterfaceID( hTL, 0, IfaceID, &bufferSize );
// TLOpenInterface( hTL, IfaceID, hNewIface );
//
//}

    return errors::Unimplemented("Init");
    int64_t bytes, width, height;
    context_.reset(
        VideoCaptureInitFunction(input.c_str(), &bytes, &width, &height));
    if (context_.get() == nullptr) {
      return errors::InvalidArgument("unable to open device ", input);
    }
    bytes_ = static_cast<int64>(bytes);
    width_ = static_cast<int64>(width);
    height_ = static_cast<int64>(height);
    return Status::OK();
  }
  Status Read(
      std::function<Status(const TensorShape& shape, Tensor** value_tensor)>
          allocate_func) {
    mutex_lock l(mu_);
    return errors::Unimplemented("Read");

    Tensor* value_tensor;
    TF_RETURN_IF_ERROR(allocate_func(TensorShape({1}), &value_tensor));

    string buffer;
    buffer.resize(bytes_);
    VideoCaptureNextFunction(context_.get(), (void*)&buffer[0],
                             static_cast<int64_t>(bytes_));
    value_tensor->flat<string>()(0) = buffer;

    return Status::OK();
  }
  string DebugString() const override {
    mutex_lock l(mu_);
    return "VideoCaptureReadableResource";
  }

 protected:
  mutable mutex mu_;
  Env* env_ GUARDED_BY(mu_);

  std::unique_ptr<void, void (*)(void*)> context_;
  int64 bytes_;
  int64 width_;
  int64 height_;
};

class VideoCaptureReadableInitOp
    : public ResourceOpKernel<VideoCaptureReadableResource> {
 public:
  explicit VideoCaptureReadableInitOp(OpKernelConstruction* context)
      : ResourceOpKernel<VideoCaptureReadableResource>(context) {
    env_ = context->env();
  }

 private:
  void Compute(OpKernelContext* context) override {
    ResourceOpKernel<VideoCaptureReadableResource>::Compute(context);

    const Tensor* input_tensor;
    OP_REQUIRES_OK(context, context->input("input", &input_tensor));
    const string& input = input_tensor->scalar<string>()();

    OP_REQUIRES_OK(context, resource_->Init(input));
  }
  Status CreateResource(VideoCaptureReadableResource** resource)
      EXCLUSIVE_LOCKS_REQUIRED(mu_) override {
    *resource = new VideoCaptureReadableResource(env_);
    return Status::OK();
  }

 private:
  mutable mutex mu_;
  Env* env_ GUARDED_BY(mu_);
};

class VideoCaptureReadableReadOp : public OpKernel {
 public:
  explicit VideoCaptureReadableReadOp(OpKernelConstruction* context)
      : OpKernel(context) {
    env_ = context->env();
  }

  void Compute(OpKernelContext* context) override {
    VideoCaptureReadableResource* resource;
    OP_REQUIRES_OK(context,
                   GetResourceFromContext(context, "input", &resource));
    core::ScopedUnref unref(resource);

    OP_REQUIRES_OK(
        context, resource->Read([&](const TensorShape& shape,
                                    Tensor** value_tensor) -> Status {
          TF_RETURN_IF_ERROR(context->allocate_output(0, shape, value_tensor));
          return Status::OK();
        }));
  }

 private:
  mutable mutex mu_;
  Env* env_ GUARDED_BY(mu_);
};
REGISTER_KERNEL_BUILDER(Name("IO>VideoCaptureReadableInit").Device(DEVICE_CPU),
                        VideoCaptureReadableInitOp);
REGISTER_KERNEL_BUILDER(Name("IO>VideoCaptureReadableRead").Device(DEVICE_CPU),
                        VideoCaptureReadableReadOp);

}  // namespace
}  // namespace data
}  // namespace tensorflow
