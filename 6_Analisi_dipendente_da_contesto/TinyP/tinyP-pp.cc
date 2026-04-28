#include "ast.hh"
#include "utils.hh"
#include "parser.hh"
#include "pretty_printer.hh"

#include <iostream>

Node_Ptr ast_ptr;

int main(int argc, char** argv) {
  // get program file
  if (argc != 2)
    fatal_error("usage: tinyP-pp filename");
  const char* filename = argv[1];
  // Parse input program
  parse_file(filename);
  // Set up pretty printer
  auto pp = make_pretty_printer(std::cerr, 2);
  ast_ptr->accept(pp.get());
}
