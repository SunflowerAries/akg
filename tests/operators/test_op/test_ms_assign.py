# Copyright 2020 Huawei Technologies Co., Ltd
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
# limitations under the License
import numpy as np
import akg
from akg.ops.math_gpu.assign import assign
from tests.common.gen_random import random_gaussian
from akg.utils import kernel_exec as utils
from akg.utils.result_analysis import target_profiling
from akg.utils.format_transform import to_tvm_nd_array

def gen_data(dtype, ref_shape, val_shape):
    if dtype == "float32":
        ref = random_gaussian(ref_shape, miu=1, sigma=0.1).astype(np.float32)
        val = random_gaussian(val_shape, miu=1, sigma=0.1).astype(np.float32)
    expect = val
    return ref, val, expect

def test_ms_assign(dtype, ref_shape, val_shape, poly_sch=True, attrs=None):
    if not attrs:
        attrs = {"target": "cuda"}
    ref, val, expect = gen_data(dtype, ref_shape, val_shape)
    mod = utils.op_build_test(assign, (ref_shape, val_shape), (dtype, dtype), kernel_name="assign",
                                polyhedral=poly_sch, attrs=attrs)
    output = np.full(val_shape, np.nan, dtype)
    result = utils.mod_launch(mod, (ref, val, output), expect = expect)
    res = np.allclose(result, expect, rtol=5e-03, atol=1.e-8)
    print("Test {}".format("Pass" if res else "Fail"))
    target_name = attrs["target"].split()[0]
    if not res:
        mod_source = mod
        if target_name != "llvm":
            mod_source = mod.imported_modules[0]
        print("Error {}:========================".format(target_name))
        print(mod_source.get_source())
        raise AssertionError("Test fail")
    if attrs["profiling"]:
        ref, val, output = to_tvm_nd_array([ref, val, output], akg.tvm.context(target_name, 0))
        target_profiling(mod, ref, val, output, target=target_name, repeat_time=attrs["repeat_time"])