%{

#include "ast.hh"
#include "utils.hh"

#include <iostream>
#include <string>

void eof_error();

#include "parser.hh"

%}

%option noyywrap
%option nodefault
%option nounput
%option yylineno

%x comment_mode

HOR_WSPACE      [ \t]
VER_WSPACE      [\f\n\v]
WSPACE          {HOR_WSPACE}|{VER_WSPACE}

DIGIT           [0-9]
USCORE          [_]
LETTER          [a-zA-Z_]
ULETTER         {USCORE}|{LETTER}
INTEGER         {DIGIT}+
IDENT           {ULETTER}({ULETTER}|{DIGIT})*

%%

  /* -------------------- operators -------------------- */

"+"           { return OP_PLUS; }
"-"           { return OP_MINUS; }
"*"           { return OP_MUL; }
"/"           { return OP_DIV; }
"%"           { return OP_MOD; }
"="           { return OP_ASSIGN; }
"<"           { return OP_LT; }
"<="          { return OP_LE; }
"=="          { return OP_EQ; }
"!="          { return OP_NE; }
">="          { return OP_GE; }
">"           { return OP_GT; }
"or"          { return OP_OR; }
"and"         { return OP_AND; }
"not"         { return OP_NOT; }

  /* -------------------- punctuation -------------------- */

"("           { return L_PAREN; }
")"           { return R_PAREN; }
"["           { return L_BRACKET; }
"]"           { return R_BRACKET; }
"{"           { return L_BRACE; }
"}"           { return R_BRACE; }
","           { return COMMA; }
";"           { return SEMICOLON; }

  /* -------------------- keywords -------------------- */

"false"       { return KWD_FALSE; }
"true"        { return KWD_TRUE; }
"if"          { return KWD_IF; }
"else"        { return KWD_ELSE; }
"while"       { return KWD_WHILE; }
"do"          { return KWD_DO; }
"assert"      { return KWD_ASSERT; }
"input"       { return KWD_INPUT; }
"output"      { return KWD_OUTPUT; }

  /* -------------------- integer literals -------------------- */

{INTEGER}     {
                yylval.str_ptr = new std::string(yytext);
                return INT_LITERAL;
              }

  /* -------------------- identifiers -------------------- */

{IDENT}      {
                yylval.str_ptr = new std::string(yytext);
                return IDENTIFIER;
              }

  /* -------------------- single line comment -------------------- */

"//".*             /* discard single line comment */

  /* -------------------- multi-line comment -------------------- */

"/*"               { BEGIN comment_mode; }  /* enter comment mode */
<comment_mode>"*/" { BEGIN INITIAL; }       /* exit comment mode */
<comment_mode>.    /* discard any char but \n */
<comment_mode>"\n" /* discard new line */
<comment_mode><<EOF>> { eof_error(); }

  /* -------------------- whitespace -------------------- */

{WSPACE}+          /* discard */

  /* -------------------- catch-all -------------------- */

.             {
                auto msg = std::string("Unknown character: ") + *yytext;
                yyerror(msg.c_str());
              }

%%

void eof_error() {
  std::cerr << "ERROR: reached EOF while processing C style comment"
            << std::endl;
  yy_fatal_error("Exiting");
}
