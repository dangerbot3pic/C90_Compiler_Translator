#ifndef ast_hpp
#define ast_hpp

#include "classes/ast_head.hpp"
#include "classes/function.hpp"
#include "classes/primitives.hpp"
#include "classes/parameter.hpp"
#include "classes/statement.hpp"
#include "classes/scope.hpp"
#include "classes/binary_expression.hpp"
#include "classes/list.hpp"
#include "classes/return_items.hpp"
#include "classes/context.hpp"
#include "classes/statement_types.hpp"
#include "classes/program.hpp"


extern const Node *parseAST();

#endif
