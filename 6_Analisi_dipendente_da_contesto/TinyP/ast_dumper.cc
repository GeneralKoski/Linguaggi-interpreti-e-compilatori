#include "ast_dumper.hh"

#include "ast.hh"
#include "ast_visitor.hh"
#include "utils.hh"

#include <cassert>
#include <iostream>
#include <string>

struct AST_Dumper : public Node_Visitor {
  std::ostream& os_;
  unsigned indent_step_;
  unsigned current_indent_ = 0;

  explicit AST_Dumper(std::ostream& os, int indent_step = 2)
    : os_(os), indent_step_(indent_step) {}

  void inc_indent() { current_indent_ += indent_step_; }
  void dec_indent() {
    assert(current_indent_ >= indent_step_);
    current_indent_ -= indent_step_;
  }

  void indent() {
    if (current_indent_ > 0)
      os_ << std::string(current_indent_, ' ');
  }

  void newline() { os_ << '\n'; }
  void print(const char* s) { os_ << s; }

  void println(const char* s) {
    print(s);
    newline();
  }

  void print_op(AExpr::Op op) {
    switch (op) {
    case AExpr::UPLUS:
      print("unary +");
      break;
    case AExpr::UMINUS:
      print("unary -");
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
      print("()");
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
      print("()");
      break;
    }
  }

  void visit_also(const Node_Ptr& p) {
    inc_indent();
    p.get()->accept(this);
    dec_indent();
  }

  void visit(const Bool_Literal* p) override {
    indent();
    print("Bool_Literal");
    os_ << " : " << (p->value_ ? "true" : "false");
    newline();
  }

  void visit(const Int_Literal* p) override {
    indent();
    print("Int_Literal");
    os_ << " : " << p->value_;
    newline();
  }

  void visit(const Ident* p) override {
    indent();
    print("Ident");
    os_ << " : " << p->name_;
    newline();
  }

  void visit(const LValue_to_RValue* p) override {
    indent();
    println("LValue_to_RValue");
    visit_also(p->arg_);
  }

  void visit(const Input* p) override {
    indent();
    println("Input");
    visit_also(p->arg1_);
    visit_also(p->arg2_);
  }

  void visit(const BExpr* p) override {
    indent();
    print("BExpr");
    print(" : ");
    print_op(p->op_);
    newline();
    visit_also(p->arg1_);
    if (p->isBinary() || p->isAtomic())
      visit_also(p->arg2_);
  }

  void visit(const AExpr* p) override {
    indent();
    print("AExpr");
    print(" : ");
    print_op(p->op_);
    newline();
    visit_also(p->arg1_);
    if (p->isBinary())
      visit_also(p->arg2_);
  }

  void visit(const Nop_Stmt*) override {
    indent();
    println("Nop_Stmt");
  }

  void visit(const Assign_Stmt* p) override {
    indent();
    println("Assign_Stmt");
    visit_also(p->lhs_);
    visit_also(p->rhs_);
  }

  void visit(const Stmt_List* p) override {
    indent();
    println("Stmt_List");
    for (const auto& s : p->stmts_)
      visit_also(s);
  }

  void visit(const Block_Stmt* p) override {
    indent();
    println("Block_Stmt");
    visit_also(p->stmt_list_);
  }

  void visit(const If_Stmt* p) override {
    indent();
    println("If_Stmt");
    visit_also(p->cond_);
    visit_also(p->then_stmt_);
    visit_also(p->else_stmt_);
  }

  void visit(const While_Stmt* p) override {
    indent();
    println("While_Stmt");
    visit_also(p->cond_);
    visit_also(p->stmt_);
  }

  void visit(const Do_Stmt* p) override {
    indent();
    println("Do_Stmt");
    visit_also(p->stmt_);
    visit_also(p->cond_);
  }

  void visit(const Assert_Stmt* p) override {
    indent();
    println("Assert_Stmt");
    visit_also(p->cond_);
  }

  void visit(const Output_Stmt* p) override {
    indent();
    println("Output_Stmt");
    visit_also(p->arg_);
  }

}; // struct AST_Dumper

std::unique_ptr<Node_Visitor>
make_ast_dumper(std::ostream& os) {
  auto dumper = std::make_unique<AST_Dumper>(os, 2);
  return dumper;
}
