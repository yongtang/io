# Copyright 2018 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
# ==============================================================================
"""Tests for NumpyFileDataset."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import numpy as np

import tensorflow as tf
tf.enable_eager_execution()
from tensorflow import keras
from tensorflow import data
from tensorflow import train
from tensorflow.python.platform import test

from tensorflow_io.numpy.python.ops import numpy_dataset_ops
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import errors
from tensorflow.python.platform import resource_loader


class NumpyFileDatasetTest(test.TestCase):

  def _test_numpy_file_dataset(self):
    """Test case for NumpyFileDataset."""
    f = {np.int32: "v32.npy", np.int64: "v64.npy"}
    for dtype in [np.int32, np.int64]:
      filename = os.path.join(resource_loader.get_data_files_path(),
                              "testdata", f[dtype])

      filenames = constant_op.constant([filename], dtypes.string)
      num_repeats = 2

      dataset = numpy_dataset_ops.NumpyFileDataset(filenames, dtypes.as_dtype(dtype)).repeat(
          num_repeats)
      iterator = dataset.make_initializable_iterator()
      init_op = iterator.initializer
      get_next = iterator.get_next()

      expected = np.array([[1, 2, 3, 4], [5, 6, 7, 8]], dtype=dtype)
      with self.cached_session() as sess:
        sess.run(init_op)
        for _ in range(num_repeats):
          self.assertAllEqual(expected, sess.run(get_next))
        with self.assertRaises(errors.OutOfRangeError):
          sess.run(get_next)

  def test_keras_with_dataset(self):
    filename = os.path.join(resource_loader.get_data_files_path(),
                            "testdata", "train.npz")

    dataset = numpy_dataset_ops.NumpyFileDataset([filename]).batch(32).repeat()
    model = keras.Sequential([
        keras.layers.Dense(64, activation='relu'),
        keras.layers.Dense(64, activation='relu'),
        keras.layers.Dense(10, activation='softmax')])

    model.compile(optimizer=train.AdamOptimizer(0.001),
        loss='categorical_crossentropy',
        metrics=['accuracy'])
    train_data = np.random.random((1000, 32)).astype(np.float32)
    train_labels = np.random.random((1000, 10)).astype(np.float32)
    #model.fit(data.Dataset.from_tensor_slices((train_data, train_labels)).batch(32).repeat(), epochs=10, steps_per_epoch=30)
    model.fit(dataset, epochs=10, steps_per_epoch=30)

if __name__ == "__main__":
  test.main()
