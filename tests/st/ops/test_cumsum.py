# Copyright 2021 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import os
import pytest
import akg.utils as utils
from tests.common.base import TestBase
from tests.common.test_run import cumsum_run

############################################################
# TestCase= class: put to tests/*/
############################################################
class TestCase(TestBase):
    def __init__(self):
        self.case_name = "cumsum_run"
        self.case_path = os.getcwd()
        self.args_default = [
            ("000_case", cumsum_run, ((16, 3, 3, 16), "float32", 0, False, False), ["level0"]),
            ("001_case", cumsum_run, ((32, 3, 3, 16), "float32", 1, True, False), ["level0"]),
            ("002_case", cumsum_run, ((64, 3, 3, 16), "float32", 2, True, True), ["level0"]),
        ]

    def setup(self):
        self.params_init(self.case_name, self.case_path)
        return True

    def run_gpu_level0(self):
        return self.run_cases(self.args_default, utils.CUDA, "level0")
    
    def run_cpu_level0(self):
        return self.run_cases(self.args_default, utils.LLVM, "level0")

    def teardown(self):
        self._log.info("{0} Teardown".format(self.casename))
        super(TestCase, self).teardown()
        return

@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_gpu_level0():
    test_case = TestCase()
    test_case.setup()
    test_case.run_gpu_level0()
    test_case.teardown()

@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_cpu_level0():
    test_case = TestCase()
    test_case.setup()
    test_case.run_cpu_level0()
    test_case.teardown()