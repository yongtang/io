# Copyright 2020 The TensorFlow Authors. All Rights Reserved.
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
"""audio"""

import tensorflow as tf

from tensorflow_io.core.python.ops import core_ops

def info(input, name=None): # pylint: disable=redefined-builtin
  """Gets metadata from content of an audio file.

  Args:
    input: A string `Tensor` of the audio input.
    name: A name for the operation (optional).

  Returns:
    shape: The shape `[samples, channels]` of the audio.
    dtype: The dtype of the audio.
    rate: The sample rate.
    encoding: The format of the audio (e.g., "wav").
  """
  shape, dtype, rate, encoding = core_ops.io_audio_info(input, name=name)
  return shape, dtype, rate, encoding

def resample(input, rate_in, rate_out, quality, name=None): # pylint: disable=redefined-builtin
  """Resample audio.

  Args:
    input: A 2-D `Tensor` of type `int16` or `float`. Audio input.
    rate_in: The rate of the audio input.
    rate_out: The rate of the audio output.
    quality: The quality of the resample, 1-10.
    name: A name for the operation (optional).

  Returns:
    output: Resampled audio.
  """
  return core_ops.io_audio_resample(
      input, rate_in=rate_in, rate_out=rate_out, quality=quality, name=name)

def decode_wav(input, shape=None, dtype=None, name=None): # pylint: disable=redefined-builtin
  """Decode WAV audio from input string.

  Args:
    input: A string `Tensor` of the audio input.
    shape: The shape of the audio.
    dtype: The data type of the audio, only tf.int16 and tf.int32 are supported.
    name: A name for the operation (optional).

  Returns:
    output: Decoded audio.
  """
  if shape is None:
    shape = tf.constant([-1, -1], tf.int64)
  assert (dtype is not None), "dtype (tf.int16/tf.int32) must be provided"
  return core_ops.io_audio_decode_wav(
      input, shape=shape, dtype=dtype, name=name)
