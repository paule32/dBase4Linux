#ifdef QT_CORE_LIB
#include "qmymainwindow.h"
#endif

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
#include "dbase_exception.h"

#include "binary_op.h"
#include "unary_op.h"
#include "if_expr_op.h"

#include "expression_ast.h"

int lineno = 1;

using namespace std;
using namespace boost::spirit;

namespace bs    = boost::spirit;
namespace phx   = boost::phoenix;
namespace ascii = boost::spirit::ascii;

using boost::spirit::ascii::space; // use the ASCII space parser
using boost::spirit::ascii::char_;
using boost::spirit::_val;

using boost::spirit::qi::skip;
using boost::spirit::qi::eoi;

using boost::phoenix::val;

namespace dBaseParser
{
    struct nil { };

    int getVariable(QString name)
    {
        bool found = false;
        int  idx = 0;
        if (dynamics.size() > -1)
        for (idx = 0;idx < dynamics.size(); idx++) {
            if (dynamics[idx]->data_name == name) {
                found = true;
                break;
            }
        }
        if (!found) {
            dBaseVariables *t = new
            dBaseVariables;

            t->data_name  = name;
            dynamics.append( t );
        }   return idx;
    }


    expression_ast expr;

    expression_ast::expression_ast(
          std::string    const& oper
        , expression_ast const& aexp
        , std::string    const& str2)
    {
        QString str = oper.c_str();
        expr_ast.expr = if_expr_op(oper, aexp, "if");
    }

    expression_ast::expression_ast  (
              std::string const& oper
            , std::string const& str1
            , std::string const& str2)
    {
        QString str = oper.c_str();
        QString val = str1.c_str();

        static QString sstr1;

        if (str == QString("addparameter1")) {
            dBaseVariables *v = new
            dBaseVariables;
            v->data_name = val;
            v->data_type = p_value;
            v->data_type_extra = b_value;
            v->data_value_bool = 0;
            dynamics[getVariable(val)] = v;
        }

        if (str == QString("create1class")) { sstr1 = val; }
        if (str == QString("create2class")) {
            expr_ast.expr = class_op(oper, sstr1.toStdString(), str1, *this);
        }

        if (str == QString("@app@parameter")) {
        }

        if (str == QString("@this@parent")) {
            vec_push_3 << val;
        }
        else if (str == QString("@this@object")) {
            vec_push_2 << val;
        }
        else if (str == QString("@this")) {
            vec_push_1 << val;
        }
        else {
            expr_ast.expr = class_op(oper, str1, str2, *this);
        }
    }

    expression_ast& expression_ast::operator += (expression_ast const& rhs)
    {
        expr = binary_op('+', expr_ast, rhs);
        return *this;
    }

    expression_ast& expression_ast::operator -= (expression_ast const& rhs)
    {
        expr = binary_op('-', expr_ast, rhs);
        return *this;
    }

    expression_ast& expression_ast::operator *= (expression_ast const& rhs)
    {
        expr = binary_op('*', expr_ast, rhs);
        return *this;
    }

    expression_ast& expression_ast::operator /= (expression_ast const& rhs)
    {
        expr = binary_op('/', expr_ast, rhs);
        return *this;
    }

    // We should be using expression_ast::operator-. There's a bug
    // in phoenix type deduction mechanism that prevents us from
    // doing so. Phoenix will be switching to BOOST_TYPEOF. In the
    // meantime, we will use a phoenix::function below:
    struct negate_expr
    {
        template <typename T>
        struct result { typedef T type; };

        expression_ast operator()(expression_ast & expr) const
        {
            expr = expression_ast(unary_op('-', expr));
        }
    };

    phx::function<negate_expr> neg;

    // -----------------------
    // walk throug the AST ...
    // -----------------------
    struct ast_print
    {
        typedef void result_type;

        void operator()(nil) const {
            cout << "done." << endl;
            expr = nil();
        }
        void operator()(int n) const {
            std::cout << n;
            expr  = n;

            if (int c = getVariable("onnn") < 1) {
                dBaseVariables *v = new dBaseVariables;
                v->data_name      = "onnn" ;
                v->data_type      = i_value;
                v->data_value_int = int(n) ;
                dynamics[c]  = v; }   else {
                dynamics[c]->data_type      = i_value;
                dynamics[c]->data_value_int = int(n);
            }
        }


        void operator()(expression_ast const& ast) const
        {
            /*
            cout << "---> "
                 << ast.expr.type().name()
                 << endl;*/

            //dast = ast;
            //if (!(ast.expr.type().name() == std::string("N11dBaseParser3nilE")))
            //boost::apply_visitor(*this, ast.expr);
        }

        void operator()(binary_op const& expr) const
        {
            int lval, rval, value;

            std::cout << "op" << expr.op << "(" <<
            expr.left.expr;
            boost::apply_visitor(*this, expr.left.expr);
            try {
                lval = boost::get<int>(expr);
            }   catch (...) {
                lval = dynamics[0]->data_value_int;
            }


            std::cout << ", ";
            expr = expr.right;
            boost::apply_visitor(*this, expr.right.expr);
            try {
                rval = boost::get<int>(expr);
            }   catch (...) {
                rval = dynamics[0]->data_value_int;
            }

            std::cout << ')';

            switch (expr.op) {
                case '+': value = lval + rval; break;
                case '-': value = lval - rval; break;
                case '*': value = lval * rval; break;
                case '/': value = lval / rval; break;
            }

            if (int c = getVariable("onnn") < 1) {
                dBaseVariables *v = new dBaseVariables;
                v->data_name      = "onnn" ;
                v->data_type      = i_value;
                v->data_value_int = value;
                dynamics[c] = v; }    else {
                dynamics[c]->data_type      = i_value;
                dynamics[c]->data_value_int = value;
            }
            expr = value;
        }

        void operator()(unary_op const& expr) const
        {
            std::cout << "oP" << expr.op << "(";
            boost::apply_visitor(*this, expr.subject.expr);
            std::cout << ')';
        }

        void operator()(class_op const& expr) const
        {
            QMessageBox::information(w,"text parser","SUCCE");
            //boost::apply_visitor(*this, expr.class_owner);
        }

        void operator()(if_expr_op const& expr) const
        {
            QMessageBox::information(w,"text parser","EXPR1111");
        }
    };

    template <typename Lexer>
    struct dbase_tokens : lex::lexer<Lexer>
    {
        // ----------------------------
        // tokens with no attributes...
        // ----------------------------
        lex::token_def<lex::omit> whitespace;
        lex::token_def<lex::omit> cpcomment;
        lex::token_def<lex::omit> d2comment;
        lex::token_def<lex::omit> d_comment;
        lex::token_def<lex::omit> c_comment;

        lex::token_def<lex::omit> kw_class;
        lex::token_def<lex::omit> kw_of;
        lex::token_def<lex::omit> kw_endclass;
        lex::token_def<lex::omit> kw_new;

        lex::token_def<lex::omit> kw_this;
        lex::token_def<lex::omit> kw_true;
        lex::token_def<lex::omit> kw_false;
        lex::token_def<lex::omit> kw_and;
        lex::token_def<lex::omit> kw_or;

        lex::token_def<lex::omit> kw_parameter;
        lex::token_def<lex::omit> kw_local;
        lex::token_def<lex::omit> kw_if;
        lex::token_def<lex::omit> kw_else;
        lex::token_def<lex::omit> kw_endif;

        // --------------------------
        // tokens with attributes ...
        // --------------------------
        lex::token_def<char> printLn;
        lex::token_def<lex::omit> my_assign;

        lex::token_def<int>         number_digit;
        lex::token_def<std::string> identifier;
        lex::token_def<std::string> quoted_string;

        dbase_tokens()
        {
            // ------------
            // keywords ...
            // ------------
            kw_class        = "(?i:class)";
            kw_endclass     = "(?i:endclass)";
            kw_of           = "(?i:of)";
            kw_new          = "(?i:new)";

            kw_parameter    = "(?i:parameter)";
            kw_local        = "(?i:local)";
            kw_if           = "(?i:if)";
            kw_else         = "(?i:else)";
            kw_endif        = "(?i:endif)";

            kw_this         = "(?i:this)";

            kw_false        = "(\\.(?i:f)\\.)|(\\.(?i:false)\\.)";
            kw_true         = "(\\.(?i:t)\\.)|(\\.(?i:true)\\.)";
            kw_and          = "\\.(?i:and)\\.";
            kw_or           = "\\.(?i:or)\\.";

            printLn   = "\\\?";

            my_assign = "\\=";

            // Values.
            number_digit      = "[0-9]+";
            quoted_string     = "\\\"(\\\\.|[^\\\"])*\\\"";

            // Identifier.
            identifier        = "[a-zA-Z][a-zA-Z0-9_]*";

            cpcomment = "\\/\\/[^\\n]*\\n"; // single line comment
            d_comment = "\\*\\*[^\\n]*\\n"; // dBase  line comment
            d2comment = "\\&\\&[^\\n]*\\n"; // dBase  line comment
            c_comment = "\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/"; // c-style comments

            whitespace = "[ \\t\\r\\n]*";

            this->self +=
                  kw_parameter
                | kw_local
                | kw_if
                | kw_else
                | kw_endif
                ;

            this->self +=
                  kw_false
                | kw_true
                | kw_and
                | kw_or
                ;

            this->self +=
                  cpcomment  [ lex::_pass = lex::pass_flags::pass_ignore ]
                | c_comment  [ lex::_pass = lex::pass_flags::pass_ignore ]
                | d_comment  [ lex::_pass = lex::pass_flags::pass_ignore ]
                | d2comment  [ lex::_pass = lex::pass_flags::pass_ignore ]
                | whitespace [ lex::_pass = lex::pass_flags::pass_ignore ]
                ;

            this->self += lex::token_def<>
                    ('(') | ')'
                    | '*' | '+' | '-' | '/' | ',' | '.';
            this->self +=
                printLn
                ;
            this->self +=
                kw_new | kw_class | kw_of | kw_endclass | kw_this
                ;
            this->self +=
                  identifier
                | my_assign
                | number_digit
                | quoted_string
                ;
        }
    };

    template<typename StreamT>
    StreamT& operator<<(StreamT& out, expression_ast const& item) {
      out << "expression_ast " ;
      boost::apply_visitor(ast_print(), item.expr) ;
      return out ;
    }
    template<typename StreamT>
    StreamT& operator<<(StreamT& out, binary_op const& item) {
      out << "binary_op" << std::endl ;
      return out ;
    }
    template<typename StreamT>
    StreamT& operator<<(StreamT& out, unary_op const& item) {
      out << "unary_op" << std::endl ;
      return out ;
    }
    template<typename StreamT>
    StreamT& operator<<(StreamT& out, class_op const& item) {
      out << "class_op" << std::endl ;
      return out ;
    }
    template<typename StreamT>
    StreamT& operator<<(StreamT& out, if_expr_op const& item) {
      out << "if_expr_op" << std::endl ;
      return out ;
    }

    template <typename Iterator, typename Lexer>
    struct dbase_grammar
    :   public qi::grammar<Iterator>
    {   template <typename TokenDef>

        dbase_grammar(TokenDef const& tok) :
        dbase_grammar::base_type(start, "start")
        {
            using qi::_val;

            start
                = *symsbols
                ;

            expression =
                term                            [ _val  = qi::_1 ]
                >> *(
                    (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> term    [ phx::construct<expression_ast>(1,2,3) ])
                |   (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)")            [ phx::construct<expression_ast>(1,2,3) ])
                |   (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> eoi                                               [ phx::construct<expression_ast>(1,2,3) ])

                |   ('+' >> term            [ _val += qi::_1 ])
                |   ('-' >> term            [ _val -= qi::_1 ])

                    )
                ;

            term =
                factor                                     [ _val  = qi::_1]
                >> *(
                    (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> factor  [ phx::construct<expression_ast>(1,2,3) ])
                |   (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)")            [ phx::construct<expression_ast>(1,2,3) ])
                |   (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> eoi                                               [ phx::construct<expression_ast>(1,2,3) ])

                |   ('*' >> factor                     [ _val *= qi::_1])
                |   ('/' >> factor                     [ _val /= qi::_1])
                )
                ;

            factor =
                tok.number_digit                [ _val = qi::_1 ]

                |   tok.number_digit >> (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> factor  [ phx::construct<expression_ast>(1,2,3) ])
                |   tok.number_digit >> (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)")            [ phx::construct<expression_ast>(1,2,3) ])
                |   tok.number_digit >> (char_("([\\+\\-\\*\\/]+)|([a-zA-Z]*)") >> eoi                                               [ phx::construct<expression_ast>(1,2,3) ])

                |  '('   >> expression          [ _val = qi::_1 ] >> ')'
                |   ('-' >> factor              [ _val = neg(qi::_1)])
                |   ('+' >> factor              [ _val = qi::_1 ] )
                ;

            symsbols
            = ( comments
              | var_expr [ _val = qi::_1 ]
              | ident_false_true
              | objective_form
              | class_definition
              )
            ;

            ident_false_true
            = ((tok.identifier
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("ident_varname"),
                        phx::construct<std::string>(qi::_1),
                        phx::construct<std::string>("ident"))
                    ])
            >> *(char_('.') >> tok.identifier
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("ident_false_true"),
                        phx::construct<std::string>(qi::_1),
                        phx::construct<std::string>(qi::_1))
                    ]
               )
            >> tok.my_assign  >> ((tok.kw_false
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("ident_false"),
                        phx::construct<std::string>("false"),
                        phx::construct<std::string>("bool"))
                    ])
                    | (tok.kw_true
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("ident_true"),
                        phx::construct<std::string>("true"),
                        phx::construct<std::string>("bool"))
                    ])
                    )
                )
            ;

            ident_function
            = (tok.identifier >> *(char_('.') >> tok.identifier)
            >> char_('(')
            >> char_(')'))
            ;

            var_expr
            = (tok.identifier >>
               tok.my_assign  >>
               expression        [ _val = qi::_1 ] )
            ;

            objective_form
            = ( parameterwidget | localparameter | newobject | ifcondition)
            ;

            parameterwidget
            = ((tok.kw_parameter >> tok.identifier
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("addparameter1"),
                        phx::construct<std::string>(qi::_1),
                        phx::construct<std::string>(qi::_1))
                    ])
              | (tok.kw_parameter >> tok.identifier
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("addparameter2"),
                        phx::construct<std::string>(qi::_1),
                        phx::construct<std::string>(qi::_1))
                    ]
                    ) >> *(char_(',') >> tok.identifier
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("addparameter3"),
                        phx::construct<std::string>(qi::_1),
                        phx::construct<std::string>(qi::_1))
                    ]
                    )
              )
            ;

            localparameter
            = (tok.kw_local >> tok.identifier)
            ;

            newobject
            = (tok.identifier >> tok.my_assign
            >> tok.kw_new     >> tok.identifier >> char_('(') >> char_(')'))
            ;

            ifcondition
            = (tok.kw_if >> (char_('(') >> expression >> char_(')')
                    [
                        phx::construct<expression_ast>(
                        phx::construct<std::string>("ifexpr1"),
                        phx::construct<expression_ast>(qi::_1),
                        phx::construct<std::string>("expr"))
                    ]
                    )
            >> *(ident_false_true | ident_function)  >> (tok.kw_else | tok.kw_endif)
            >> *(ident_false_true | ident_function)  >> tok.kw_endif)
            ;

            comments
            = ( tok.cpcomment | tok.c_comment
              | tok.d_comment | tok.d2comment | tok.whitespace
              )
            ;

            class_definition
            = (tok.kw_class
               >> (tok.identifier [ phx::construct<expression_ast>(
                                    phx::construct<std::string>("create1class"),
                                    phx::construct<std::string>(qi::_1),
                                    phx::construct<std::string>("ident"))
                                  ] )
               >>  tok.kw_of
               >> (tok.identifier [ phx::construct<expression_ast>(
                                    phx::construct<std::string>("create2class"),
                                    phx::construct<std::string>(qi::_1),
                                    phx::construct<std::string>("class of"))
                                  ] )
               >> class_entries
               >> tok.kw_endclass)
            ;

            class_entry1
            = *((tok.kw_this >>
              *('.' >> tok.identifier)) >> tok.my_assign) ;

            class_entries
            = *(comments | class_entry1 >> expression) ;

            start.name("start");
            symsbols.name("symsbols");
            expression.name("expression");
            term.name("term");
            factor.name("factor");
            var_expr.name("var_expr");
            comments.name("comments");
            class_definition.name("class_definition");
            class_entry1.name("class_entry1");
            class_entries.name("class_entries");

            objective_form.name("objective_form");
            ifcondition.name("ifcondition");
            localparameter.name("localparameter");
            parameterwidget.name("parameterwidget");
            newobject.name("newobject");
            ident_function.name("ident_function");
            ident_false_true.name("ident_false_true");

                    /*
            BOOST_SPIRIT_DEBUG_NODE(start);
            BOOST_SPIRIT_DEBUG_NODE(symsbols);
            BOOST_SPIRIT_DEBUG_NODE(var_expr);
            BOOST_SPIRIT_DEBUG_NODE(factor);
            BOOST_SPIRIT_DEBUG_NODE(term);
            BOOST_SPIRIT_DEBUG_NODE(expression);
            BOOST_SPIRIT_DEBUG_NODE(comments);

            qi::debug(start);
            qi::debug(symsbols);
            qi::debug(factor);
            qi::debug(term);
            qi::debug(expression);
            qi::debug(var_expr);*/
        }

        typedef qi::unused_type skipper_type;
        typedef qi::rule<Iterator, skipper_type> simple_rule;

        simple_rule start, symsbols, comments
                    , class_definition
                    , class_entries
                    , class_entry1
                    , objective_form, ident_false_true, ident_function
                    , ifcondition, localparameter, parameterwidget, newobject
                    ;

        qi::rule<Iterator, expression_ast()>
             expression, term, factor, var_expr
           ;
    };
}

bool InitParseText(std::string text)
{
    std::string data = text;
    if (data.size() < 1)
    return false;

    namespace dp = dBaseParser;

    typedef std::string::iterator base_iterator_type;
    typedef lex::lexertl::token<
        base_iterator_type, boost::mpl::vector<char, int, std::size_t, std::string>
    > token_type;
    typedef lex::lexertl::actor_lexer<token_type> lexer_type;

    typedef dp::dbase_tokens<lexer_type> dbase_tokens;
    typedef dbase_tokens::iterator_type iterator_type;
    typedef dp::dbase_grammar<iterator_type, dbase_tokens::lexer_def> dbase_grammar;

    dbase_tokens  tokens;
    dbase_grammar dbase(tokens);

    base_iterator_type it = data.begin();
    iterator_type iter    = tokens.begin(it, data.end());
    iterator_type end     = tokens.end();

    dp::expression_ast ast;
    return qi::parse(iter, end, dbase, ast);
}

bool parseText(QString text, int mode)
{
    namespace dp = dBaseParser;

    dp::ast_print printer;

    dp::dynamics.clear();

    dp::vec_push_1.clear();
    dp::vec_push_2.clear();
    dp::vec_push_3.clear();

    dp::dBaseVariables *v = new
    dp::dBaseVariables;
    v->data_value_int = -2;
    v->data_name = "@root";
    dp::dynamics[dp::getVariable("@root")] = v;

    try {
        if (InitParseText(text.toStdString())) {
            QMessageBox::information(w,"text parser","SUCCESS");

            //if (dp::dynamics.size() < 0)
            //return true;

            printer(dp::expr);
            int bool_value = 0;

            for (int o = 0; o <= dp::dynamics.count(); o++)
            {
                if (dp::dynamics[o]->data_type == dp::dBaseTypes::p_value) {
                    dp::dBaseVariables *v =
                    dp::dynamics[
                    dp::getVariable(
                    dp::dynamics[o]->data_name)]; o++;

                    QString name = v->data_name;

                    if (v->data_type == dp::dBaseTypes::b_value) {
                        bool_value = v->data_value_bool;

                        if (!bool_value)
                        QMessageBox::information(w,"infor","false bool"); else
                        QMessageBox::information(w,"infor","true bool");
                    }
                }

                if (dp::dynamics[o]->data_type == dp::dBaseTypes::w_value) {
                if (dp::dynamics[o]->data_value_widget != nullptr)
                    dp::dynamics[o]->data_value_widget->showModal();
                }
            }
            dp::dynamics.clear();
            return true;
        }   else {
            QMessageBox::information(w,"text parser","ERROR");
            return false;
        }
    }
    catch (exception& e) {
        QMessageBox::information(w,"parser error",e.what());
        return false;
    }   return true;
}
