#include "interpreter.hh"

#include "ast.hh"
#include "ast_visitor.hh"
#include "utils.hh"
#include <cassert>
#include <iostream>
#include <map>
#include <string>

struct Interpreter : public Node_Visitor {
  using Env = std::map<std::string, mpz_class>;
  Env env;
  const Ident* lvalue;
  mpz_class zvalue;
  bool bvalue = false;
  bool be_noisy_ = false;

  void dump_env() const {
    auto& os = std::cerr;
    os << "=== dumping environment ===" << std::endl;
    for (const auto& p : env) {
      os << "  " << p.first << " = " << p.second << std::endl;
    }
    os << "===========================" << std::endl;
  }

  Interpreter() = default;
  explicit Interpreter(bool be_noisy) : be_noisy_(be_noisy) {}
  ~Interpreter() { if (be_noisy_) dump_env(); }

  void visit_also(const Node_Ptr& p) { p.get()->accept(this); }

  void visit(const Bool_Literal* p) override { bvalue = p->value_; }

  void visit(const Int_Literal* p) override { zvalue = p->value_; }

  void visit(const Ident* p) override { lvalue = p; }

  void visit(const LValue_to_RValue* p) override {
    visit_also(p->arg_);
    const auto& env_key = lvalue->name_;
    auto it = env.find(env_key);
    if (it != env.end())
      zvalue = it->second;
    else
      fatal_error(std::string("exception: ")
                  + "reading uninitialized variable " + env_key);
  }

  void visit(const Input* p) override {
    visit_also(p->arg1_);
    auto lower_bound = zvalue;
    visit_also(p->arg2_);
    auto upper_bound = zvalue;
    if (lower_bound > upper_bound)
      fatal_error("exception: empty input range");
    std::cout << "Waiting for input in range "
              << "[" << lower_bound << ", " << upper_bound << "]"
              << std::endl;
    std::cin >> zvalue;
    if (not std::cin)
      fatal_error("exception: invalid input value");
    if (not (lower_bound <= zvalue && zvalue <= upper_bound))
      fatal_error("exception: input value out of range");
    std::cout << "Read input: " << zvalue << std::endl;
  }

  void visit(const AExpr* p) override {
    if (p->isBinary()) {
      visit_also(p->arg2_);
      auto zvalue2 = zvalue;
      visit_also(p->arg1_);
      switch (p->op_) {
      case AExpr::ADD:
        zvalue += zvalue2;
        break;
      case AExpr::SUB:
        zvalue -= zvalue2;
        break;
      case AExpr::MUL:
        zvalue *= zvalue2;
        break;
      case AExpr::DIV:
        if (zvalue2 == 0)
          fatal_error("exception: division by zero");
        zvalue /= zvalue2;
        break;
      case AExpr::MOD:
        if (zvalue2 == 0)
          fatal_error("exception: division by zero");
        zvalue %= zvalue2;
        break;
      default:
        fatal_error("unreachable code in AExpr evaluation");
        break;
      }
    } else if (p->isUnary()) {
      visit_also(p->arg1_);
      switch (p->op_) {
      case AExpr::UPLUS:
        break;
      case AExpr::UMINUS:
        zvalue = -zvalue;
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

  void visit(const BExpr* p) override {
    if (p->isAtomic()) {
      visit_also(p->arg1_);
      auto val1 = zvalue;
      visit_also(p->arg2_);
      switch (p->op_) {
      case BExpr::LT:
        bvalue = (val1 < zvalue);
        break;
      case BExpr::LE:
        bvalue = (val1 <= zvalue);
        break;
      case BExpr::EQ:
        bvalue = (val1 == zvalue);
        break;
      case BExpr::NE:
        bvalue = (val1 != zvalue);
        break;
      case BExpr::GE:
        bvalue = (val1 >= zvalue);
        break;
      case BExpr::GT:
        bvalue = (val1 > zvalue);
        break;
      default:
        fatal_error("unreachable code in BExpr evaluation");
        break;
      }
    } else if (p->isUnary()) {
      visit_also(p->arg1_);
      switch (p->op_) {
      case BExpr::NOT:
        bvalue = not bvalue;
        break;
      default:
        fatal_error("unreachable code in BExpr evaluation");
        break;
      }
    } else if (p->isBinary()) {
      visit_also(p->arg2_);
      auto bvalue2 = bvalue;
      visit_also(p->arg1_);
      switch (p->op_) {
      case BExpr::AND:
        bvalue &= bvalue2;
        break;
      case BExpr::OR:
        bvalue |= bvalue2;
        break;
      default:
        fatal_error("unreachable code in BExpr evaluation");
        break;
      }
    } else {
      assert(p->isParen());
      visit_also(p->arg1_);
    }
  }

  void visit(const Nop_Stmt*) override {
    // nop
  }

  void visit(const Assign_Stmt* p) override {
    visit_also(p->lhs_);
    if (lvalue == nullptr)
      fatal_error("unreachable code in assignment: invalid lvalue");
    visit_also(p->rhs_);
    env[lvalue->name_] = zvalue;
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
    visit_also(p->cond_);
    if (bvalue)
      visit_also(p->then_stmt_);
    else
      visit_also(p->else_stmt_);
  }

  void visit(const While_Stmt* p) override {
    while (true) {
      visit_also(p->cond_);
      if (not bvalue) break;
      visit_also(p->stmt_);
    }
  }

  void visit(const Do_Stmt* p) override {
    while (true) {
      visit_also(p->stmt_);
      visit_also(p->cond_);
      if (not bvalue) break;
    }
  }

  void visit(const Assert_Stmt* p) override {
    visit_also(p->cond_);
    if (not bvalue) {
      fatal_error("exception: assertion is false");
    }
  }

  void visit(const Output_Stmt* p) override {
    visit_also(p->arg_);
    std::cout << "Output value: " << zvalue << std::endl;
  }

}; // struct Interpreter

std::unique_ptr<Node_Visitor>
make_interpreter(bool be_noisy) {
  auto ptr = std::make_unique<Interpreter>(be_noisy);
  return ptr;
}
