# Copyright 2020 The TensorFlow Authors. All Rights Reserved.
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
"""Tests for HDF5 file"""

import os
import glob
import shutil
import tempfile
import numpy as np
import h5py

import tensorflow as tf
import tensorflow_io as tfio


def test_hdf5_io_dataset_resource_reuse():
    """test_hdf5_io_dataset_resource_reuse: GitHub issue 841"""

    runpath = tempfile.mkdtemp()
    os.makedirs(runpath, exist_ok=True)
    shapes = [(1, 2048), (1, 4096)]
    for i, shape in enumerate(shapes):
        filename = "{}/file_{}.h5".format(runpath, i)
        f = h5py.File(filename, "w")
        f.create_dataset("features", data=np.random.random(shape))
        f.close()
    filenames = ["{}/file_{}.h5".format(runpath, i) for i, shape in enumerate(shapes)]
    dataset = tf.data.Dataset.from_tensor_slices(filenames)
    #dataset = dataset.map(lambda e: tfio.IOTensor.from_hdf5(e, {"/features": tf.TensorSpec([None], tf.float64)})("/features").to_tensor())
    dataset = dataset.map(lambda e: tfio.IODataset.from_hdf5(e, "/features", tf.float64))
    dataset = dataset.flat_map(lambda e: e.map(tf.shape))
    value = [e for e in dataset]
    print("VALUE: ", value) #assert list(value) == list(shape[1:])

    shutil.rmtree(runpath)


if __name__ == "__main__":
    test.main()
