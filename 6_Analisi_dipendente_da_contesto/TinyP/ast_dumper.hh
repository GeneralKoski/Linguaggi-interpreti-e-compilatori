#pragma once

#include "ast_visitor.hh"
#include <iostream>
#include <memory>

std::unique_ptr<Node_Visitor>
make_ast_dumper(std::ostream& os);
