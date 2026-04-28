#include "utils.hh"
#include <iostream>

void fatal_error(const char* msg) {
  std::cerr << msg << std::endl;
  exit(1);
}

void fatal_error(const std::string& msg) {
  fatal_error(msg.c_str());
}

void yyerror(const char* msg) {
  fatal_error(msg);
}

