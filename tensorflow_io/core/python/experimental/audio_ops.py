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
"""Audio Ops."""

import numpy as np

import tensorflow as tf


def spectrogram(input, window, stride, nfft, name=None):
    """
    Create spectrogram from audio.

    Args:
      input: An audio signal Tensor.
      window: Size of window.
      stride: Stride between windows.
      nfft: Size of FFT.
      name: A name for the operation (optional).

    Returns:
      A tensor of spectrogram.
    """

    def periodic_hann_window(window, dtype):
        return 0.5 - 0.5 * tf.math.cos(
            2.0
            * np.pi
            * tf.range(tf.cast(window, tf.float32), dtype=dtype)
            / tf.cast(window, tf.float32)
        )

    return tf.abs(
        tf.signal.stft(
            input,
            frame_length=window,
            frame_step=stride,
            fft_length=nfft,
            window_fn=periodic_hann_window,
            pad_end=True,
        )
    )

def mel_scale(input, nfilter, fmin, fmax, name=None):
    """
    Create MelSpectrogram from audio.

    Args:
      input: An audio signal Tensor.
      window: Size of window.
      stride: Stride between windows.
      nfft: Size of FFT.
      name: A name for the operation (optional).

    Returns:
      A tensor of spectrogram.
    """
    return spectrogram(input, window, stride, nfft, name)



def time_mask(input, param, name=None):
    """
    Apply masking to a spectrogram in the time domain.

    Args:
      input: An audio spectogram.
      param: Parameter of time masking.
      name: A name for the operation (optional).

    Returns:
      A tensor of spectrogram.
    """
    time_max = tf.shape(input)[1]
    t = tf.random.uniform(shape=(), minval=0, maxval=param, dtype=tf.dtypes.int32)
    t0 = tf.random.uniform(
        shape=(), minval=0, maxval=time_max - t, dtype=tf.dtypes.int32
    )
    indices = tf.reshape(tf.range(time_max), (1, -1, 1))
    condition = tf.math.logical_and(
        tf.math.greater_equal(indices, t0), tf.math.less(indices, t0 + t)
    )
    return tf.where(condition, 0, input)


def freq_mask(input, param, name=None):
    """
    Apply masking to a spectrogram in the freq domain.

    Args:
      input: An audio spectogram.
      param: Parameter of freq masking.
      name: A name for the operation (optional).

    Returns:
      A tensor of spectrogram.
    """
    freq_max = tf.shape(input)[2]
    f = tf.random.uniform(shape=(), minval=0, maxval=param, dtype=tf.dtypes.int32)
    f0 = tf.random.uniform(
        shape=(), minval=0, maxval=freq_max - f, dtype=tf.dtypes.int32
    )
    indices = tf.reshape(tf.range(freq_max), (1, 1, -1))
    condition = tf.math.logical_and(
        tf.math.greater_equal(indices, f0), tf.math.less(indices, f0 + f)
    )
    return tf.where(condition, 0, input)
