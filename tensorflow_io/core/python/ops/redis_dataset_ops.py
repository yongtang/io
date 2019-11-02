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
"""RedisDataset"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import uuid
import sys

import tensorflow as tf
from tensorflow_io.core.python.ops import core_ops
from tensorflow_io.core.python.ops import io_dataset_ops

class _RedisIODatasetFunction(object):
  def __init__(self, resource):
    self._resource = resource
  def __call__(self, start, stop):
    return core_ops.io_kafka_readable_read(
        self._resource, start=start, stop=stop,
        shape=tf.TensorShape([None]), dtype=tf.string)

class RedisIOStreamDataset(tf.data.Dataset): # pylint: disable=protected-access
  """RedisIOStreamDataset"""

  def __init__(self,
               channel,
               internal=True):
    """RedisIOStreamDataset."""
    with tf.name_scope("RedisIOStreamDataset") as scope:
      self._resource = core_ops.io_redis_pub_sub_init(
          channel,
          container=scope,
          shared_name="%s/%s" % (channel, uuid.uuid4().hex))

      dataset = tf.compat.v2.data.Dataset.range(0, sys.maxsize)
      dataset = dataset.map(
          lambda index: core_ops.io_redis_pub_sub_read(
              self._resource, index))
      dataset = dataset.apply(
          tf.data.experimental.take_while(
              lambda v: tf.greater(tf.shape(v)[0], 0)))
      dataset = dataset.unbatch()

      self._dataset = dataset
      super(RedisIOStreamDataset, self).__init__(
          self._dataset._variant_tensor) # pylint: disable=protected-access

  def _inputs(self):
    return []

  @property
  def element_spec(self):
    return self._dataset.element_spec
