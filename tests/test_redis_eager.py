# Copyright 2019 The TensorFlow Authors. All Rights Reserved.
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
"""Tests for Redis."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import time
import pytest
import threading
import numpy as np

import redis

import tensorflow as tf
import tensorflow_io as tfio

def dataset_with_keras(
    dataset_fit, batch_size_fit,
    dataset_predict, batch_size_predict):
  model = tf.keras.models.Sequential([
    tf.keras.layers.Flatten(input_shape=(28, 28)),
    tf.keras.layers.Dense(128, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(10, activation='softmax')
  ])
  model.compile(optimizer='adam',
                loss='sparse_categorical_crossentropy',
                metrics=['accuracy'])
  dataset_y = dataset_fit.map(lambda e: tf.random.uniform((), 0, 10))
  dataset_x = dataset_fit.map(
      lambda e: tf.cast(tf.broadcast_to(e, [28, 28]), tf.float32))
  dataset = tf.data.Dataset.zip((dataset_x, dataset_y)).batch(batch_size_fit)
  model.fit(dataset, epochs=5, verbose=2)

  dataset = dataset_predict.map(
      lambda e: tf.cast(tf.broadcast_to(e, [28, 28]), tf.float32))
  dataset = dataset.batch(batch_size_predict)
  return model.predict(dataset)

def dataset_with_iterator(dataset, repeat=True):
  entries = [d for d in dataset]
  if repeat:
    repeated = [d for d in dataset]
    assert entries == repeated
    repeated = [d for d in dataset.repeat(2)]
    assert entries + entries == repeated
  else:
    pass
  return entries 

def test_dataset_with_iterator_for_redis():
  dataset = tf.data.Dataset.range(0, 10)

  def run_dataset_with_iterator(returned):
    returned.append(dataset_with_iterator(dataset, repeat=True))
  returned = []
  thread = threading.Thread(target=run_dataset_with_iterator, args=(returned,))
  thread.start()

  r = redis.Redis(host='localhost', port=6379, db=0)
  r.publish('foo', 'hello world')

  thread.join()

  assert returned[0] == list(range(10))

def test_dataset_with_keras_for_redis():
  train = tf.data.Dataset.range(0, 10)

  #dataset = tf.data.Dataset.range(0, 10)
  dataset = tfio.IOStreamDataset.from_redis("foo")
  dataset = dataset.map(tf.strings.to_number)

  def run_dataset_with_keras(returned):
    returned.append(dataset_with_keras(train, 1, dataset, 1))
  returned = []
  thread = threading.Thread(target=run_dataset_with_keras, args=(returned,))
  thread.start()

  r = redis.Redis()
  r.publish('foo', 'hello world')

  thread.join()

  assert returned[0].shape == (10, 10)
