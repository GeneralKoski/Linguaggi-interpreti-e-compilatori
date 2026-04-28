%{

#include "ast.hh"
#include "utils.hh"
#include <cassert>
#include <cstddef>
#include <string>

extern Node_Ptr ast_ptr;

#include "parser.hh"

%}

%union {
  std::string* str_ptr;
  Node* node_ptr;
};

  /* tokens (with precedence and associativity) */

%token OP_PLUS
%token OP_MINUS
%token OP_MUL
%token OP_DIV
%token OP_MOD
%token OP_ASSIGN
%token OP_LT
%token OP_LE
%token OP_EQ
%token OP_NE
%token OP_GE
%token OP_GT
%token OP_AND
%token OP_OR
%token OP_NOT
%token L_PAREN
%token R_PAREN
%token L_BRACKET
%token R_BRACKET
%token L_BRACE
%token R_BRACE
%token COMMA
%token SEMICOLON

%token KWD_FALSE
%token KWD_TRUE
%token KWD_IF
%token KWD_ELSE
%token KWD_WHILE
%token KWD_DO
%token KWD_ASSERT
%token KWD_INPUT
%token KWD_OUTPUT

%token <str_ptr> IDENTIFIER
%token <str_ptr> INT_LITERAL
%token UNARY

%left OP_OR
%left OP_AND
%nonassoc OP_NOT

%left OP_PLUS OP_MINUS
%left OP_MUL OP_DIV OP_MOD
%nonassoc UNARY

  /* non-terminals */
%start prog
%type <node_ptr> prog
%type <node_ptr> stmts
%type <node_ptr> stmt
%type <node_ptr> bexpr
%type <node_ptr> batom
%type <node_ptr> aexpr
%type <node_ptr> atom
%type <node_ptr> ident
%type <node_ptr> int_literal

%%

prog:
    stmts      { $$ = $1; ast_ptr.reset($$); }
  ;

stmts:
               { $$ = new Stmt_List; }
  | stmts stmt {
      auto stmt_list = dynamic_cast<Stmt_List*>($1);
      assert(stmt_list);
      stmt_list->add_stmt($2);
      $$ = stmt_list;
    }
  ;

stmt:
    SEMICOLON { $$ = new Nop_Stmt; }
  | ident OP_ASSIGN aexpr SEMICOLON {
      $$ = new Assign_Stmt($1, $3);
    }
  | KWD_IF L_PAREN bexpr R_PAREN stmt {
      $$ = new If_Stmt($3, $5, new Nop_Stmt);
    }
  | KWD_IF L_PAREN bexpr R_PAREN stmt KWD_ELSE stmt {
      $$ = new If_Stmt($3, $5, $7);
    }
  | KWD_WHILE L_PAREN bexpr R_PAREN stmt {
      $$ = new While_Stmt($3, $5);
    }
  | KWD_DO stmt KWD_WHILE L_PAREN bexpr R_PAREN SEMICOLON {
      $$ = new Do_Stmt($2, $5);
    }
  | L_BRACE stmts R_BRACE { $$ = new Block_Stmt($2); }
  | KWD_ASSERT L_PAREN bexpr R_PAREN SEMICOLON { $$ = new Assert_Stmt($3); }
  | KWD_OUTPUT aexpr SEMICOLON { $$ = new Output_Stmt($2); }
  ;

bexpr:
    bexpr OP_OR bexpr     { $$ = new BExpr(BExpr::OR, $1, $3); }
  | bexpr OP_AND bexpr    { $$ = new BExpr(BExpr::AND, $1, $3); }
  | OP_NOT bexpr          { $$ = new BExpr(BExpr::NOT, $2, nullptr); }
  | L_PAREN bexpr R_PAREN { $$ = new BExpr(BExpr::PAREN, $2, nullptr); }
  | batom
  ;

batom:
    aexpr OP_LT aexpr  { $$ = new BExpr(BExpr::LT, $1, $3); }
  | aexpr OP_LE aexpr  { $$ = new BExpr(BExpr::LE, $1, $3); }
  | aexpr OP_EQ aexpr  { $$ = new BExpr(BExpr::EQ, $1, $3); }
  | aexpr OP_NE aexpr  { $$ = new BExpr(BExpr::NE, $1, $3); }
  | aexpr OP_GE aexpr  { $$ = new BExpr(BExpr::GE, $1, $3); }
  | aexpr OP_GT aexpr  { $$ = new BExpr(BExpr::GT, $1, $3); }
  | KWD_FALSE   { $$ = new Bool_Literal(false); }
  | KWD_TRUE    { $$ = new Bool_Literal(true); }
  ;

aexpr:
    aexpr OP_PLUS aexpr  { $$ = new AExpr(AExpr::ADD, $1, $3); }
  | aexpr OP_MINUS aexpr { $$ = new AExpr(AExpr::SUB, $1, $3); }
  | aexpr OP_MUL aexpr   { $$ = new AExpr(AExpr::MUL, $1, $3); }
  | aexpr OP_DIV aexpr   { $$ = new AExpr(AExpr::DIV, $1, $3); }
  | aexpr OP_MOD aexpr   { $$ = new AExpr(AExpr::MOD, $1, $3); }
  | OP_PLUS aexpr %prec UNARY  { $$ = new AExpr(AExpr::UPLUS, $2, nullptr); }
  | OP_MINUS aexpr %prec UNARY { $$ = new AExpr(AExpr::UMINUS, $2, nullptr); }
  | L_PAREN aexpr R_PAREN { $$ = new AExpr(AExpr::PAREN, $2, nullptr); }
  | atom
  ;

atom:
    int_literal
  | ident { $$ = new LValue_to_RValue($1); }
  | KWD_INPUT L_BRACKET int_literal COMMA int_literal R_BRACKET {
      $$ = new Input($3, $5);
    }
  ;

ident:
    IDENTIFIER {
      auto str_ptr = $1;
      $$ = new Ident(*str_ptr);
      delete str_ptr;
    }

int_literal:
    INT_LITERAL {
      auto str_ptr = $1;
      $$ = new Int_Literal(*str_ptr);
      delete str_ptr;
    }

%%

void
parse_file(const char* filename) {
  yyin = fopen(filename, "r");
  if (yyin == nullptr)
    fatal_error(std::string("exception: ")
                + "cannot open `" + filename + "'");
  if (yyparse())
    fatal_error("exception: parse error at end of file");
  fclose(yyin);
}
