/**
 * Copyright 2022 Huawei Technologies Co., Ltd
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
#ifndef POLY_TILING_HERMES_TILING_IR_SURVEY_H_
#define POLY_TILING_HERMES_TILING_IR_SURVEY_H_

#include <tvm.h>
#include <tvm/ir_visitor.h>

#include <map>
#include <string>
#include <vector>

namespace akg {
namespace ir {
namespace poly {
class TilingIRSurvey : public IRVisitor {
 public:
  TilingIRSurvey() = default;

  void Visit_(const AttrStmt *op) override;
  void Visit_(const For *op) override;
  void Visit_(const Call *op) override;
  void Visit_(const Provide *op) override;
  void Visit_(const Realize *op) override;

  [[nodiscard]] bool IsSymbolicEnabled() const { return is_symbolic_enabled_; }

 private:
  bool is_symbolic_enabled_{true};
  std::map<std::string, int> for_range_;
  std::vector<std::string> provide_args_;
};

bool HasSymbolicStatusChanged(const Stmt &stmt_sch);
}  // namespace poly
}  // namespace ir
}  // namespace akg
#endif  // POLY_TILING_HERMES_TILING_IR_SURVEY_H_
