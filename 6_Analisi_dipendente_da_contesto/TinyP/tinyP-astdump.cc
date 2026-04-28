#include "ast.hh"
#include "utils.hh"
#include "parser.hh"
#include "ast_dumper.hh"

#include <iostream>

Node_Ptr ast_ptr;

int main(int argc, char** argv) {
  // get program file
  if (argc != 2)
    fatal_error("usage: tinyP-astdump filename");
  const char* filename = argv[1];
  // Parse input program
  parse_file(filename);
  // Set up ast dumper
  auto dumper = make_ast_dumper(std::cout);
  ast_ptr->accept(dumper.get());
}
