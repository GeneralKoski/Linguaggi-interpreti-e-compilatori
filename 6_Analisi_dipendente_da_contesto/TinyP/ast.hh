#pragma once

#include "ast_visitor.hh"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <gmpxx.h>

struct Node;
using Node_Ptr = std::unique_ptr<Node>;
using Node_Ptrs = std::vector<Node_Ptr>;

// Dirty trick to type less
#define ACCEPT_VISITOR \
  void accept(Node_Visitor* v) const override { v->visit(this); }


struct Node {
  Node() = default;
  virtual ~Node() = default;

  // copie/spostamenti non permessi
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;
  Node(Node&&) = delete;
  Node& operator=(Node&&) = delete;

  // copia profonda
  virtual Node* clone() const = 0;
  // for visitor pattern
  virtual void accept(Node_Visitor* v) const = 0;
};

struct Bool_Literal : public Node {
  bool value_;
  explicit Bool_Literal(bool value) : value_(value) {}
  Bool_Literal* clone() const override { return new Bool_Literal(value_); }
  ACCEPT_VISITOR;
};

struct Int_Literal : public Node {
  mpz_class value_;
  explicit Int_Literal(const std::string& value) : value_(value) {}
  explicit Int_Literal(const mpz_class& z) : value_(z) {}
  Int_Literal* clone() const override { return new Int_Literal(value_); }
  ACCEPT_VISITOR;
};

struct Ident : public Node {
  std::string name_;
  explicit Ident(const std::string& name) : name_(name) {}
  Ident* clone() const override { return new Ident(name_); }
  ACCEPT_VISITOR;
};

struct LValue_to_RValue : public Node {
  Node_Ptr arg_;
  LValue_to_RValue(Node* arg) : arg_(arg) {}
  LValue_to_RValue* clone() const override {
    return new LValue_to_RValue(arg_->clone());
  }
  ACCEPT_VISITOR;
};

struct Input : public Node {
  Node_Ptr arg1_;
  Node_Ptr arg2_;
  Input(Node* arg1, Node* arg2) : arg1_(arg1), arg2_(arg2) {}
  Input* clone() const override {
    return new Input(arg1_->clone(), arg2_->clone());
  }
  ACCEPT_VISITOR;
};

struct AExpr : public Node {
  enum Op { UMINUS, UPLUS, ADD, SUB, MUL, DIV, MOD, PAREN };
  Op op_;
  Node_Ptr arg1_;
  Node_Ptr arg2_;

  AExpr(Op op, Node* arg1, Node* arg2)
    : op_(op), arg1_(arg1), arg2_(arg2) {}
  bool isParen() const { return (op_ == PAREN); }
  bool isUnary() const { return (op_ == UMINUS) || (op_ == UPLUS); }
  bool isBinary() const { return not isUnary() && not isParen(); }

  AExpr* clone() const override {
    Node* c1 = arg1_->clone();
    Node* c2 = (not isBinary()) ? nullptr : arg2_->clone();
    return new AExpr(op_, c1, c2);
  }

  ACCEPT_VISITOR;
};

struct BExpr : public Node {
  enum Op { LT, LE, EQ, NE, GE, GT, NOT, AND, OR, PAREN };
  Op op_;
  Node_Ptr arg1_;
  Node_Ptr arg2_;

  BExpr(Op op, Node* arg1, Node* arg2)
    : op_(op), arg1_(arg1), arg2_(arg2) {}
  bool isParen() const { return (op_ == PAREN); }
  bool isUnary() const { return (op_ == NOT); }
  bool isBinary() const { return (op_ == AND) || (op_ == OR); }
  bool isAtomic() const {
    return not isUnary() && not isBinary() && not isParen();
  }
  BExpr* clone() const override {
    Node* c1 = arg1_->clone();
    Node* c2 = (isUnary() || isParen()) ? nullptr : arg2_->clone();
    return new BExpr(op_, c1, c2);
  }
  ACCEPT_VISITOR;
};

struct Nop_Stmt : public Node {
  Nop_Stmt* clone() const override { return new Nop_Stmt; }
  ACCEPT_VISITOR;
};

struct Assign_Stmt : public Node {
  Node_Ptr lhs_;
  Node_Ptr rhs_;

  Assign_Stmt(Node* lhs, Node* rhs) : lhs_(lhs), rhs_(rhs) {}
  Assign_Stmt* clone() const override {
    return new Assign_Stmt(lhs_->clone(), rhs_->clone());
  }
  ACCEPT_VISITOR;
};

struct Stmt_List : public Node {
  Node_Ptrs stmts_;

  Stmt_List() = default;
  void add_stmt(Node* stmt) {
    stmts_.emplace_back(stmt);
  }

  Stmt_List* clone() const override {
    auto res = new Stmt_List();
    for (const auto& p : stmts_)
      res->add_stmt(p->clone());
    return res;
  }

  ACCEPT_VISITOR;
};

struct Block_Stmt : public Node {
  Node_Ptr stmt_list_;

  explicit Block_Stmt(Node* stmt_list) : stmt_list_(stmt_list) {}
  Block_Stmt* clone() const override {
    return new Block_Stmt(stmt_list_->clone());
  }
  ACCEPT_VISITOR;
};

struct If_Stmt : public Node {
  Node_Ptr cond_;
  Node_Ptr then_stmt_;
  Node_Ptr else_stmt_;

  If_Stmt(Node* cond, Node* then_stmt, Node* else_stmt)
    : cond_(cond), then_stmt_(then_stmt), else_stmt_(else_stmt) {}

  If_Stmt* clone() const override {
    return new If_Stmt(cond_->clone(),
                       then_stmt_->clone(),
                       else_stmt_->clone());
  }

  ACCEPT_VISITOR;
};

struct While_Stmt : public Node {
  Node_Ptr cond_;
  Node_Ptr stmt_;
  While_Stmt(Node* cond, Node* stmt) : cond_(cond), stmt_(stmt) {}

  While_Stmt* clone() const override {
    return new While_Stmt(cond_->clone(), stmt_->clone());
  }

  ACCEPT_VISITOR;
};

struct Do_Stmt : public Node {
  Node_Ptr cond_;
  Node_Ptr stmt_;
  Do_Stmt(Node* stmt, Node* cond) : cond_(cond), stmt_(stmt) {}
  Do_Stmt* clone() const override {
    return new Do_Stmt(cond_->clone(), stmt_->clone());
  }
  ACCEPT_VISITOR;
};

struct Assert_Stmt : public Node {
  Node_Ptr cond_;
  Assert_Stmt(Node* cond) : cond_(cond) {}

  Assert_Stmt* clone() const override {
    return new Assert_Stmt(cond_->clone());
  }
  ACCEPT_VISITOR;
};

struct Output_Stmt : public Node {
  Node_Ptr arg_;
  Output_Stmt(Node* arg) : arg_(arg) {}

  Output_Stmt* clone() const override {
    return new Output_Stmt(arg_->clone());
  }
  ACCEPT_VISITOR;
};

