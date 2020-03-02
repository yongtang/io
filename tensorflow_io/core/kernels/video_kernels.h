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

#include "tensorflow/core/framework/resource_mgr.h"
#include "tensorflow/core/framework/resource_op_kernel.h"

#if defined(__linux__)

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/videodev2.h>

static int xioctl(int fh, int request, void* arg) {
  int r;

  do {
    r = ioctl(fh, request, arg);
  } while (-1 == r && EINTR == errno);

  return r;
}
namespace tensorflow {
namespace data {

class VideoCaptureContext {
 public:
  VideoCaptureContext()
      : context_(nullptr,
                 [](void* p) {
                   if (p != nullptr) {
                     free(p);
                   }
                 }),
        fd_scope_(nullptr, [](int* p) {
          if (p != nullptr) {
            close(*p);
          }
        }) {}
  ~VideoCaptureContext() {}

  Status Init(const string& device) {
    device_ = device;

    const char* devname = device.c_str();
    struct stat st;
    if (-1 == stat(devname, &st)) {
      return errors::InvalidArgument("cannot identify '", devname, "': ", errno,
                                     ", ", strerror(errno));
    }

    if (!S_ISCHR(st.st_mode)) {
      return errors::InvalidArgument(devname, " is no device");
    }

    fd_ = open(devname, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == fd_) {
      return errors::InvalidArgument("cannot open '", devname, "': ", errno,
                                     ", ", strerror(errno));
    }
    fd_scope_.reset(&fd_);

    struct v4l2_capability cap;
    if (-1 == xioctl(fd_, VIDIOC_QUERYCAP, &cap)) {
      if (EINVAL == errno) {
        return errors::InvalidArgument(devname, " is no V4L2 device");
      } else {
        return errors::InvalidArgument("cannot VIDIOC_QUERYCAP '", devname,
                                       "': ", errno, ", ", strerror(errno));
      }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
      return errors::InvalidArgument(devname, " is no video capture device");
    }


                if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                        return errors::InvalidArgument(devname, " does not support streaming i/o");
                }
		std::cerr << "XXXXX START XXXXX" << std::endl;
	if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
		struct v4l2_fmtdesc     fmtdesc;
		struct v4l2_format      format;
		int tab = 1;
		printf("video capture\n");
		for (int i = 0;; i++) {
			memset(&fmtdesc,0,sizeof(fmtdesc));
			fmtdesc.index = i;
			fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == ioctl(fd_,VIDIOC_ENUM_FMT,&fmtdesc)) {
		printf("video capture2\n");
				break;
			}
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_CAPTURE)\n",i);
			//print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == ioctl(fd_,VIDIOC_G_FMT,&format)) {
		printf("video capture3\n");
			perror("VIDIOC_G_FMT(VIDEO_CAPTURE)");
		} else {
			printf("    VIDIOC_G_FMT(VIDEO_CAPTURE)\n");
			//print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		printf("\n");
	}

    struct v4l2_format fmt;
    memset(&(fmt), 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   if (-1 == xioctl(fd_, VIDIOC_G_FMT, &fmt)) {
        return errors::InvalidArgument("cannot VIDIOC_G_FMT '", devname,
                                       "': ", errno, ", ", strerror(errno));
        }
    std::cerr << "SUCCESS: " << fd_ << std::endl;
    return Status::OK();
  }

 protected:
  mutable mutex mu_;

  std::unique_ptr<void, void (*)(void*)> context_;
  std::unique_ptr<int, void (*)(int*)> fd_scope_;
  string device_;
  int fd_;
};

}  // namespace data
}  // namespace tensorflow
#endif
