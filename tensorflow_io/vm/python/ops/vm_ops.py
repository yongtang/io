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
"""VM Dataset."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import ctypes
import numpy 
import numpy as np

import tensorflow as tf
from tensorflow_io.core.python.ops import core_ops
from tensorflow_io.core.python.ops import data_ops

def vm_numpy(array, **kwargs):
  """vm_numpy"""
  if not tf.executing_eagerly():
    raise NotImplementedError("vm_numpy only support eager mode")
  start = kwargs.get("start", 0)
  stop = kwargs.get("stop", array.shape[0])
  address, read_only = array.__array_interface__['data']
  c_float_p = ctypes.POINTER(ctypes.c_float)
  data = numpy.array([[0.1, 0.1], [0.2, 0.2], [0.3, 0.3]])
  data = data.astype(numpy.float32)
  data_p = data.ctypes.data_as(c_float_p)
  array_p = array.ctypes.data_as(ctypes.POINTER(ctypes.c_long))
  v = ctypes.addressof(data_p)
  v = ctypes.addressof(array_p)
  print("VM_NUMPY: ", start, stop, address, read_only)
  print("DATA_P: ", data_p.contents)
  print("DATA_X: ", hex(v), v, hex(address))
  return core_ops.vm_numpy(
      os.getpid(), address=address, start=start, stop=stop, dtype=array.dtype)

class VMNumpyDataset(data_ops.BaseDataset):
  """A Numpy Dataset that reads the numpy array."""

  def __init__(self, array, **kwargs):
    """Create a `NumpyDataset`.

    Args:
      array: numpy array to read.
    """
    # Note: count and dtype could be in kwargs if in graph mode.
    if not tf.executing_eagerly():
      count = kwargs.get("count")
      dtype = kwargs.get("dtype")
    else:
      columns = list_parquet_columns(filename)
      count = columns[column].shape[0]
      dtype = columns[column].dtype

    shape = tf.TensorShape([None])

    # capacity is the rough count for each chunk in dataset
    capacity = kwargs.get("capacity", 65536)
    entry_start = list(range(0, count, capacity))
    entry_count = [min(capacity, count - start) for start in entry_start]
    dataset = data_ops.BaseDataset.from_tensor_slices(
        (tf.constant(entry_start, tf.int64), tf.constant(entry_count, tf.int64))
    ).map(lambda start, count: parquet_ops.read_parquet(
        filename, column, start, count, dtype=dtype, memory=""))
    self._dataset = dataset

    super(ParquetDataset, self).__init__(
        self._dataset._variant_tensor, [dtype], [shape]) # pylint: disable=protected-access
