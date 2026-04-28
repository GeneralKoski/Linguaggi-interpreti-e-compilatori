#include "collecting.hh"

#include "ast.hh"
#include "ast_visitor.hh"
#include "pretty_printer.hh"
#include "utils.hh"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>

using Env = std::map<std::string, mpz_class>;
using Envs = std::set<Env>;
using ZValues = std::set<mpz_class>;

void dump_env(const Env& env) {
  auto& os = std::cout;
  os << "=== dumping environment ===" << std::endl;
  for (const auto& p : env) {
    os << "  " << p.first << " = " << p.second << std::endl;
  }
  os << "===========================" << std::endl;
}

void dump_envs(const Envs& envs) {
  auto& os = std::cout;
  os << "=== dumping possible environments ===\n\n";
  auto i = 0;
  for (const auto& env : envs) {
    os << "=== dumping env " << i << "\n";
    ++i;
    dump_env(env);
    os << "\n";
  }
  os << "==== end of possible environments ===\n\n";
}

Envs
join_envs(const Envs& envs1, const Envs& envs2) {
  Envs res;
  std::set_union(envs1.begin(), envs1.end(),
                 envs2.begin(), envs2.end(),
                 std::inserter(res, res.end()));
  return res;
}

Envs
meet_envs(const Envs& envs1, const Envs& envs2) {
  Envs res;
  std::set_intersection(envs1.begin(), envs1.end(),
                        envs2.begin(), envs2.end(),
                        std::inserter(res, res.end()));
  return res;
}

struct AExpr_Collecting;
struct VExpr_Collecting;
struct Stmt_Collecting;

ZValues visit_aexpr(const Node_Ptr& p, const Envs& envs);
Envs visit_bexpr(const Node_Ptr& p, const Envs& envs);
Envs visit_stmt(const Node_Ptr& p, const Envs& envs);

const Ident* get_ident(const Node_Ptr& p) {
  return dynamic_cast<const Ident*>(p.get());
}

//////////////////////////////////////////////////////////////////////

struct AExpr_Collecting : public Dummy_Visitor {
  const Envs& envs;
  ZValues zvalues;

  void visit_also(const Node_Ptr& p) { p.get()->accept(this); }

  AExpr_Collecting(const Envs& e) : envs(e) {}

  void visit(const Int_Literal* p) override {
    zvalues = { p->value_ };
  }

  void visit(const LValue_to_RValue* p) override {
    auto lvalue = get_ident(p->arg_);
    const auto& env_key = lvalue->name_;
    zvalues.clear();
    for (auto& env : envs) {
      auto it = env.find(env_key);
      if (it != env.end())
        zvalues.insert(it->second);
    }
  }

  void visit(const Input* p) override {
    visit_also(p->arg1_);
    assert(zvalues.size() == 1);
    auto lower_bound = *(zvalues.begin());
    visit_also(p->arg2_);
    assert(zvalues.size() == 1);
    auto upper_bound = *(zvalues.begin());
    zvalues.clear();
    for (auto z = lower_bound; z <= upper_bound; ++z) {
      zvalues.insert(zvalues.end(), z);
    }
  }

  void visit(const AExpr* p) override {
    zvalues.clear();
    for (const auto& env : envs) {
      if (p->isBinary()) {
        auto zvalues1 = visit_aexpr(p->arg1_, { env });
        auto zvalues2 = visit_aexpr(p->arg2_, { env });
        for (const auto& z1 : zvalues1) {
          for (const auto& z2 : zvalues2) {
            switch (p->op_) {
            case AExpr::ADD:
              zvalues.insert(z1 + z2);
              break;
            case AExpr::SUB:
              zvalues.insert(z1 - z2);
              break;
            case AExpr::MUL:
              zvalues.insert(z1 * z2);
              break;
            case AExpr::DIV:
              if (z2 != 0)
                zvalues.insert(z1 / z2);
              break;
            case AExpr::MOD:
              if (z2 != 0)
                zvalues.insert(z1 % z2);
              break;
            default:
              fatal_error("unreachable code in AExpr evaluation");
              break;
            }
          }
        }
      } else if (p->isUnary()) {
        zvalues = visit_aexpr(p->arg1_, { env });
        switch (p->op_) {
        case AExpr::UPLUS:
          // nothing to do
          break;
        case AExpr::UMINUS:
          {
            ZValues res;
            for (const auto& z : zvalues) {
              res.insert(-z);
            }
            std::swap(res, zvalues);
          }
          break;
        default:
          fatal_error("unreachable code in AExpr evaluation");
          break;
        }
      } else {
        assert(p->isParen());
        visit_also(p->arg1_);
      }
    }
  }
};  // AExpr_Collecting

ZValues
visit_aexpr(const Node_Ptr& aexpr, const Envs& envs) {
  auto visitor = std::make_unique<AExpr_Collecting>(envs);
  aexpr->accept(visitor.get());
  auto res = std::move(visitor->zvalues);
  return res;
}

/////////////////////////////////////////////////////////////////////////////

struct BExpr_Negator : public Dummy_Visitor {
  Node* negated;

  static BExpr::Op negate_operator(BExpr::Op op) {
    switch (op) {
    case BExpr::LT:
      return BExpr::GE;
    case BExpr::LE:
      return BExpr::GT;
    case BExpr::EQ:
      return BExpr::NE;
    case BExpr::NE:
      return BExpr::EQ;
    case BExpr::GE:
      return BExpr::LT;
    case BExpr::GT:
      return BExpr::LE;
    case BExpr::AND:
      return BExpr::OR;
    case BExpr::OR:
      return BExpr::AND;
    default:
      fatal_error("unreachable code in BExpr evaluation");
      return BExpr::LE;
    }
  }

  void visit_also(const Node_Ptr& p) { p.get()->accept(this); }

  void visit(const Bool_Literal* p) override {
    negated = new Bool_Literal(not p->value_);
  }

  void visit(const BExpr* p) override {
    if (p->isAtomic()) {
      auto neg_op = negate_operator(p->op_);
      auto arg1 = p->arg1_->clone();
      auto arg2 = p->arg2_->clone();
      negated = new BExpr(neg_op, arg1, arg2);
      return;
    }

    if (p->isUnary()) {
      switch (p->op_) {
      case BExpr::NOT:
        negated = p->arg1_->clone();
        break;
      default:
        fatal_error("unreachable code in BExpr evaluation");
        break;
      }
      return;
    }

    if (p->isBinary()) {
      auto neg_op = negate_operator(p->op_);
      visit_also(p->arg1_);
      auto neg1 = negated;
      visit_also(p->arg2_);
      auto neg2 = negated;
      negated = new BExpr(neg_op, neg1, neg2);
      return;
    }

    assert(p->isParen());
    visit_also(p->arg1_);
  }

}; // BExpr_Negator

Node_Ptr
negate_bexpr(const Node_Ptr& bexpr) {
  auto visitor = std::make_unique<BExpr_Negator>();
  bexpr->accept(visitor.get());
  return Node_Ptr(visitor->negated);
}

/////////////////////////////////////////////////////////////////////////////

struct BExpr_Collecting : public Dummy_Visitor {
  Envs envs;

  BExpr_Collecting(const Envs& es) : envs(es) {}

  void visit_also(const Node_Ptr& p) { p.get()->accept(this); }

  void visit(const Bool_Literal* p) override {
    if (p->value_) {
      /* true: no filter */
    } else {
      /* false: empty set */
      envs.clear();
    }
  }

  template <typename BinaryPred>
  static bool
  check_any_of(const ZValues& zvs1, const ZValues& zvs2,
               BinaryPred pred) {
    auto it = std::find_first_of(zvs1.begin(), zvs1.end(),
                                 zvs2.begin(), zvs2.end(),
                                 pred);
    return it != zvs1.end();
  }

  static bool check_LT(const ZValues& zvs1, const ZValues& zvs2) {
    return check_any_of(zvs1, zvs2, std::less<mpz_class>{});
  }
  static bool check_LE(const ZValues& zvs1, const ZValues& zvs2) {
    return check_any_of(zvs1, zvs2, std::less_equal<mpz_class>{});
  }
  static bool check_EQ(const ZValues& zvs1, const ZValues& zvs2) {
    return check_any_of(zvs1, zvs2, std::equal_to<mpz_class>{});
  }
  static bool check_NE(const ZValues& zvs1, const ZValues& zvs2) {
    return check_any_of(zvs1, zvs2, std::not_equal_to<mpz_class>{});
  }
  static bool check_GE(const ZValues& zvs1, const ZValues& zvs2) {
    return check_any_of(zvs1, zvs2, std::greater_equal<mpz_class>{});
  }
  static bool check_GT(const ZValues& zvs1, const ZValues& zvs2) {
    return check_any_of(zvs1, zvs2, std::greater<mpz_class>{});
  }

  void visit(const BExpr* p) override {
    if (p->isAtomic()) {
      Envs res;
      for (const auto& env : envs) {
        auto zvalues1 = visit_aexpr(p->arg1_, {env});
        auto zvalues2 = visit_aexpr(p->arg2_, {env});
        switch (p->op_) {
        case BExpr::LT:
          if (check_LT(zvalues1, zvalues2))
            res.insert(env);
          break;
        case BExpr::LE:
          if (check_LE(zvalues1, zvalues2))
            res.insert(env);
          break;
        case BExpr::EQ:
          if (check_EQ(zvalues1, zvalues2))
            res.insert(env);
          break;
        case BExpr::NE:
          if (check_NE(zvalues1, zvalues2))
            res.insert(env);
          break;
        case BExpr::GE:
          if (check_GE(zvalues1, zvalues2))
            res.insert(env);
          break;
        case BExpr::GT:
          if (check_GT(zvalues1, zvalues2))
            res.insert(env);
          break;
        default:
          fatal_error("unreachable code in BExpr evaluation");
          break;
        }
      }
      std::swap(envs, res);
      return;
    }

    if (p->isUnary()) {
      switch (p->op_) {
      case BExpr::NOT:
        {
          auto negated = negate_bexpr(p->arg1_);
          visit_also(negated);
        }
        break;
      default:
        fatal_error("unreachable code in BExpr evaluation");
        break;
      }
      return;
    }

    if (p->isBinary()) {
      auto envs1 = visit_bexpr(p->arg1_, envs);
      auto envs2 = visit_bexpr(p->arg2_, envs);
      switch (p->op_) {
      case BExpr::AND:
        envs = meet_envs(envs1, envs2);
        break;
      case BExpr::OR:
        envs = join_envs(envs1, envs2);
        break;
      default:
        fatal_error("unreachable code in BExpr evaluation");
        break;
      }
      return;
    }

    assert(p->isParen());
    visit_also(p->arg1_);
  }

}; // BExpr_Collecting


Envs
visit_bexpr(const Node_Ptr& bexpr, const Envs& envs) {
  auto visitor = std::make_unique<BExpr_Collecting>(envs);
  bexpr->accept(visitor.get());
  auto res = std::move(visitor->envs);
  return res;
}

//////////////////////////////////////////////////////////////////////

struct Stmt_Collecting : public Dummy_Visitor {
  Envs envs;

  void visit_also(const Node_Ptr& p) { p.get()->accept(this); }

  explicit Stmt_Collecting(const Envs& es) : envs(es) {}

  void visit(const Nop_Stmt*) override { /* nop */ }

  void visit(const Assign_Stmt* p) override {
    Envs res;
    auto lvalue = get_ident(p->lhs_);
    for (const auto& env : envs) {
      ZValues zvalues = visit_aexpr(p->rhs_, { env });
      for (const auto& z : zvalues) {
        auto mod_env = env;
        mod_env[lvalue->name_] = z;
        res.insert(mod_env);
      }
    }
    std::swap(envs, res);
  }

  void visit(const Stmt_List* p) override {
    for (const auto& s : p->stmts_) {
      visit_also(s);
    }
  }

  void visit(const Block_Stmt* p) override {
    visit_also(p->stmt_list_);
  }

  void visit(const If_Stmt* p) override {
    // then branch
    const auto& then_cond = p->cond_;
    auto then_envs = visit_bexpr(then_cond, envs);
    then_envs = visit_stmt(p->then_stmt_, then_envs);
    // else branch
    auto else_cond = negate_bexpr(then_cond);
    auto else_envs = visit_bexpr(else_cond, envs);
    else_envs = visit_stmt(p->else_stmt_, else_envs);
    envs = join_envs(then_envs, else_envs);
  }

  void compute_fixpoint(Envs& X, const Node_Ptr& cond, const Node_Ptr& stmt) {
    // equation: F(X) = R join S[[stmt]] (C[[cond]] X)
    auto R = X; // initial state
    Envs prev_X; // previous state (to later check fixpoint)
    while (true) {
      // save previous state
      prev_X = X;
      // filter states entering loop
      X = visit_bexpr(cond, X);
      // execute loop body
      X = visit_stmt(stmt, X);
      // join with initial state
      X = join_envs(R, X);
      // check for fixpoint (optimized)
      if (X.size() == prev_X.size()) {
        assert(X == prev_X);
        return;
      }
    }
  }

  void visit(const While_Stmt* p) override {
    const auto& enter_cond = p->cond_;
    const auto& loop_body = p->stmt_;
    compute_fixpoint(envs, enter_cond, loop_body);
    // exit states = C[[¬c]](lfp F )
    auto exit_cond = negate_bexpr(enter_cond);
    envs = visit_bexpr(exit_cond, envs);
  }

  void visit(const Do_Stmt* p) override {
    // execute first iteration without checking cond
    const auto& loop_body = p->stmt_;
    envs = visit_stmt(loop_body, envs);
    // from here, same as while loop
    const auto& enter_cond = p->cond_;
    compute_fixpoint(envs, enter_cond, loop_body);
    // exit states = C[[¬c]](lfp F )
    auto exit_cond = negate_bexpr(enter_cond);
    envs = visit_bexpr(exit_cond, envs);
  }

  void visit(const Assert_Stmt* p) override {
#if 0
    // interpretato come una assume (vedi Miné)
    envs = visit_bexpr(p->cond_, envs);
#else
    // interpretato come un check
    std::cout << "\nassertion: ";
    auto pp = make_pretty_printer(std::cout, 2);
    p->cond_->accept(pp.get());
    std::cout << std::endl;
    auto unsafe_cond = negate_bexpr(p->cond_);
    auto unsafe_envs = visit_bexpr(unsafe_cond, envs);
    if (unsafe_envs.size() == 0) {
      std::cout << "  -> assertion successfully checked\n";
    } else {
      std::cout << "  -> assertion failed!\n";
      std::cout << "dumping envs violating assertion:\n";
      dump_envs(unsafe_envs);
    }
#endif
  }

  void visit(const Output_Stmt* p) override {
    auto zvalues = visit_aexpr(p->arg_, envs);
    std::cout << "\n=== begin output statement ===\n";
    for (const auto& z : zvalues) {
      std::cout << "(Possible) Output value: " << z << "\n";
    }
    std::cout << "=== end output statement ===\n";
  }

}; // Stmt_Collecting

Envs
visit_stmt(const Node_Ptr& stmt, const Envs& envs) {
  auto visitor = std::make_unique<Stmt_Collecting>(envs);
  stmt->accept(visitor.get());
  auto res = std::move(visitor->envs);
  return res;
}

////////////////////////////////////////////////////////////

std::unique_ptr<Node_Visitor> make_collecting() {
  // start with singleton set containing an empty environment
  Envs envs { Env{} };
  return std::make_unique<Stmt_Collecting>(envs);
}

/////////////////////////////////////////////////////////////////////
