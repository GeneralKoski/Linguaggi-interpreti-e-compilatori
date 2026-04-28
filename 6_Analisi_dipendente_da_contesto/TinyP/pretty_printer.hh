#pragma once

#include "ast_visitor.hh"
#include <iostream>
#include <memory>

std::unique_ptr<Node_Visitor>
make_pretty_printer(std::ostream& os, unsigned indent_step = 2);
