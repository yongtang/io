/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/core/framework/common_shape_fns.h"
#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/shape_inference.h"

namespace tensorflow {

REGISTER_OP("GRPCIOServerInit")
  .Input("input: string")
  .Input("metadata: string")
  .Output("output: resource")
  .Attr("container: string = ''")
  .Attr("shared_name: string = ''")
  .SetShapeFn([](shape_inference::InferenceContext* c) {
    c->set_output(0, c->Scalar());
    return Status::OK();
   });

REGISTER_OP("GRPCIOServerIterableNext")
  .Input("input: resource")
  .Input("capacity: int64")
  .Input("component: string")
  .Output("value: dtype")
  .Output("label: label_dtype")
  .Attr("shape: shape")
  .Attr("dtype: type")
  .Attr("label_shape: shape")
  .Attr("label_dtype: type")
  .SetShapeFn([](shape_inference::InferenceContext* c) {
    PartialTensorShape shape;
    TF_RETURN_IF_ERROR(c->GetAttr("shape", &shape));
    shape_inference::ShapeHandle entry;
    TF_RETURN_IF_ERROR(c->MakeShapeFromPartialTensorShape(shape, &entry));
    c->set_output(0, entry);

    PartialTensorShape label_shape;
    TF_RETURN_IF_ERROR(c->GetAttr("label_shape", &label_shape));
    shape_inference::ShapeHandle label_entry;
    TF_RETURN_IF_ERROR(c->MakeShapeFromPartialTensorShape(label_shape, &label_entry));
    c->set_output(1, label_entry);

    return Status::OK();
   });

REGISTER_OP("GRPCInput")
    .Input("source: string")
    .Output("handle: variant")
    .Attr("columns: list(string) = []")
    .Attr("schema: string = ''")
    .SetShapeFn([](shape_inference::InferenceContext* c) {
       c->set_output(0, c->MakeShape({c->UnknownDim()}));
       return Status::OK();
     });

REGISTER_OP("GRPCDataset")
    .Input("input: T")
    .Input("batch: int64")
    .Output("handle: variant")
    .Attr("output_types: list(type) >= 1")
    .Attr("output_shapes: list(shape) >= 1")
    .Attr("T: {string, variant} = DT_VARIANT")
    .SetIsStateful()
    .SetShapeFn([](shape_inference::InferenceContext* c) {
       c->set_output(0, c->MakeShape({}));
       return Status::OK();
     });

}  // namespace tensorflow
