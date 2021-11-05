# Copyright 2021 Google LLC
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

# disable module docstring for tests
# pylint: disable=C0114
# disable class docstring for tests
# pylint: disable=C0115

import os
from .bigtable_emulator import BigtableEmulator
from tensorflow_io.python.ops.bigtable.bigtable_dataset_ops import (
    BigtableClient,
)
import tensorflow as tf
from tensorflow import test


class BigtableReadTest(test.TestCase):
    def setUp(self):
        self.emulator = BigtableEmulator()

    def tearDown(self):
        self.emulator.stop()

    def test_read(self):
        os.environ["BIGTABLE_EMULATOR_HOST"] = self.emulator.get_addr()
        self.emulator.create_table(
            "fake_project", "fake_instance", "test-table", ["fam1", "fam2"]
        )

        values = [[f"[{i,j}]" for j in range(2)] for i in range(20)]

        ten = tf.constant(values)

        client = BigtableClient("fake_project", "fake_instance")
        table = client.get_table("test-table")

        self.emulator.write_tensor(
            "fake_project",
            "fake_instance",
            "test-table",
            ten,
            ["row" + str(i).rjust(3, "0") for i in range(20)],
            ["fam1:col1", "fam2:col2"],
        )

        for i, r in enumerate(table.read_rows(["fam1:col1", "fam2:col2"])):
            for j, c in enumerate(r):
                print("ij", i, j, c)
                self.assertEqual(values[i][j], c.numpy().decode())
