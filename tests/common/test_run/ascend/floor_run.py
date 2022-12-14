# Copyright 2019-2021 Huawei Technologies Co., Ltd
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

from akg.utils import kernel_exec as utils
import numpy as np
from tests.common.tensorio import compare_tensor
from akg.ops.math.ascend import floor
from tests.common.gen_random import random_gaussian


def floor_run(shape, dtype, kernel_name, attrs):
    if 'tuning' in attrs.keys():
        t = attrs.get("tuning", False)
        kernel_name = attrs.get("kernel_name", False)
        mod = utils.op_build_test(floor, [shape], [dtype], kernel_name=kernel_name, attrs=attrs, tuning=t)
        if t:
            expect, input_, output = gen_data(dtype, shape)
            return mod, expect, (input_, output)
        else:
            return mod
    else:
        mod = utils.op_build_test(floor, [shape], [dtype], kernel_name=kernel_name, attrs=attrs)
        expect, input_, output = gen_data(dtype, shape)
        output = utils.mod_launch(mod, (input_, output), expect=expect)
        return input_, output, expect, compare_tensor(output, expect, rtol=5e-03, equal_nan=True)


def gen_data(dtype, shape):
    input_ = random_gaussian(shape, miu=1.000001, sigma=0.09).astype(dtype)
    expect = np.floor(input_).astype(np.int32)
    output = np.full(shape, np.nan, "int32")
    return expect, input_, output
