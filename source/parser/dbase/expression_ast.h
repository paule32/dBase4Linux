#ifndef EXPRESSION_AST_H
#define EXPRESSION_AST_H

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_ACTIONS_ALLOW_ATTR_COMPAT

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/qi_skip.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <typeinfo>
#include <set>
#include <utility>
#include <vector>

#include "dbasevariables.h"

#include "binary_op.h"
#include "unary_op.h"
#include "class_op.h"
#include "if_expr_op.h"

namespace dBaseParser {
struct expression_ast
{
    struct nil { };
    typedef
    boost::variant<
          nil
        , int
        , double
        , dBaseTypes
        , std::string
        , boost::recursive_wrapper<expression_ast>
        , boost::recursive_wrapper<binary_op>
        , boost::recursive_wrapper<unary_op>
        , boost::recursive_wrapper<class_op>
        , boost::recursive_wrapper<if_expr_op>
    >
    type;
    type expr;

    expression_ast() : expr(nil()) { }

    expression_ast(int dummy1, int dummy2, int dummy3) {
        throw dBaseMissException;
    }

    template <typename Expr>
    expression_ast(Expr const & expr)
        : expr(expr) { }

    expression_ast(
          std::string const& oper
        , std::string const& str1
        , std::string const& str2);

    expression_ast(
          std::string    const& oper
        , expression_ast const& aexp
        , std::string    const& str2);


    expression_ast& operator += (expression_ast const & rhs);
    expression_ast& operator -= (expression_ast const & rhs);
    expression_ast& operator *= (expression_ast const & rhs);
    expression_ast& operator /= (expression_ast const & rhs);
};
}

#endif // EXPRESSION_AST_H
