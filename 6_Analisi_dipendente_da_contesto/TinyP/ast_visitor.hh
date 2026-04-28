#pragma once

#include "utils.hh"

// Forward declarations
struct Node;
struct Bool_Literal;
struct Int_Literal;
struct Ident;
struct LValue_to_RValue;
struct Input;
struct AExpr;
struct BExpr;
struct Nop_Stmt;
struct Assign_Stmt;
struct Stmt_List;
struct Block_Stmt;
struct If_Stmt;
struct While_Stmt;
struct Do_Stmt;
struct Assert_Stmt;
struct Output_Stmt;

struct Node_Visitor {
  virtual void visit(const Bool_Literal*) = 0;
  virtual void visit(const Int_Literal*) = 0;
  virtual void visit(const Ident*) = 0;
  virtual void visit(const LValue_to_RValue*) = 0;
  virtual void visit(const Input*) = 0;
  virtual void visit(const AExpr*) = 0;
  virtual void visit(const BExpr*) = 0;
  virtual void visit(const Nop_Stmt*) = 0;
  virtual void visit(const Assign_Stmt*) = 0;
  virtual void visit(const Stmt_List*) = 0;
  virtual void visit(const Block_Stmt*) = 0;
  virtual void visit(const If_Stmt*) = 0;
  virtual void visit(const While_Stmt*) = 0;
  virtual void visit(const Do_Stmt*) = 0;
  virtual void visit(const Assert_Stmt*) = 0;
  virtual void visit(const Output_Stmt*) = 0;
  virtual ~Node_Visitor() = default;
};

struct Dummy_Visitor : public Node_Visitor {
  static void error() {
    fatal_error("Visiting unsupported node");
  }
  void visit(const Bool_Literal*) override { error(); }
  void visit(const Int_Literal*) override { error(); }
  void visit(const Ident*) override { error(); }
  void visit(const LValue_to_RValue*) override { error(); }
  void visit(const Input*) override {  error(); }
  void visit(const AExpr*) override { error(); }
  void visit(const BExpr*) override { error(); }
  void visit(const Nop_Stmt*) override { error(); }
  void visit(const Assign_Stmt*) override { error(); }
  void visit(const Stmt_List*) override { error(); }
  void visit(const Block_Stmt*) override { error(); }
  void visit(const If_Stmt*) override { error(); }
  void visit(const While_Stmt*) override { error(); }
  void visit(const Do_Stmt*) override { error(); }
  void visit(const Assert_Stmt*) override { error(); }
  void visit(const Output_Stmt*) override { error(); }
};
