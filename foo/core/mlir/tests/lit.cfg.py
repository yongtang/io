# Copyright 2021 Yong Tang. All Rights Reserved.
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
import lit.formats

config.substitutions.append(
    (
        "foo-opt",
        os.path.abspath(os.path.join(os.getcwd(), "foo", "core", "mlir", "foo-opt")),
    )
)
config.substitutions.append(
    (
        "FileCheck",
        os.path.abspath(
            os.path.join(os.getcwd(), "external", "llvm-project", "llvm", "FileCheck")
        ),
    )
)
config.name = "LIT hello world"
config.test_format = lit.formats.ShTest("0")
