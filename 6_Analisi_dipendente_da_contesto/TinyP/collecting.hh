#pragma once

#include "ast_visitor.hh"
#include <memory>

std::unique_ptr<Node_Visitor> make_collecting();
