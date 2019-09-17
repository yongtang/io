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
#include "tensorflow_io/core/kernels/stream.h"

namespace tensorflow {
namespace data {

// See http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
struct WAVHeader {
  char riff[4];              // RIFF Chunk ID: "RIFF"
  int32 riff_size;           // RIFF chunk size: 4 + n (file size - 8)
  char wave[4];              // WAVE ID: "WAVE"
  char fmt[4];               // fmt Chunk ID: "fmt "
  int32 fmt_size;            // fmt Chunk size: 16, 18, or 40
  int16 wFormatTag;          // Format code: WAVE_FORMAT_PCM (1) for PCM. WAVE_FORMAT_EXTENSIBLE (0xFFFE) for SubFormat
  int16 nChannels;           // Number of channels
  int32 nSamplesPerSec;      // Sampling rate
  int32 nAvgBytesPerSec;     // Data rate
  int16 nBlockAlign;         // Data block size (bytes)
  int16 wBitsPerSample;      // Bits per sample
};

struct ExtensionHeader {
  int16 cbSize;              // Size of the extension (0 or 22)
  int16 wValidBitsPerSample; // Number of valid bits
  int32 dwChannelMask;       // Speaker position mask
  char SubFormat[16];        // GUID
};

struct DataHeader {
  char mark[4];
  int32 size;
};
Status ValidateWAVHeader(struct WAVHeader *header) {
  if (memcmp(header->riff, "RIFF", 4) != 0) {
    return errors::InvalidArgument("WAV file must starts with `RIFF`");
  }
  if (memcmp(header->wave, "WAVE", 4) != 0) {
    return errors::InvalidArgument("WAV file must contains riff type `WAVE`");
  }
  if (memcmp(header->fmt, "fmt ", 4) != 0) {
    return errors::InvalidArgument("WAV file must contains `fmt ` mark");
  }
  if (header->fmt_size != 16 && header->fmt_size != 18 && header->fmt_size != 40) {
    return errors::InvalidArgument("WAV file must have `fmt_size ` 16, 18, or 40, received: ", header->fmt_size);
  }
  if (header->wFormatTag != 1 && header->wFormatTag != static_cast<int16>(0xFFFE)) {
    return errors::InvalidArgument("WAV file must have `wFormatTag` 1 or 0xFFFE, received: ", header->wFormatTag);
  }
  if (header->nChannels <= 0) {
    return errors::InvalidArgument("WAV file have invalide channels: ", header->nChannels);
  }
  return Status::OK();
}

class WAVIndexable : public IOIndexableInterface {
 public:
  WAVIndexable(Env* env)
  : env_(env) {}

  ~WAVIndexable() {}
  Status Init(const std::vector<string>& input, const std::vector<string>& metadata, const void* memory_data, const int64 memory_size) override {
    if (input.size() > 1) {
      return errors::InvalidArgument("more than 1 filename is not supported");
    }
    const string& filename = input[0];
    file_.reset(new SizedRandomAccessFile(env_, filename, memory_data, memory_size));
    TF_RETURN_IF_ERROR(file_->GetFileSize(&file_size_));

    StringPiece result;
    TF_RETURN_IF_ERROR(file_->Read(0, sizeof(header_), &result, (char *)(&header_)));

    TF_RETURN_IF_ERROR(ValidateWAVHeader(&header_));
    if (header_.riff_size + 8 != file_size_) {
      // corrupted file?
    }
    int64 filesize = header_.riff_size + 8;

    int64 position = result.size();

    if (header_.fmt_size != 16) {
      position += header_.fmt_size - 16;
    }

    int64 nSamples = 0;
    do {
      struct DataHeader head;
      TF_RETURN_IF_ERROR(file_->Read(position, sizeof(head), &result, (char *)(&head)));
      position += result.size();
      if (memcmp(head.mark, "data", 4) == 0) {
        // Data should be block aligned
        // bytes = nSamples * nBlockAlign
        if (head.size % header_.nBlockAlign != 0) {
          return errors::InvalidArgument("data chunk should be block aligned (", header_.nBlockAlign, "), received: ", head.size);
        }
        nSamples += head.size / header_.nBlockAlign;
      }
      position += head.size;
    } while (position < filesize);

    switch (header_.wBitsPerSample) {
    case 8:
      dtype_ = DT_INT8;
      break;
    case 16:
      dtype_ = DT_INT16;
      break;
    case 24:
      dtype_ = DT_INT32;
      break;
    default:
      return errors::InvalidArgument("unsupported wBitsPerSample: ", header_.wBitsPerSample);
    }

    shape_ = TensorShape({nSamples, header_.nChannels});

    return Status::OK();
  }
  Status Spec(const Tensor& component, PartialTensorShape* shape, DataType* dtype) override {
    *shape = shape_;
    *dtype = dtype_;
    return Status::OK();
  }

  Status Extra(const Tensor& component, std::vector<Tensor>* extra) override {
    // Expose a sample `rate`
    Tensor rate(DT_INT32, TensorShape({}));
    rate.scalar<int32>()() = header_.nSamplesPerSec;
    extra->push_back(rate);
    return Status::OK();
  }

  Status GetItem(const int64 start, const int64 stop, const int64 step, const Tensor& component, Tensor* tensor) override {
    if (step != 1) {
      return errors::InvalidArgument("step ", step, " is not supported");
    }

    const int64 sample_start = start;
    const int64 sample_stop = stop;

    int64 sample_offset = 0;
    if (header_.riff_size + 8 != file_size_) {
      // corrupted file?
    }
    int64 filesize = header_.riff_size + 8;
    int64 position = sizeof(header_) + header_.fmt_size - 16;
    do {
      StringPiece result;
      struct DataHeader head;
      TF_RETURN_IF_ERROR(file_->Read(position, sizeof(head), &result, (char *)(&head)));
      position += result.size();
      if (memcmp(head.mark, "data", 4) == 0) {
        // Already checked the alignment
        int64 block_sample_start = sample_offset;
        int64 block_sample_stop = sample_offset + head.size / header_.nBlockAlign;
        // only read if block_sample_start and block_sample_stop within range
        if (sample_start < block_sample_stop && sample_stop > block_sample_start) {
          int64 read_sample_start = (block_sample_start > sample_start ? block_sample_start : sample_start);
          int64 read_sample_stop = (block_sample_stop < sample_stop ? block_sample_stop : sample_stop);
          int64 read_bytes_start = position + (read_sample_start - block_sample_start) * header_.nBlockAlign;
          int64 read_bytes_stop = position + (read_sample_stop - block_sample_start) * header_.nBlockAlign;
          string buffer;
          buffer.resize(read_bytes_stop - read_bytes_start);
          TF_RETURN_IF_ERROR(file_->Read(read_bytes_start, read_bytes_stop - read_bytes_start, &result, &buffer[0]));
          switch (header_.wBitsPerSample) {
          case 8:
            if (header_.wBitsPerSample * header_.nChannels != header_.nBlockAlign * 8) {
              return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
            }
            memcpy((char *)(tensor->flat<int8>().data()) + ((read_sample_start - sample_start) * header_.nBlockAlign), &buffer[0], (read_bytes_stop - read_bytes_start));
            break;
          case 16:
            if (header_.wBitsPerSample * header_.nChannels != header_.nBlockAlign * 8) {
              return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
            }
            memcpy((char *)(tensor->flat<int16>().data()) + ((read_sample_start - sample_start) * header_.nBlockAlign), &buffer[0], (read_bytes_stop - read_bytes_start));
            break;
          case 24:
            // NOTE: The conversion is from signed integer 24 to signed integer 32 (left shift 8 bits)
            if (header_.wBitsPerSample * header_.nChannels != header_.nBlockAlign * 8) {
              return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
            }
            for (int64 i = read_sample_start; i < read_sample_stop; i++) {
              for (int64 j = 0; j < header_.nChannels; j++) {
                char *data_p = (char *)(tensor->flat<int32>().data() + ((i - sample_start) * header_.nChannels + j));
                char *read_p = (char *)(&buffer[((i - read_sample_start) * header_.nBlockAlign)]) + 3 * j;
                data_p[3] = read_p[2];
                data_p[2] = read_p[1];
                data_p[1] = read_p[0];
                data_p[0] = 0x00;
              }
            }
            break;
          default:
            return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
          }
        }
        sample_offset = block_sample_stop;
      }
      position += head.size;
    } while (position < filesize);

    return Status::OK();
  }

  string DebugString() const override {
    mutex_lock l(mu_);
    return strings::StrCat("WAVIndexable");
  }
 private:
  mutable mutex mu_;
  Env* env_ GUARDED_BY(mu_);
  std::unique_ptr<SizedRandomAccessFile> file_ GUARDED_BY(mu_);
  uint64 file_size_ GUARDED_BY(mu_);
  DataType dtype_;
  TensorShape shape_;
  struct WAVHeader header_;
};

REGISTER_KERNEL_BUILDER(Name("WAVIndexableInit").Device(DEVICE_CPU),
                        IOInterfaceInitOp<WAVIndexable>);
REGISTER_KERNEL_BUILDER(Name("WAVIndexableSpec").Device(DEVICE_CPU),
                        IOInterfaceSpecOp<WAVIndexable>);
REGISTER_KERNEL_BUILDER(Name("WAVIndexableGetItem").Device(DEVICE_CPU),
                        IOIndexableGetItemOp<WAVIndexable>);

class WAVReadable : public ResourceBase {
 public:
  WAVReadable(Env* env)
  : env_(env) {}

  ~WAVReadable() {}
  Status Init(const std::vector<string>& input, const std::vector<string>& metadata, const void* memory_data, const int64 memory_size) {
    if (input.size() > 1) {
      return errors::InvalidArgument("more than 1 filename is not supported");
    }
    const string& filename = input[0];
    file_.reset(new SizedRandomAccessFile(env_, filename, memory_data, memory_size));
    TF_RETURN_IF_ERROR(file_->GetFileSize(&file_size_));

    StringPiece result;
    TF_RETURN_IF_ERROR(file_->Read(0, sizeof(header_), &result, (char *)(&header_)));

    TF_RETURN_IF_ERROR(ValidateWAVHeader(&header_));
    if (header_.riff_size + 8 != file_size_) {
      // corrupted file?
    }
    DataType dtype;
    switch (header_.wBitsPerSample) {
    case 8:
      dtype = DT_INT8;
      break;
    case 16:
      dtype = DT_INT16;
      break;
    case 24:
      dtype = DT_INT32;
      break;
    default:
      return errors::InvalidArgument("unsupported wBitsPerSample: ", header_.wBitsPerSample);
    }

    PartialTensorShape shape = PartialTensorShape({-1, header_.nChannels});

    shapes_.clear();
    shapes_.push_back(shape);

    dtypes_.clear();
    dtypes_.push_back(dtype);

    return Status::OK();
  }
  Status Components(std::vector<PartialTensorShape>* shapes, std::vector<DataType>* dtypes) {
    for (size_t i = 0; i < shapes_.size(); i++) {
      shapes->push_back(shapes_[i]);
    }
    for (size_t i = 0; i < dtypes_.size(); i++) {
      dtypes->push_back(dtypes_[i]);
    }
    return Status::OK();
  }
  Status Spec(const Tensor& component, const int64 items, TensorShape* shape, DataType* dtype) {
    gtl::InlinedVector<int64, 4> dims = shapes_[0].dim_sizes();
    dims[0] = items;
    *shape = TensorShape(dims);
    *dtype = dtypes_[0];
    return Status::OK();
  }

  Status Partitions(const int64 capacity, std::vector<int64>* partitions) {
    partitions->clear();

    int64 filesize = header_.riff_size + 8;

    int64 position = sizeof(header_);

    if (header_.fmt_size != 16) {
      position += header_.fmt_size - 16;
    }

    int64 nSamples = 0;
    do {
      StringPiece result;
      struct DataHeader head;
      TF_RETURN_IF_ERROR(file_->Read(position, sizeof(head), &result, (char *)(&head)));
      position += result.size();
      if (memcmp(head.mark, "data", 4) == 0) {
        // Data should be block aligned
        // bytes = nSamples * nBlockAlign
        if (head.size % header_.nBlockAlign != 0) {
          return errors::InvalidArgument("data chunk should be block aligned (", header_.nBlockAlign, "), received: ", head.size);
        }

        // In case capacity is -1, we will just fill in items and positions
        // else cut this portion into smaller chunks.
        if (capacity <= 0) {
          partitions->push_back(head.size / header_.nBlockAlign);
        } else {
          int64 chunks = head.size / header_.nBlockAlign / capacity;
          int64 remain = head.size / header_.nBlockAlign % capacity;
          for (int64 i = 0; i < chunks; i++) {
            partitions->push_back(capacity);
          }
          if (remain != 0) {
            partitions->push_back(remain);
          }
        }
        nSamples += head.size / header_.nBlockAlign;
      }
      position += head.size;
    } while (position < filesize);

    return Status::OK();
  }

  Status Extra(std::vector<Tensor>* extra) {
    // Expose a sample `rate`, per component
    Tensor rate(DT_INT32, TensorShape({1}));
    rate.flat<int32>()(0) = header_.nSamplesPerSec;
    extra->push_back(rate);
    return Status::OK();
  }

  Status Read(const int64 start, const int64 stop, const Tensor& component, Tensor* tensor) {
    const int64 sample_start = start;
    const int64 sample_stop = stop;

    int64 sample_offset = 0;
    if (header_.riff_size + 8 != file_size_) {
      // corrupted file?
    }
    int64 filesize = header_.riff_size + 8;
    int64 position = sizeof(header_) + header_.fmt_size - 16;
    do {
      StringPiece result;
      struct DataHeader head;
      TF_RETURN_IF_ERROR(file_->Read(position, sizeof(head), &result, (char *)(&head)));
      position += result.size();
      if (memcmp(head.mark, "data", 4) == 0) {
        // Already checked the alignment
        int64 block_sample_start = sample_offset;
        int64 block_sample_stop = sample_offset + head.size / header_.nBlockAlign;
        // only read if block_sample_start and block_sample_stop within range
        if (sample_start < block_sample_stop && sample_stop > block_sample_start) {
          int64 read_sample_start = (block_sample_start > sample_start ? block_sample_start : sample_start);
          int64 read_sample_stop = (block_sample_stop < sample_stop ? block_sample_stop : sample_stop);
          int64 read_bytes_start = position + (read_sample_start - block_sample_start) * header_.nBlockAlign;
          int64 read_bytes_stop = position + (read_sample_stop - block_sample_start) * header_.nBlockAlign;
          string buffer;
          buffer.resize(read_bytes_stop - read_bytes_start);
          TF_RETURN_IF_ERROR(file_->Read(read_bytes_start, read_bytes_stop - read_bytes_start, &result, &buffer[0]));
          switch (header_.wBitsPerSample) {
          case 8:
            if (header_.wBitsPerSample * header_.nChannels != header_.nBlockAlign * 8) {
              return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
            }
            memcpy((char *)(tensor->flat<int8>().data()) + ((read_sample_start - sample_start) * header_.nBlockAlign), &buffer[0], (read_bytes_stop - read_bytes_start));
            break;
          case 16:
            if (header_.wBitsPerSample * header_.nChannels != header_.nBlockAlign * 8) {
              return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
            }
            memcpy((char *)(tensor->flat<int16>().data()) + ((read_sample_start - sample_start) * header_.nBlockAlign), &buffer[0], (read_bytes_stop - read_bytes_start));
            break;
          case 24:
            // NOTE: The conversion is from signed integer 24 to signed integer 32 (left shift 8 bits)
            if (header_.wBitsPerSample * header_.nChannels != header_.nBlockAlign * 8) {
              return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
            }
            for (int64 i = read_sample_start; i < read_sample_stop; i++) {
              for (int64 j = 0; j < header_.nChannels; j++) {
                char *data_p = (char *)(tensor->flat<int32>().data() + ((i - sample_start) * header_.nChannels + j));
                char *read_p = (char *)(&buffer[((i - read_sample_start) * header_.nBlockAlign)]) + 3 * j;
                data_p[3] = read_p[2];
                data_p[2] = read_p[1];
                data_p[1] = read_p[0];
                data_p[0] = 0x00;
              }
            }
            break;
          default:
            return errors::InvalidArgument("unsupported wBitsPerSample and header.nBlockAlign: ", header_.wBitsPerSample, ", ", header_.nBlockAlign);
          }
        }
        sample_offset = block_sample_stop;
      }
      position += head.size;
    } while (position < filesize);

    return Status::OK();
  }

  string DebugString() const override {
    mutex_lock l(mu_);
    return strings::StrCat("WAVReadable");
  }

 private:
  mutable mutex mu_;
  Env* env_ GUARDED_BY(mu_);
  std::unique_ptr<SizedRandomAccessFile> file_ GUARDED_BY(mu_);
  uint64 file_size_ GUARDED_BY(mu_);
  struct WAVHeader header_;
  std::vector<PartialTensorShape> shapes_;
  std::vector<DataType> dtypes_;
};

template<typename IOReadableType>
class IOReadableSpecOp : public OpKernel {
 public:
  explicit IOReadableSpecOp(OpKernelConstruction* context) : OpKernel(context) {
    env_ = context->env();
  }

  void Compute(OpKernelContext* context) override {
    std::vector<string> input;
    const Tensor* input_tensor;
    OP_REQUIRES_OK(context, context->input("input", &input_tensor));
    for (int64 i = 0; i < input_tensor->NumElements(); i++) {
        input.push_back(input_tensor->flat<string>()(i));
    }

    Status status;

    std::vector<string> metadata;
    const Tensor* metadata_tensor;
    status = context->input("metadata", &metadata_tensor);
    if (status.ok()) {
      for (int64 i = 0; i < metadata_tensor->NumElements(); i++) {
        metadata.push_back(metadata_tensor->flat<string>()(i));
      }
    }

    size_t memory_size = 0;
    const void *memory_data = nullptr;
    const Tensor* memory_tensor;
    status = context->input("memory", &memory_tensor);
    if (status.ok()) {
      memory_data = memory_tensor->scalar<string>()().data();
      memory_size = memory_tensor->scalar<string>()().size();
    }

    int64 capacity = -1;
    const Tensor* capacity_tensor;
    status = context->input("capacity", &capacity_tensor);
    if (status.ok()) {
      capacity = capacity_tensor->scalar<int64>()();
    }

    std::unique_ptr<IOReadableType> readable(new IOReadableType(env_));
    OP_REQUIRES_OK(context, readable->Init(input, metadata, memory_data, memory_size));

    std::vector<PartialTensorShape> shapes;
    std::vector<DataType> dtypes;
    OP_REQUIRES_OK(context, readable->Components(&shapes, &dtypes));
    OP_REQUIRES(context, shapes.size() == dtypes.size(), errors::InvalidArgument("components should have equal shapes and dtypes: ", shapes.size(), " vs. ", dtypes.size()));

    int64 maxrank = 0;
    for (size_t component = 0; component < shapes.size(); component++) {
      maxrank = maxrank > shapes[component].dims() ? maxrank : shapes[component].dims();
    }
    Tensor shapes_tensor(DT_INT64, TensorShape({static_cast<int64>(shapes.size()), maxrank}));
    for (size_t component = 0; component < shapes.size(); component++) {
      for (int64 i = 0; i < shapes[component].dims(); i++) {
        shapes_tensor.flat<int64>()(component * maxrank + i) = shapes[component].dim_size(i);
      }
      for (int64 i = shapes[component].dims(); i < maxrank; i++) {
        shapes_tensor.flat<int64>()(component * maxrank + i) = 0;
      }
    }
    Tensor dtypes_tensor(DT_INT64, TensorShape({static_cast<int64>(dtypes.size())}));
    for (size_t component = 0; component < dtypes.size(); component++) {
      dtypes_tensor.flat<int64>()(component) = dtypes[component];
    }

    context->set_output(0, shapes_tensor);
    context->set_output(1, dtypes_tensor);

    std::vector<int64> partitions;
    OP_REQUIRES_OK(context, readable->Partitions(capacity, &partitions));
    Tensor partitions_tensor(DT_INT64, TensorShape({static_cast<int64>(partitions.size())}));
    for (size_t partition_index = 0; partition_index < partitions.size(); partition_index++) {
      partitions_tensor.flat<int64>()(partition_index) = partitions[partition_index];
    }
    context->set_output(2, partitions_tensor);

    std::vector<Tensor> extra;
    status = readable->Extra(&extra);
    if (!errors::IsUnimplemented(status)) {
      OP_REQUIRES_OK(context, status);
      for (size_t i = 0; i < extra.size(); i++) {
        context->set_output(3 + i, extra[i]);
      }
    }
  }
 private:
  mutable mutex mu_;
  Env* env_ GUARDED_BY(mu_);
};

template<typename IOReadableType>
class IOReadableInitOp : public ResourceOpKernel<IOReadableType> {
 public:
  explicit IOReadableInitOp<IOReadableType>(OpKernelConstruction* context)
      : ResourceOpKernel<IOReadableType>(context) {
    env_ = context->env();
  }
 private:
  void Compute(OpKernelContext* context) override {
    ResourceOpKernel<IOReadableType>::Compute(context);

    Status status;

    std::vector<string> input;
    const Tensor* input_tensor;
    OP_REQUIRES_OK(context, context->input("input", &input_tensor));
    for (int64 i = 0; i < input_tensor->NumElements(); i++) {
        input.push_back(input_tensor->flat<string>()(i));
    }

    std::vector<string> metadata;
    const Tensor* metadata_tensor;
    status = context->input("metadata", &metadata_tensor);
    if (status.ok()) {
      for (int64 i = 0; i < metadata_tensor->NumElements(); i++) {
        metadata.push_back(metadata_tensor->flat<string>()(i));
      }
    }

    const void *memory_data = nullptr;
    size_t memory_size = 0;

    const Tensor* memory_tensor;
    status = context->input("memory", &memory_tensor);
    if (status.ok()) {
      memory_data = memory_tensor->scalar<string>()().data();
      memory_size = memory_tensor->scalar<string>()().size();
    }

    OP_REQUIRES_OK(context, this->resource_->Init(input, metadata, memory_data, memory_size));
  }
  Status CreateResource(IOReadableType** resource)
      EXCLUSIVE_LOCKS_REQUIRED(mu_) override {
    *resource = new IOReadableType(env_);
    return Status::OK();
  }
  mutex mu_;
  Env* env_;
};

template<typename IOReadableType>
class IOReadableReadOp : public OpKernel {
 public:
  explicit IOReadableReadOp<IOReadableType>(OpKernelConstruction* ctx)
      : OpKernel(ctx) {
  }

  void Compute(OpKernelContext* context) override {
    IOReadableType* resource;
    OP_REQUIRES_OK(context, GetResourceFromContext(context, "input", &resource));
    core::ScopedUnref unref(resource);

    const Tensor* start_tensor;
    OP_REQUIRES_OK(context, context->input("start", &start_tensor));
    int64 start = start_tensor->scalar<int64>()();

    const Tensor* stop_tensor;
    OP_REQUIRES_OK(context, context->input("stop", &stop_tensor));
    int64 stop = stop_tensor->scalar<int64>()();

    Tensor component_empty(DT_INT64, TensorShape({}));
    component_empty.scalar<int64>()() = 0;
    const Tensor* component;
    Status status = context->input("component", &component);
    if (!status.ok()) {
      component = &component_empty;
    }

    TensorShape shape;
    DataType dtype;
    OP_REQUIRES_OK(context, resource->Spec(*component, stop - start, &shape, &dtype));

    Tensor tensor(dtype, shape);
    OP_REQUIRES_OK(context, resource->Read(start, stop, *component, &tensor));
    context->set_output(0, tensor);
  }
};
REGISTER_KERNEL_BUILDER(Name("WAVReadableSpec").Device(DEVICE_CPU),
                        IOReadableSpecOp<WAVReadable>);
REGISTER_KERNEL_BUILDER(Name("WAVReadableInit").Device(DEVICE_CPU),
                        IOReadableInitOp<WAVReadable>);
REGISTER_KERNEL_BUILDER(Name("WAVReadableRead").Device(DEVICE_CPU),
                        IOReadableReadOp<WAVReadable>);

}  // namespace data
}  // namespace tensorflow
