/**
 * Copyright 2021 Huawei Technologies Co., Ltd
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
#include "composite/optimize/optimize.h"
#include <memory>
#include "composite/optimize/reshape_tensor.h"
#include "composite/optimize/elim_transform_op.h"
#include "composite/optimize/inplace_assign_mutator.h"
#include "composite/optimize/broadcast_inserter.h"
#include "composite/optimize/axis_attr_normalize.h"
#include "composite/optimize/fold_dimension.h"
#include "composite/optimize/typecast_inserter.h"
#include "composite/optimize/ops_combine.h"

namespace akg {
Stmt Optimize(Stmt &s, BuildInfo &info) {
  auto pm = CompositeOptPassMgr(info);
  // reshape optimize
  if (info.opt.target == "aicore") {
    pm.RegisterPass(std::make_shared<ReshapeTensor>());
  }
  // ops combine
  if (info.opt.target == "aicore") {
    pm.RegisterPass(std::make_shared<OpsCombine>(pm.info_));
  }
  // elemwise opt
  pm.RegisterPass(std::make_shared<ElimTransformOp>(pm.info_));
  // normalize axis attr
  pm.RegisterPass(std::make_shared<AxisAttrNormalize>());
  // fold dimension for multi-dim shape
  if (info.opt.fold_dim) {
    pm.RegisterPass(std::make_shared<FoldDimension>());
  }
  // inplace_assign
  pm.RegisterPass(std::make_shared<InplaceAssignOpt>(pm.info_));
  // insert broadcast
  pm.RegisterPass(std::make_shared<BroadcastInserter>());
  // insert cast for equal(int32) in ascend
  if (info.opt.target == "aicore") {
    pm.RegisterPass(std::make_shared<TypeCastInserter>());
  }
  s = pm.Run(s);
  return s;
}

}  // namespace akg
