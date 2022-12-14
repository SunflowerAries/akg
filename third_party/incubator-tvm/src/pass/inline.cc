/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * 2022.3.28 - Add a parameter to support inline of CSR tensors.
 */

/*!
 * \file inline.cc
 */
#include <tvm/ir.h>
#include <tvm/ir_mutator.h>
#include <tvm/ir_pass.h>
#include "ir_util.h"

namespace air {
namespace ir {

// inliner to inline a function
// the result may not be SSA,
// ConvertSSA need to be applied after this pass
class IRInline final : public IRMutator {
 public:
  IRInline(FunctionRef f, Array<Var> args, Expr body, bool is_csr = false)
      : f_(f), args_(args), body_(body), is_csr_(is_csr) {}

  Expr Mutate_(const Call* op, const Expr& e) final {
    Expr expr = IRMutator::Mutate_(op, e);
    op = expr.as<Call>();

    if (op->func == f_) {
      CHECK_EQ(op->value_index, 0);
      expr = body_;
      Array<Expr> op_args = op->args;
      if (is_csr_) {
        op_args = VarsFromArgs(op_args);
      }
      CHECK_EQ(args_.size(), op_args.size());

      bool has_side_effect = false;
      for (size_t i = 0; i < op_args.size(); ++i) {
        if (HasSideEffect(op_args[i])) has_side_effect = true;
      }
      if (has_side_effect) {
        for (size_t i = 0; i < args_.size(); ++i) {
          expr = Let::make(args_[i], op_args[i], expr);
        }
      } else {
        Map<Var, Expr> vmap;
        for (size_t i = 0; i < args_.size(); ++i) {
          vmap.Set(args_[i], op_args[i]);
        }
        expr = Substitute(
            Evaluate::make(expr), vmap).as<Evaluate>()->value;
      }
      return expr;
    } else {
      return expr;
    }
  }

 private:
  FunctionRef f_;
  Array<Var> args_;
  Expr body_;
  bool is_csr_;
};

Stmt Inline(Stmt stmt,
            FunctionRef f,
            Array<Var> args,
            Expr body,
            bool is_csr) {
  CHECK_EQ(f->num_outputs(), 1)
      << "can only inline output single value operation";
  Stmt ret = IRInline(f, args, body, is_csr).Mutate(stmt);
  if (ret.same_as(stmt)) return ret;
  return ConvertSSA(ret);
}
}  // namespace ir
}  // namespace air
