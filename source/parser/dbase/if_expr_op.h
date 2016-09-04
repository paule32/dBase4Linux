#ifndef IF_EXPR_OP_H
#define IF_EXPR_OP_H

#ifdef QT_CORE_LIB
#include "qmymainwindow.h"
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <typeinfo>
#include <set>
#include <utility>
#include <vector>

#include "expression_ast.h"
#include "dbasevariables.h"

namespace dBaseParser {
struct if_expr_op {
    if_expr_op(
          std::string const &oper
        , expression_ast const& aexp
        , std::string const str2)
        : oper(oper)
        , name(str2)
    {
        // only expression
        if (oper == "ifexpr1") {
            static int count = 0;

            left_expr  = aexp;  // always true
            right_expr = aexp;  // because: if (expr) ...

            left_expr .expr = 1;
            right_expr.expr = 1;

            QString ori = QString("ifexpr:%1").arg(count);
            int o = getVariable(ori);
            dBaseVariables *v = new
            dBaseVariables;
            v->data_name = ori;
            v->data_type = p_value;
            v->data_type_extra = b_value;
            v->data_value_bool = false;
            dynamics[o] = v;

            count = count + 1;
            textcursor_in_if_block = true;
        }
    }

    // dummy
    if_expr_op();

    std::string oper;
    std::string name;

    bool status_info;

    expression_ast left_expr;
    expression_ast right_expr;
};
}
#endif // IF_EXPR_OP_H

