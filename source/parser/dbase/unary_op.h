#ifndef UNARY_OP_H
#define UNARY_OP_H

#include "expression_ast.h"

namespace dBaseParser {
struct unary_op
{
    unary_op(
        char op
      , expression_ast const& subject)
    : op(op), subject(subject) {}

    char op;
    expression_ast subject;
};
} // namespace
#endif // UNARY_OP_H
