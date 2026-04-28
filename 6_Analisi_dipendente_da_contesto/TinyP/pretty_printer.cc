#include "pretty_printer.hh"

#include "ast.hh"
#include "ast_visitor.hh"
#include "utils.hh"

#include <cassert>
#include <iostream>
#include <string>

struct Pretty_Printer : public Node_Visitor {
  std::ostream& os_;
  unsigned indent_step_;
  unsigned current_indent_ = 0;
  bool should_indent_ = true;

  explicit Pretty_Printer(std::ostream& os, int indent_step = 2)
    : os_(os), indent_step_(indent_step) {}

  void inc_indent() { current_indent_ += indent_step_; }
  void dec_indent() {
    assert(current_indent_ >= indent_step_);
    current_indent_ -= indent_step_;
  }

  void maybe_indent() {
    if (should_indent_) {
      should_indent_ = false;
      if (current_indent_ > 0)
        os_ << std::string(current_indent_, ' ');
    }
  }

  void print(const char* s) {
    maybe_indent();
    os_ << s;
  }

  void semicolon() { print(";"); }
  void space() { print(" "); }
  void newline() {
    // no indentation before
    os_ << '\n';
    should_indent_ = true;
  }

  void print_op(AExpr::Op op) {
    switch (op) {
    case AExpr::UPLUS:
      print("+");
      break;
    case AExpr::UMINUS:
      print("-");
      break;
    case AExpr::ADD:
      print("+");
      break;
    case AExpr::SUB:
      print("-");
      break;
    case AExpr::MUL:
      print("*");
      break;
    case AExpr::DIV:
      print("/");
      break;
    case AExpr::MOD:
      print("%");
      break;
    case AExpr::PAREN:
      fatal_error("unreachable code when printing AExpr");
      break;
    }
  }

  void print_op(BExpr::Op op) {
    switch (op) {
    case BExpr::NOT:
      print("not");
      break;
    case BExpr::AND:
      print("and");
      break;
    case BExpr::OR:
      print("or");
      break;
    case BExpr::LT:
      print("<");
      break;
    case BExpr::LE:
      print("<=");
      break;
    case BExpr::EQ:
      print("==");
      break;
    case BExpr::NE:
      print("!=");
      break;
    case BExpr::GE:
      print(">=");
      break;
    case BExpr::GT:
      print(">");
      break;
    case BExpr::PAREN:
      fatal_error("unreachable code when printing BExpr");
      break;
    }
  }

  void visit_also(const Node_Ptr& p) { p.get()->accept(this); }

  void visit(const Bool_Literal* p) override {
    if (p->value_)
      print("true");
    else
      print("false");
  }

  void visit(const Int_Literal* p) override {
    maybe_indent();
    os_ << p->value_;
  }

  void visit(const Ident* p) override {
    maybe_indent();
    os_ << p->name_;
  }

  void visit(const LValue_to_RValue* p) override {
    visit_also(p->arg_);
  }

  void visit(const Input* p) override {
    print("input");
    space();
    print("[");
    visit_also(p->arg1_);
    print(",");
    space();
    visit_also(p->arg2_);
    print("]");
  }

  void visit(const BExpr* p) override {
    if (p->isUnary()) {
      print_op(p->op_);
      space();
      visit_also(p->arg1_);
    } else if (p->isBinary() || p->isAtomic()) {
      visit_also(p->arg1_);
      space();
      print_op(p->op_);
      space();
      visit_also(p->arg2_);
    } else {
      assert(p->isParen());
      print("(");
      visit_also(p->arg1_);
      print(")");
    }
  }

  void visit(const AExpr* p) override {
    if (p->isUnary()) {
      print_op(p->op_);
      space();
      visit_also(p->arg1_);
    } else if (p->isBinary()) {
      visit_also(p->arg1_);
      space();
      print_op(p->op_);
      space();
      visit_also(p->arg2_);
    } else {
      assert(p->isParen());
      print("(");
      visit_also(p->arg1_);
      print(")");
    }
  }

  void visit(const Nop_Stmt*) override {
    semicolon();
    newline();
  }

  void visit(const Assign_Stmt* p) override {
    visit_also(p->lhs_);
    print(" = ");
    visit_also(p->rhs_);
    semicolon();
    newline();
  }

  void visit(const Stmt_List* p) override {
    for (const auto& s : p->stmts_) {
      visit_also(s);
    }
  }

  void visit(const Block_Stmt* p) override {
    print("{");
    newline();
    inc_indent();
    visit_also(p->stmt_list_);
    dec_indent();
    print("}");
    newline();
  }

  void visit(const If_Stmt* p) override {
    print("if (");
    visit_also(p->cond_);
    print(")");
    newline();
    inc_indent();
    visit_also(p->then_stmt_);
    dec_indent();
    print("else");
    newline();
    inc_indent();
    visit_also(p->else_stmt_);
    dec_indent();
  }

  void visit(const While_Stmt* p) override {
    print("while (");
    visit_also(p->cond_);
    print(")");
    newline();
    inc_indent();
    visit_also(p->stmt_);
    dec_indent();
  }

  void visit(const Do_Stmt* p) override {
    print("do");
    newline();
    inc_indent();
    visit_also(p->stmt_);
    dec_indent();
    print("while (");
    visit_also(p->cond_);
    print(")");
    semicolon();
    newline();
  }

  void visit(const Assert_Stmt* p) override {
    print("assert(");
    visit_also(p->cond_);
    print(")");
    semicolon();
    newline();
  }

  void visit(const Output_Stmt* p) override {
    print("output");
    space();
    visit_also(p->arg_);
    semicolon();
    newline();
  }

}; // struct Pretty_Printer

std::unique_ptr<Node_Visitor>
make_pretty_printer(std::ostream& os, unsigned indent_step) {
  auto pp = std::make_unique<Pretty_Printer>(os, indent_step);
  return pp;
}
