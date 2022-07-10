/**
 * Copyright 2021-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef POLY_TILING_HERMES_HARDWARE_H_
#define POLY_TILING_HERMES_HARDWARE_H_

#include <string>

namespace akg {
namespace ir {
namespace poly {
class Hardware {
 public:
  Hardware(int, int, int, int, int, int, int, int);

  static bool HasUBFail(const std::string);
  static void AddUBFailCounter() { Hardware::mem_VC_alloc_failed_++; }
  static void ResetUBFailCounter() { Hardware::mem_VC_alloc_failed_ = 0; }

  int num_core_;
  int mem_VC_size_;
  int mem_C1_size_;
  int mem_C0_size_;
  int mem_VC_align_;
  int mem_C1_align_;
  int vblocknum_;
  int vblocksize_;

 private:
  static int mem_VC_alloc_failed_;
};

const int kNumCore = 32;
const int kMemVCSize = 262144;
const int kMemC1Size = 1048576;
const int kMemC0Size = 65536;
const int kMemVCAlign = 32;
const int kMemC1Align = 512;
const int kVBlockNum = 8;
const int kVBlockSize = 32;
}  // namespace poly
}  // namespace ir
}  // namespace akg
#endif  // POLY_TILING_HERMES_HARDWARE_H_
