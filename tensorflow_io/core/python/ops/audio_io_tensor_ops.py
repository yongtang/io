# Copyright 2018 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""AudioIOTensor"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import uuid

import tensorflow as tf
from tensorflow_io.core.python.ops import io_tensor_ops
from tensorflow_io.core.python.ops import core_ops

class AudioIOTensor(io_tensor_ops.BaseIOTensor): # pylint: disable=protected-access
  """AudioIOTensor

  An `AudioIOTensor` is an `IOTensor` backed by audio files such as WAV
  format. It consists of only one `Tensor` with `shape` defined as
  `[n_samples, n_channels]`. It is a subclass of `BaseIOTensor`
  with additional `rate` property exposed, indicating the sample rate
  of the audio.
  """

  #=============================================================================
  # Constructor (private)
  #=============================================================================
  def __init__(self,
               filename,
               internal=False):
    with tf.name_scope("AudioIOTensor") as scope:
      shapes, dtypes, partitions, rates = core_ops.wav_readable_spec(
          filename, capacity=4096)
      shapes = tf.unstack(shapes)
      dtypes = tf.unstack(dtypes)
      rates = tf.unstack(rates)
      assert (len(shapes), len(dtypes), len(rates)) == (1, 1, 1)

      length = tf.reduce_sum(partitions).numpy()
      shape = tf.TensorShape([length]).concatenate(shapes[0][1:].numpy())
      dtype = tf.as_dtype(dtypes[0].numpy())
      spec = tf.TensorSpec(shape, dtype)

      partitions = partitions.numpy()

      rate = rates[0].numpy()

      self._rate = rate

      def function_init():
        return core_ops.wav_readable_init(filename)
      def function_read(resource, start, stop, dtype):
        return core_ops.wav_readable_read(resource, start, stop, dtype=dtype)

      super(AudioIOTensor, self).__init__(
          spec, partitions,
          function_init, function_read,
          internal=internal)

  #=============================================================================
  # Accessors
  #=============================================================================

  @io_tensor_ops._IOTensorMeta # pylint: disable=protected-access
  def rate(self):
    """The sample `rate` of the audio stream"""
    return self._rate
