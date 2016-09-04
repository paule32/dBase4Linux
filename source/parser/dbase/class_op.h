#ifndef CLASS_OP_H
#define CLASS_OP_H

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <typeinfo>
#include <set>
#include <utility>
#include <vector>

#include "dbasevariables.h"
#include "expression_ast.h"

using namespace dBaseParser;
namespace dBaseParser {
struct class_op
{
    class_op(
          std::string const& oper
        , std::string const& cname
        , std::string const& oname
        , expression_ast const& co)
        : oper(oper)
        , class_cname(cname)
        , class_oname(oname)
        , class_owner(co)
    {
        QString str = class_oname.c_str();
        QString ori = class_cname.c_str();

        if (str.toLower().contains("form"))
        {
            if (getVariable(ori) < 1) {
                dBaseVariables *v    = new dBaseVariables;
                v->data_name         = ori;
                v->data_type         = w_value;
                v->data_value_widget = new QMyMainWindow;
                dynamics[getVariable(ori)] = v;
            }   else {
                int o = getVariable(ori);
                dynamics[o]->data_type = w_value;
                dynamics[o]->data_value_widget = new QMyMainWindow;
            }
        }
    }

    std::string oper;
    std::string class_cname;
    std::string class_oname;

    expression_ast class_owner;
};
}  // namespace dBaseParser

#endif // CLASS_OP_H

