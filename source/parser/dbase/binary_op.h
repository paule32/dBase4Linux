#ifndef BINARY_OP_H
#define BINARY_OP_H

#include "expression_ast.h"

namespace dBaseParser {
struct binary_op
{
    binary_op(
          char op
        , expression_ast const & left
        , expression_ast const & right)
        : op(op)
        , left(left)
        , right(right) { }

    char op;
    expression_ast left;
    expression_ast right;
};
}

#endif // BINARY_OP_H

