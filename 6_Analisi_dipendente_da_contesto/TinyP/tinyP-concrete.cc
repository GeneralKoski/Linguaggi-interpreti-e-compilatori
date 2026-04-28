#include "ast.hh"
#include "utils.hh"
#include "parser.hh"
#include "collecting.hh"

#include <iostream>

Node_Ptr ast_ptr;

int main(int argc, char** argv) {
  // get program file
  if (argc != 2)
    fatal_error("usage: tinyP-concrete filename");
  const char* filename = argv[1];
  // Parse input program
  parse_file(filename);
  // Set up interpreter for collecting semantics
  auto ptr = make_collecting();
  std::cout << "Starting program " << filename << std::endl;
  // Execute it
  ast_ptr->accept(ptr.get());
  std::cout << "Program run to completion" << std::endl;
}
