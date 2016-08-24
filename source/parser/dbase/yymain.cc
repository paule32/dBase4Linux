/*
using g++ 5.4.0

#/bin/sh
g++ -c -pipe -H \
-Wno-unused-parameter \
-Wno-unused-variable \
-Wno-unused-local-typedefs \
-Wno-unused-but-set-variable \
-Wno-write-strings \
-Wno-extra \
-Wno-reorder \
-Wunused-function \
-Woverloaded-virtual \
-fpermissive \
-ftemplate-depth=200 -frtti -fexceptions -std=c++17 \
-D__BYTE_ORDER=__LITTLE_ENDIAN -D_REENTRANT -fPIC \
-DBUILDTIME=\"14:43:09\" -DBUILDDATE=\"2016-08-22\" \
-I. \
-I/usr/include \
\
-o yymain.o yymain.cc

------------------------------------

    term =
    factor                          [ _val  = qi::_1]
    >> *(   (tok.my_mul >> skip(space)[
             tok.my_mul
            ]                       [lex::_pass = lex::pass_flags::pass_fail ])    // <--- here
        |   (tok.my_mul >> factor   [ _val *= qi::_1])
        |   ('/' >> factor          [ _val /= qi::_1])
        )
    ;


/usr/include/boost/proto/transform/default.hpp:154: Fehler: cannot convert 'const boost::spirit::lex::pass_flags' to 'bool' in assignment
BOOST_PROTO_BINARY_DEFAULT_EVAL(=, assign, make_mutable, make)
^

/usr/include/boost/proto/transform/default.hpp:154: Fehler: cannot convert 'const boost::spirit::lex::pass_flags' to 'bool' in assignment

/usr/include/boost/spirit/home/support/action_dispatch.hpp:178: Fehler: no match for call to '(const boost::phoenix::actor<boost::proto::exprns_::expr<boost::proto::tagns_::tag::assign, boost::proto::argsns_::list2<boost::proto::exprns_::basic_expr<boost::proto::tagns_::tag::terminal, boost::proto::argsns_::term<boost::phoenix::argument<3> >, 0l>, boost::phoenix::actor<boost::proto::exprns_::expr<boost::proto::tagns_::tag::terminal, boost::proto::argsns_::term<boost::spirit::lex::pass_flags>, 0l> > >, 2l> >) (boost::spirit::traits::pass_attribute<boost::spirit::qi::skip_parser<boost::spirit::lex::reference<const boost::spirit::lex::token_def<char>, long unsigned int>, boost::spirit::qi::char_class<boost::spirit::tag::char_code<boost::spirit::tag::space, boost::spirit::char_encoding::ascii> > >, char, void>::type&, boost::spirit::context<boost::fusion::cons<dBaseParser::expression_ast&, boost::fusion::nil>, boost::fusion::vector0<> >&, bool&)'
 f(attr_wrap, context, pass);
  ^

/usr/include/boost/type_traits/make_unsigned.hpp:38: Fehler: static assertion failed: (::boost::type_traits::ice_or< ::boost::is_integral<T>::value, ::boost::is_enum<T>::value>::value)
BOOST_STATIC_ASSERT(
^

/usr/include/boost/type_traits/make_signed.hpp:38: Fehler: static assertion failed: (::boost::type_traits::ice_or< ::boost::is_integral<T>::value, ::boost::is_enum<T>::value>::value)
BOOST_STATIC_ASSERT(
^
*/


#ifdef QT_CORE_LIB
#include "includes/mainwindow.h"
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

int lineno = 1;
bool MissThrower = false;

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
    // --------------------------------
    // exception class for don't match
    // -------------------------------
    class MydBaseMissException: public exception
    {
        virtual const char* what() const throw()
        {
            return "dBaseException occur.";
        }
    } dBaseMissException;

    // -----------------
    // AST for dBase ...
    // -----------------
    struct binary_op;
    struct unary_op;
    struct nil { };
    //struct dBaseExpression;
    //struct class_op;

    enum dBaseTypes {
        unknown,
        m_value,
        c_value,
        i_value         // int type
    };
    dBaseTypes dBaseType;
    class dBaseVariants
    {
    public:
        std::string data_name;
        dBaseTypes  data_type;
        int         data_value_int;
    };
    std::vector<dBaseVariants> dynamics(200);

    struct expression_ast
    {
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
           // , boost::recursive_wrapper<class_op>
           // , boost::recursive_wrapper<dBaseExpression>
        >
        type;
        type expr;

        expression_ast() : expr(nil()) { }

        expression_ast(int dummy) {
            cout << "trower" << endl;
            throw dBaseMissException;
        }

        template <typename Expr>
        expression_ast(Expr const & expr)
            : expr(expr) { }

        /*
        expression_ast(
                  std::string const& name
                , std::string const& str1
                , std::string const& str2
                , expression_ast const& rhs);

        expression_ast(expression_ast const & rhs, std::string const & name); */

        expression_ast& operator += (expression_ast const & rhs);
        expression_ast& operator -= (expression_ast const & rhs);
        expression_ast& operator *= (expression_ast const & rhs);
        expression_ast& operator /= (expression_ast const & rhs);
    };

    expression_ast dast;

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
    struct unary_op
    {
        unary_op(
            char op
          , expression_ast const& subject)
        : op(op), subject(subject) {}

        char op;
        expression_ast subject;
    };

    /*
    struct class_op
    {
        class_op(
              std::string const& op
            , std::string const& cname
            , std::string const& oname
            , expression_ast const& left
            , expression_ast const& right)
            : op(op)
            , class_cname(cname)
            , class_oname(oname)
            , left(left)
            , right(right) { }

        std::string op;
        std::string class_cname;
        std::string class_oname;

        expression_ast left;
        expression_ast right;
    };*/

/*
    expression_ast::expression_ast(expression_ast const & rhs, std::string const & name)
    {
        cout << "dinit" << endl;
        cout << name << endl;
        cout << rhs.expr.type().name() << endl;

        expr = rhs;
    }

    expression_ast::expression_ast  (
              std::string const& name
            , std::string const& str1
            , std::string const& str2, expression_ast const& rhs)
    {
        expr = class_op(name, str1, str2, *this, rhs);
    }
*/
    expression_ast& expression_ast::operator += (expression_ast const& rhs)
    {
        expr = binary_op('+', expr, rhs);
        dast = expr;
        return *this;
    }

    expression_ast& expression_ast::operator -= (expression_ast const& rhs)
    {
        expr = binary_op('-', expr, rhs);
        dast = expr;
        return *this;
    }

    expression_ast& expression_ast::operator *= (expression_ast const& rhs)
    {
        expr = binary_op('*', expr, rhs);
        dast = expr;
        return *this;
    }

    expression_ast& expression_ast::operator /= (expression_ast const& rhs)
    {
        expr = binary_op('/', expr, rhs);
        dast = expr;
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
            dast = expression_ast(unary_op('-', expr));
            return dast;
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
            dast.expr = nil();
        }
        void operator()(int n) const {
            std::cout << n;
            dast.expr  = n;

            dynamics[0].data_name      = std::string("onnnn");
            dynamics[0].data_type      = i_value;
            dynamics[0].data_value_int = int(n);
        }


        void operator()(expression_ast const& ast) const
        {
            cout << "-> "
                 << ast.expr.type().name()
                 << endl;

            dast = ast;
            //if (!(ast.expr.type().name() == std::string("N11dBaseParser3nilE")))
            boost::apply_visitor(*this, ast.expr);
        }

        void operator()(binary_op const& expr) const
        {
            int lval, rval, value;

            std::cout << "op" << expr.op << "(";
            dast = expr.left;
            boost::apply_visitor(*this, expr.left.expr);
            try {
                lval = boost::get<int>(dast.expr);
            }   catch (...) {
                lval = dynamics[0].data_value_int;
            }


            std::cout << ", ";
            dast = expr.right;
            boost::apply_visitor(*this, expr.right.expr);
            try {
                rval = boost::get<int>(dast.expr);
            }   catch (...) {
                rval = dynamics[0].data_value_int;
            }

            std::cout << ')';

            switch (expr.op) {
            case '+': value = lval + rval; break;
            case '-': value = lval - rval; break;
            case '*': value = lval * rval; break;
            case '/': value = lval / rval; break;
            }

            dynamics[0].data_name      = std::string("onnnn");
            dynamics[0].data_type      = i_value;
            dynamics[0].data_value_int = value;

            dast.expr = value;
        }

        void operator()(unary_op const& expr) const
        {
            std::cout << "oP" << expr.op << "(";
            boost::apply_visitor(*this, expr.subject.expr);
            std::cout << ')';
        }

        /*
        void operator()(class_op const& expr) const
        {
            std::cout << "class:"
                      << expr.class_cname << ":"
                      << expr.class_oname
                      << "(null)"
                      << std::endl;
            boost::apply_visitor(*this, expr.left.expr);
        }*/
    };

    template <typename Lexer>
    struct dbase_tokens : lex::lexer<Lexer>
    {
        // ----------------------------
        // tokens with no attributes...
        // ----------------------------
        lex::token_def<lex::omit> whitespace;
        lex::token_def<lex::omit> cpcomment;
        lex::token_def<lex::omit> d_comment;
        lex::token_def<lex::omit> c_comment;

        lex::token_def<lex::omit> kw_class;
        lex::token_def<lex::omit> kw_of;
        lex::token_def<lex::omit> kw_endclass;

        lex::token_def<lex::omit> miss_1;
        lex::token_def<char> my_mul;

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

            printLn   = "\\\?";

            my_assign = "\\=";
            my_mul    = "\\*";

            miss_1 = "[0-9]*((\\*)([ \\t\\r\\n]+)(\\*))";

            // Values.
            number_digit      = "[0-9]*";
            quoted_string     = "\\\"(\\\\.|[^\\\"])*\\\"";

            // Identifier.
            identifier        = "[a-zA-Z][a-zA-Z0-9_]*";

            cpcomment = "\\/\\/[^\\n]*\\n";                    // single line comment
            d_comment = "\\*\\*[^\\n]*\\n";                    // dBase  line comment
            c_comment = "\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/"; // c-style comments

            whitespace = "[ \\t\\n]+";

            this->self += lex::token_def<>
                    ('(') | ')'
                    | '+' | '-'
                    | '/'
                    | ',' | '.';
            this->self +=
                printLn | my_mul
                ;
            this->self +=
                kw_class | kw_of | kw_endclass
                ;
            this->self +=
                  identifier
                | my_assign
                | number_digit
                | quoted_string
                ;

            this->self +=
                  miss_1     [ lex::_pass = lex::pass_flags::pass_fail ]
                | whitespace [ lex::_pass = lex::pass_flags::pass_ignore ]
                | cpcomment  [ lex::_pass = lex::pass_flags::pass_ignore ]
                | c_comment  [ lex::_pass = lex::pass_flags::pass_ignore ]
                | d_comment  [ lex::_pass = lex::pass_flags::pass_ignore ]
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

    template <typename Iterator, typename Lexer>
    struct dbase_grammar
    :   public qi::grammar<Iterator>
    {   template <typename TokenDef>

        dbase_grammar(TokenDef const& tok) :
        dbase_grammar::base_type(start, "start")
        {
            using qi::_val;

            start
                = +symsbols
                ;

            expression =
                term                            [ _val  = qi::_1 ]
                >> *(   ('+' >> term            [ _val += qi::_1 ])
                    |   ('-' >> term            [ _val -= qi::_1 ])
                    )
                ;

            term =
                factor                          [ _val  = qi::_1]
                >> *(   (tok.my_mul >> skip(space)[
                         tok.my_mul
                        ]                       [ _val  = phx::construct<expression_ast>(2) ])
                    |   (tok.my_mul >> factor   [ _val *= qi::_1])
                    |   ('/' >> factor          [ _val /= qi::_1])
                    )
                ;

            factor =
                tok.number_digit                [ _val = qi::_1 ]
                |  '('   >> expression          [ _val = qi::_1 ] >> ')'
                |   ('-' >> factor              [ _val = neg(qi::_1)])
                |   ('+' >> factor              [ _val = qi::_1 ] )
                ;

            symsbols
                = tok.miss_1 [ _val = phx::construct<expression_ast>(1) ]
                | printLn
                | comments
  //              | class_definition
                | h_expression
                ;

            h_expression
                = (tok.identifier
                >> tok.my_assign
                >> expression               [ _val = qi::_1 ] )
                ;

            comments
                = tok.cpcomment
                | tok.c_comment
                | tok.d_comment
                ;

            printLn
                = tok.printLn >> tok.quoted_string
                ;

                    /*
            class_definition
                =   (  tok.kw_class      >> *comments
                    >> tok.identifier    >> *comments
                    >> tok.kw_of         >> *comments
                    >> tok.identifier    >> *comments >> class_body
                    >> tok.kw_endclass)
                    [
                        qi::_val = phx::construct<expression_ast>(
                        phx::construct<std::string>(qi::_1),
                        phx::construct<std::string>(qi::_2))
                    ]
                ;
            class_body
                = *comments
                ;
                        */

            start.name("start");
            symsbols.name("symsbols");
            comments.name("comments");
            expression.name("expression");
            term.name("term");
            factor.name("factor");
            printLn.name("printLn");
//            class_definition.name("class_definition");
//            class_body.name("class_body");
            h_expression.name("h_expression");

            BOOST_SPIRIT_DEBUG_NODE(start);
            BOOST_SPIRIT_DEBUG_NODE(symsbols);
            BOOST_SPIRIT_DEBUG_NODE(comments);
            BOOST_SPIRIT_DEBUG_NODE(printLn);
//            BOOST_SPIRIT_DEBUG_NODE(class_definition);
//            BOOST_SPIRIT_DEBUG_NODE(class_body);
            BOOST_SPIRIT_DEBUG_NODE(factor);
            BOOST_SPIRIT_DEBUG_NODE(term);
            BOOST_SPIRIT_DEBUG_NODE(expression);
            BOOST_SPIRIT_DEBUG_NODE(h_expression);

            qi::debug(start);
            qi::debug(symsbols);
            qi::debug(factor);
            qi::debug(term);
            qi::debug(expression);
        }

        typedef qi::unused_type skipper_type;
        typedef qi::rule<Iterator, skipper_type> simple_rule;

        simple_rule start, symsbols, comments, printLn;
        simple_rule class_body;

        qi::rule<Iterator, expression_ast()>
             expression, term, factor
           , h_expression
           , class_definition
           ;
    };
}

bool InitParseText(std::string text)
{
    std::string data = text;
    //std::string data(text.toStdString().c_str());
    if (data.size() < 1) {
#ifndef QT_CORE_LIB
        //std::cerr << "no data for parser" << std::endl;
#else
        QMessageBox::information(0,"Error","No Data for parser.\nABORT.");
#endif
        return false;
    }

    using dBaseParser::expression_ast;

    typedef std::string::iterator base_iterator_type;
    typedef lex::lexertl::token<
        base_iterator_type, boost::mpl::vector<char, int, std::size_t, std::string>
    > token_type;
    typedef lex::lexertl::actor_lexer<token_type> lexer_type;

    typedef dBaseParser::dbase_tokens<lexer_type> dbase_tokens;
    typedef dbase_tokens::iterator_type iterator_type;
    typedef dBaseParser::dbase_grammar<iterator_type, dbase_tokens::lexer_def> dbase_grammar;

    dbase_tokens  tokens;
    dbase_grammar dbase(tokens);

    base_iterator_type it = data.begin();
    iterator_type iter    = tokens.begin(it, data.end());
    iterator_type end     = tokens.end();

    expression_ast ast;
    return qi::parse(iter, end, dbase, ast);
}

#ifndef QT_CORE_LIB
bool parseText(std::string text, int mode)
#else
bool parseText(QString text, int mode)
#endif
{
    dBaseParser::ast_print  printer;
    dBaseParser::dynamics.clear();

    try {
        if (InitParseText(text.toStdString())) {
#ifndef QT_CORE_LIB
            std::cout << "SUCCESS" << std::endl;
#else
            QMessageBox::information(w,"text parser","SUCCESS");
#endif
            printer(dBaseParser::dast);

            int val = 0;
            {
                val = dBaseParser::dynamics[0].data_value_int;

#ifndef QT_CORE_LIB
                cout << val;
#else
                w->ui->warningMemo->addItem(QString("--> %1")
                .arg(val));
#endif
            }

            //w->ui->warningMemo->addItem("-----");
            //.arg(val));
        } else {
#ifndef QT_CORE_LIB
            std::cout << "ERROR" << std::endl;
#else
            QMessageBox::information(w,"text parser","ERROR");
#endif
        }
    }
    catch (exception& e) {
#ifndef QT_CORE_LIB
        cout << "error: " << e.what() << endl;
#else
        QMessageBox::information(w,"parser error",e.what());
#endif
    }

    return 0;
}
