#pragma once

#include <string>

extern FILE* yyin;
int yylex();
void parse_file(const char* filename);

void yyerror(const char* msg);
void fatal_error(const char* msg);
void fatal_error(const std::string& msg);
