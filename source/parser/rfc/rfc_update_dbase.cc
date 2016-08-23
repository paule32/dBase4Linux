#include "includes/mainwindow.h"

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_ACTIONS_ALLOW_ATTR_COMPAT

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
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

#include <boost/phoenix/bind/bind_member_function.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <utility>

using namespace std;
using namespace boost::spirit;

namespace bs    = boost::spirit;
namespace phx   = boost::phoenix;
namespace ascii = boost::spirit::ascii;

using boost::spirit::ascii::space; // use the ASCII space parser
using boost::spirit::ascii::char_;
using boost::spirit::_val;
using boost::spirit::qi::eoi;

using boost::phoenix::val;

namespace RFCupdateParser
{
    // --------------------------
    // AST for dBase updater ...
    // --------------------------
    struct binary_op;
    struct unary_op;
    struct nil { };
    struct header_1struct;

    int errors = 1;
    QString  dummy;

    struct expression_ast
    {
        typedef
            boost::variant<
                  nil
                , int
                , float
                , std::string
                , boost::recursive_wrapper<expression_ast>
                , boost::recursive_wrapper<binary_op>
                , boost::recursive_wrapper<unary_op>
                , boost::recursive_wrapper<header_1struct>
            >
        type;
        type expr;

        expression_ast() : expr(nil()) {}
        expression_ast(
                  std::string const& name
                , float version
                , int status_code
                , std::string const& status_text, expression_ast const& rhs);

        template <typename Expr>
            expression_ast(Expr const& expr)
            : expr(expr) {}

        expression_ast& operator += (expression_ast const& rhs);
        expression_ast& operator -= (expression_ast const& rhs);
        expression_ast& operator *= (expression_ast const& rhs);
        expression_ast& operator /= (expression_ast const& rhs);
    };

    struct binary_op
    {
        binary_op(
              char op
            , expression_ast const& left
            , expression_ast const& right)
            : op(op)
            , left(left)
            , right(right) {}

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


    expression_ast dast;

    struct header_1struct
    {
        header_1struct(
              std::string name
            , float       version
            , int         status_code
            , std::string status_text
            , expression_ast const& left
            , expression_ast const& right)
            : name(name)
            , version(version)
            , status_code(status_code)
            , status_text(status_text)
            , left(left)
            , right(right) { }

        std::string name;
        std::string status_text;
        int         status_code;
        float version;

        expression_ast left;
        expression_ast right;
    };

    expression_ast::expression_ast(string const &name
            , float version
            , int status_code
            , string const& status_text
            , expression_ast const& rhs)
    {
        expr = header_1struct(
                      name
                    , version
                    , status_code
                    , status_text
                    , expr
                    , rhs
               );
        dast = expr;
    }

    expression_ast& expression_ast::operator += (expression_ast const& rhs)
    {
        expr = binary_op('+', expr, rhs);
        dast = *this;
        return  dast;
    }

    expression_ast& expression_ast::operator -= (expression_ast const& rhs)
    {
        expr = binary_op('-', expr, rhs);
        dast = *this;
        return  dast;
    }

    expression_ast& expression_ast::operator *= (expression_ast const& rhs)
    {
        expr = binary_op('*', expr, rhs);
        dast = *this;
        return  dast;
    }

    expression_ast& expression_ast::operator /= (expression_ast const& rhs)
    {
        expr = binary_op('/', expr, rhs);
        dast = *this;
        return  dast;
    }

    // We should be using expression_ast::operator-. There's a bug
    // in phoenix type deduction mechanism that prevents us from
    // doing so. Phoenix will be switching to BOOST_TYPEOF. In the
    // meantime, we will use a phoenix::function below:
    struct negate_expr
    {
        template <typename T>
        struct result { typedef T type; };

        expression_ast operator()(expression_ast const& expr) const
        {
            return expression_ast(unary_op('-', expr));
        }
    };

    boost::phoenix::function<negate_expr> neg;

    // -----------------------
    // walk throug the AST ...
    // -----------------------
    struct ast_print
    {
        typedef void result_type;

        void operator()(nil) const   {
            cout << "empty" << endl;
        }
        void operator()(int n) const { std::cout << n; }

        void operator()(expression_ast const& ast) const
        {
            cout << "-> "
                 << ast.expr.type().name()
                 << " : "
                 << endl;

            boost::apply_visitor(*this, ast.expr);
        }

        void operator()(binary_op const& expr) const
        {
            std::cout << "op:" << expr.op << "(";
            boost::apply_visitor(*this, expr.left.expr);
            std::cout << ", ";
            boost::apply_visitor(*this, expr.right.expr);
            std::cout << ')';
        }

        void operator()(unary_op const& expr) const
        {
            std::cout << "op:" << expr.op << "(";
            boost::apply_visitor(*this, expr.subject.expr);
            std::cout << ')';
        }

        void operator()(header_1struct const& expr) const
        {
            std::cout << "header:"
                      << expr.status_text
                      << std::endl;
            boost::apply_visitor(*this, expr.left.expr);
            std::cout << "header2:"
                      << expr.status_text
                      << std::endl;
        }
    };

    template <typename Lexer>
    struct rfcupdate_tokens : lex::lexer<Lexer>
    {
        // ----------------------------
        // tokens with no attributes...
        // ----------------------------
        lex::token_def<lex::omit> whitespace;
        lex::token_def<lex::omit> cpcomment;
        lex::token_def<lex::omit> d_comment;
        lex::token_def<lex::omit> c_comment;
        lex::token_def<lex::omit> any_content;

        lex::token_def<std::string> kw_http_ok;
        lex::token_def<std::string> kw_http_notfound;
        lex::token_def<std::string> kw_http_date;
        lex::token_def<std::string> kw_http_server;
        lex::token_def<std::string> kw_http_vary;
        lex::token_def<std::string> kw_http_content_len;
        lex::token_def<std::string> kw_http_content_typ;

        lex::token_def<std::string> kw_http_last_modify;
        lex::token_def<std::string> kw_http_etag;
        lex::token_def<std::string> kw_http_acceptrange;
        lex::token_def<std::string> kw_http_x_pad;
        lex::token_def<std::string> kw_http_body_sep;
        lex::token_def<std::string> kw_http_body_update;

        lex::token_def<std::string> identifier;
        lex::token_def<std::string> quoted_string;

        rfcupdate_tokens()
        {
            // ----------------------------
            // keywords ...
            // ----------------------------
            /*
            HTTP/1.1 404 Not Found
            Date: Thu, 21 Jul 2016 12:40:21 GMT
            Server: Apache/2.2.22 (Debian)
            Vary: Accept-Encoding
            Content-Length: 290
            Content-Type: text/html; charset=iso-8859-1 */

            kw_http_ok          = "HTTP\\/1\\.1 200 OK\\r\\n";
            kw_http_notfound    = "HTTP\\/1\\.1 404 Not Found\\r\\n";

            kw_http_date        = "Date\\: (Mon|Tue|Wed|Thu|Fri|Sat|Sun)\\, ([0-9]*) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) ([0-9]*) ([0-9]*\\:[0-9]*\\:[0-9]*) GMT\\r\\n";
            kw_http_server      = "Server\\: Apache\\/2\\.2\\.22 \\(Debian\\)\\r\\n";
            kw_http_vary        = "Vary\\: Accept\\-Encoding\\r\\n";
            kw_http_content_len = "Content\\-Length\\: [0-9]*\\r\\n";
            kw_http_content_typ = "Content\\-Type\\: ((text\\/[a-zA-Z]*\\; charset=iso\\-8859\\-1)|(text\\/plain))\\r\\n(.*\\r|\\n)*";

            kw_http_last_modify = "Last\\-Modified\\: (Mon|Tue|Wed|Thu|Fri|Sat|Sun)\\, ([0-9]*) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) ([0-9]*) ([0-9]*\\:[0-9]*\\:[0-9]*) GMT\\r\\n";
            kw_http_etag        = "ETag\\: \\\"[a-zA-Z0-9_\\-]*\\\"\\r\\n";
            kw_http_acceptrange = "Accept\\-Ranges\\: bytes\\r\\n";
            kw_http_x_pad       = "X\\-Pad\\: avoid browser bug\\r\\n";

            kw_http_body_sep    = "\\r\\n";
            kw_http_body_update = "Latest\\-Update\\: [0-9]*\\-[0-9]*\\-[0-9]*\\n";

            any_content = "(.*\\r\\n)*";


            quoted_string  = "\\\"(\\\\.|[^\\\"])*\\\""; // \"(\\.|[^\"])*\"

            // Identifier.
            identifier        = "[a-zA-Z][a-zA-Z0-9_\\-]*";

            cpcomment = "\\/\\/[^\\n]*\\n";                    // single line comment
            d_comment = "\\*\\*[^\\n]*\\n";                    // dBase  line comment
            c_comment = "\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/"; // c-style comments

            whitespace = "[ \\t\\n]+";

            this->self += lex::token_def<>
                    ('(') | ')'
                    | '+' | '-'
                    | '*' | ',' | '.' | '/'
                    ;

            this->self +=
                  kw_http_ok
                | kw_http_notfound
                | kw_http_date
                | kw_http_server
                | kw_http_vary
                | kw_http_content_len
                | kw_http_content_typ
                | kw_http_last_modify
                | kw_http_etag
                | kw_http_acceptrange
                | kw_http_x_pad
                | kw_http_body_sep
                | kw_http_body_update
                | any_content
                ;

            this->self +=
                  identifier
                | quoted_string
                ;

            this->self +=
                  whitespace [ lex::_pass = lex::pass_flags::pass_ignore ]
                | cpcomment
                | c_comment
                | d_comment
                ;
        }
    };

    template <typename Iterator, typename Lexer>
    struct rfcupdate_grammar
    :   public qi::grammar<Iterator>
    {   template <typename TokenDef>

        rfcupdate_grammar(TokenDef const& tok) :
        rfcupdate_grammar::base_type(start, "start")
        {
            using qi::_val;

            start
                = +symsbols
                ;

            symsbols
                = comments
                | rfc_header
                ;

            comments
                = tok.whitespace | tok.cpcomment
                | tok.c_comment
                | tok.d_comment
                ;

            rfc_header
                = tok.kw_http_notfound
                | tok.kw_http_ok
                | tok.kw_http_date
                | tok.kw_http_server
                | tok.kw_http_vary
                | tok.kw_http_content_len
                | tok.kw_http_content_typ
                | tok.kw_http_last_modify
                | tok.kw_http_etag
                | tok.kw_http_acceptrange
                | tok.kw_http_x_pad
                | tok.kw_http_body_sep
                | tok.kw_http_body_update
                  [
                    qi::_val = phx::construct<expression_ast>(
                          std::string("class")
                        , 1.11
                        , 200
                        , std::string("ok")
                        , qi::_val)
                  ]
                ;

            start.name("start");
            symsbols.name("symsbols");
            comments.name("comments");
            rfc_header.name("rfc_header");

            BOOST_SPIRIT_DEBUG_NODE(start);
            BOOST_SPIRIT_DEBUG_NODE(symsbols);
            BOOST_SPIRIT_DEBUG_NODE(comments);
            BOOST_SPIRIT_DEBUG_NODE(rfc_header);
        }

        typedef qi::unused_type skipper_type;
        typedef qi::rule<Iterator, skipper_type> simple_rule;

        simple_rule start, symsbols, comments;

        qi::rule<Iterator, expression_ast()>
              rfc_header;
    };
}

int InitParseTextRFC(QString text)
{
    //QMessageBox::information(w,"info",text);

    std::string data(text.toStdString().c_str());
    if (data.size() < 1) {
        QMessageBox::information(0,"Error","No Data for parser.\nABORT.");
        return 0;
    }

    using RFCupdateParser::expression_ast;
    using RFCupdateParser::ast_print;

    typedef std::string::iterator base_iterator_type;
    typedef lex::lexertl::token<
        base_iterator_type, boost::mpl::vector<char, int, float, std::size_t, std::string>
    > token_type;
    typedef lex::lexertl::actor_lexer<token_type> lexer_type;

    typedef RFCupdateParser::rfcupdate_tokens<lexer_type> rfcupdate_tokens;
    typedef rfcupdate_tokens::iterator_type iterator_type;
    typedef RFCupdateParser::rfcupdate_grammar<iterator_type, rfcupdate_tokens::lexer_def> rfcupdate_grammar;

    rfcupdate_tokens  tokens;
    rfcupdate_grammar rfcupdate(tokens);

    base_iterator_type it = data.begin();
    iterator_type iter    = tokens.begin(it, data.end());
    iterator_type end     = tokens.end();

    RFCupdateParser::expression_ast ast;
    bool r = qi::parse(iter, end, rfcupdate, ast);

    if (r == true)
    return 1;
    return 0;
}

int parseRFC_dBaseUpdate(QString text, int mode)
{
    //RFCupdateParser::ast_print  printer;
    QStringList str = text.split("\r\n");
    static int r = 0;


    r = InitParseTextRFC(QString("%1").arg(str.at(0)));
    if (r == 1 && str.at(0).contains("200 OK"))
    {   for (int i=1; i<13; i++)
        {
            if (i < 11)
            {
                if (!InitParseTextRFC(QString("%1").arg(str.at(i))))
                {   QMessageBox::information(w,"update server","ERROR <-----");
                    r = 0;
                    break;
                }

                if (i == 6 && mode == 1) {
                    RFCupdateParser::dummy = str.at(6).toStdString().substr(16,10).c_str();
                    r = QString(RFCupdateParser::dummy).toInt();
                    break;
                }
            }
            else if (i == 11 && mode == 0) {
                if (InitParseTextRFC(QString("%1").arg(str.at(i)))
                && (str.at(11).contains("Latest-Update:"))) {
                    RFCupdateParser::dummy = str.at(11).toStdString().substr(15,10).c_str();
                    QDate dateA = QDate::fromString(RFCupdateParser::dummy,"yyyy-MM-dd");
                    QDate dateB = QDate::fromString(BUILDDATE,"yyyy-MM-dd");

                    QString date1 = dateA.toString();
                    QString date2 = dateB.toString();

                    if (date1 < date2) {
                        QMessageBox::information(w,"info","There is an update available.");
                        r = 1; break; } else {
                        r = 0; break;
                    }
                }
            }
            else if (i == 6 && mode == 1) {
                r = QString(RFCupdateParser::dummy).toInt();
                break;
            }   else {
                QMessageBox::information(w,"update-server","Sorry, problem with protocol.");
                r = 0;
                break;
            }
        }
    }   else {
        QMessageBox::information(w,"update server","Sorry, the update server is not available.");
        r = 0;
    }   return r;
}
