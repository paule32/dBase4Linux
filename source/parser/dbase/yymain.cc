//#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_ACTIONS_ALLOW_ATTR_COMPAT

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define USE_QT
#ifdef  USE_QT
#include <QMessageBox>
#endif

using namespace std;

using boost::phoenix::function;
using boost::phoenix::ref;
using boost::phoenix::size;

using namespace boost::spirit;
using namespace boost::spirit::qi;

namespace client
{
     namespace fusion = boost::fusion;
     namespace phoenix = boost::phoenix;

     namespace qi = boost::spirit::qi;
     namespace ascii = boost::spirit::ascii;

     enum byte_code
     {
         op_neg,         //  negate the top stack entry
         op_add,         //  add top two stack entries
         op_sub,         //  subtract top two stack entries
         op_mul,         //  multiply top two stack entries
         op_div,         //  divide top two stack entries

         op_not,         //  boolean negate the top stack entry
         op_eq,          //  compare the top two stack entries for ==
         op_neq,         //  compare the top two stack entries for !=
         op_lt,          //  compare the top two stack entries for <
         op_lte,         //  compare the top two stack entries for <=
         op_gt,          //  compare the top two stack entries for >
         op_gte,         //  compare the top two stack entries for >=

         op_and,         //  logical and top two stack entries
         op_or,          //  logical or top two stack entries

         op_add_var,     //  add new variable
         op_load,        //  load a variable
         op_store,       //  store a variable

         op_double,      //  push constant integer into the stack
         op_true,        //  push constant 0 into the stack
         op_false,       //  push constant 1 into the stack

         op_jump_if,     //  jump to an absolute position in the code if top stack
                         //  evaluates to false
         op_jump,        //  jump to an absolute position in the code

         op_stk_adj,     // adjust the stack (for args and locals)
         op_call,        // function call
         op_return       // return from function
     };

     class vmachine
     {
     public:

         vmachine(unsigned stackSize = 4096)
           : stack(stackSize)
         {
         }

         int execute(
             std::vector<int> const& code            // the program code
           , std::vector<int>::const_iterator pc     // program counter
           , std::vector<int>::iterator frame_ptr    // start of arguments and locals
         );

         std::vector<int> stack;
     };

     struct error_handler_
     {
         template <typename, typename, typename>
         struct result { typedef void type; };

         template <typename Iterator>
         void operator()(
             info const& what
           , Iterator err_pos, Iterator last) const
         {
             printf("EEEEEEEEEEEEEEEEEERRRRRR\n");
             std::stringstream ss;
             ss  << "Error! Expecting "
                 << what                         // what failed?
                 << " here: \""
                 << std::string(err_pos, last)   // iterators to error-pos, end
                 << "\""
                 << std::endl
             ;
             std::cout << ss.str();
             QMessageBox::information(0,"Parser", ss.str().c_str());
         }
     };
     boost::phoenix::function<client::error_handler_> const error_handler = client::error_handler_();

     template <typename Iterator>
     struct dbase_skipper : public qi::grammar<Iterator>
     {
         dbase_skipper() : dbase_skipper::base_type(my_skip, "dBase")
         {
             using qi::ascii::char_;
             using qi::ascii::space;
             using qi::eol;
             using qi::eoi;

             using qi::on_error;
             using qi::fail;

             my_skip =  (char_("[ \t\n\r]"))                            |
             ("**" >> *((char_("äöüÄÖÜß") | char_) - eol) >> (eol | eoi | char_("[\n\r]"))) |
             ("&&" >> *((char_("äöüÄÖÜß") | char_) - eol) >> (eol | eoi | char_("[\n\r]"))) |
             ("//" >> *((char_("äöüÄÖÜß") | char_) - eol) >> (eol | eoi | char_("[\n\r]"))) |
             ("/*" >> *((char_("äöüÄÖÜß") | char_) - "*/") >> "*/")
             ;

             on_error<fail>
              (
                  my_skip
                , std::cout
                      << boost::phoenix::val("Error! Expecting comment")
                      << std::endl
              );

             BOOST_SPIRIT_DEBUG_NODE((my_skip));
         }
         qi::rule<Iterator> my_skip;
     };

     struct my_ops {
         byte_code op_code;
         std::string name;
         double value;
     };
     std::vector<struct my_ops> code;

     struct compile_op
     {
         template <typename A, typename B = unused_type, typename C = unused_type>
         struct result { typedef void type; };

         compile_op() { }
         compile_op(std::vector<my_ops> _code)
         {
             cout << "weiter00000" << endl;
             code = _code;
             cout << "weiter11111" << endl;
         }


         void operator()(const byte_code&, boost::spirit::unused_type& ut) const
         {
             cout << "unnnnn" << endl;
         }

         void operator()(const byte_code &a) const
         {
             cout << "weiter22222" << endl;

             struct my_ops my_pspush;
             my_pspush.op_code = a;
             my_pspush.name    = "";
             my_pspush.value   = 0.00;
             code.push_back(my_pspush);

             cout << "weiter333333" << endl;
         }

         void operator()(const byte_code &a, double &b) const
         {
             cout << "weiter4444444" << endl;

             struct my_ops my_pspush;
             my_pspush.op_code = a;
             my_pspush.name    = "";
             my_pspush.value   = b;
             code.push_back(my_pspush);

             cout << "weiter55555" << endl;
         }

         void operator()(const byte_code &a, const std::string &b) const
         {
             cout << "weiter1" << endl;
             cout << b << endl;
             cout << "weiter2" << endl;

             struct my_ops my_pspush;
             my_pspush.op_code = a;
             my_pspush.name    = b;
             my_pspush.value   = 0.00;
             code.push_back(my_pspush);
         }

         /*
         void operator()(byte_code a, byte_code b, byte_code c)
         {
             code.push_back(a);
             code.push_back(b);
             code.push_back(c);
         }*/
     };



     template <typename Iterator, typename Skipper = dbase_skipper<Iterator>>
     struct dbase_grammar : public qi::grammar<Iterator, Skipper>
     {
         qi::rule<Iterator, Skipper> start, run_app;
         dbase_grammar()  : dbase_grammar::base_type(start)
         {
             std::stringstream ident_buffer;

             using boost::spirit::ascii::no_case;

             using qi::lit;
             using qi::char_;
             using qi::lexeme;

             using qi::on_error;
             using qi::fail;

             start = * symsbols;

             expression    = equality_expr.alias();
             equality_expr =
                 relational_expr
                 >> *(   ("==" > relational_expr     [op(op_eq)])
                     |   ("!=" > relational_expr     [op(op_neq)])
                     )
                 ;

             relational_expr =
                 logical_expr
                 >> *(   ("<=" > logical_expr        [op(op_lte)])
                     |   ('<' > logical_expr         [op(op_lt)])
                     |   (">=" > logical_expr        [op(op_gte)])
                     |   ('>' > logical_expr         [op(op_gt)])
                     )
                 ;

             logical_expr =
                 additive_expr
                 >> *(   (lit(".and.") > additive_expr       [op(op_and)])
                     |   (lit(".or.")  > additive_expr       [op(op_or)])
                     )
                 ;

             additive_expr =
                 multiplicative_expr
                 >> *(   ('+' > multiplicative_expr  [op(op_add)])
                     |   ('-' > multiplicative_expr  [op(op_sub)])
                     )
                 ;

             multiplicative_expr =
                 unary_expr
                 >> *(   ('*' > unary_expr           [op(op_mul)])
                     |   ('/' > unary_expr           [op(op_div)])
                     )
                 ;

             unary_expr =
                     primary_expr
                 |   ('!' > primary_expr             [op(op_not)])
                 |   ('-' > primary_expr             [op(op_neg)])
                 |   ('+' > primary_expr)
                 ;

             primary_expr =
                 double_                             [op(op_double , _1)]
                 |   variable                        [op(op_add_var, _1)]
                 |   lit("true")                     [op(op_true)]
                 |   lit("false")                    [op(op_false)]
                 |   ( '(' > expression > ')' )
                 ;



             symbol_expr =
             term                                [ op(op_add,_1) ]
                 >> *(   ('+' >> term            [ op(op_add) ])
                     |   ('-' >> term            [ op(op_sub) ])
                     )
                 ;

             term =
                 factor                          [ op(op_add,_1)]
                 >> *(   ('*' >> factor          [ op(op_mul) ])
                     |   ('/' >> factor          [ op(op_div) ])
                     )
                 ;

             factor =
                 +double_                        [ op(op_add) ]
                 |  '('   >> symbol_expr         [ op(op_add) ] >> ')'
                 |   ('-' >> factor              [ op(op_neg) ] )
                 |   ('+' >> factor              [ op(op_add) ] )
                 ;


             variable = symbol_alpha
                [
                    _val = _1
                ]
                ;


             symbol_def_expr %=
                (symbol_alpha >> qi::lit('=') >> symbol_expr)
                ;

             symsbols %=
                   symbol_def_parameter  |
                   symbol_def_local |
                   symbol_def_if |
                   symbol_def_class |
                   symbol_def_expr
                  ;

             symbol_class     = lexeme[no_case["class"]];
             symbol_of        = lexeme[no_case["of"]];
             symbol_endclass  = lexeme[no_case["endclass"]];
             symbol_parameter = lexeme[no_case["parameter"]];
             symbol_local     = lexeme[no_case["local"]];
             symbol_if        = lexeme[no_case["if"]];
             symbol_else      = lexeme[no_case["else"]];
             symbol_endif     = lexeme[no_case["endif"]];

             symbol_def_parameter %=
                  (symbol_parameter >> (symbol_alpha % ','))
                  ;

             symbol_def_local %=
                  (symbol_local >> (symbol_alpha % ','))
                  ;

             symbol_def_class %= symbol_def_class_inner;
             symbol_def_class_inner %=
                    symbol_class                    
                 >  symbol_alpha
                 >  symbol_of
                 >  symbol_alpha >> *(
                    symbol_def_stmts ) > symbol_endclass;

             symbol_def_if %= symbol_def_if_inner;
             symbol_def_if_inner %=
                    symbol_if > '(' > expression > ')' >>
                 * (symbol_def_stmts | symbol_else)
                 >  symbol_endif;


             symbol_def_stmts %=
                  (symbol_def_expr ) |
                  (symbol_def_class) |
                  (symbol_def_if   ) 
                  ;

             symbol_space =
                 +(qi::char_(" \t\n\r") | eol | eoi)
                 ;

             symbol_alpha %=
                  qi::char_("a-zA-Z_") >>
                 *qi::char_("a-zA-Z0-9_")
                  [
                     _val = _1
                  ]
                 ;

             symbol_digit =
                 +(qi::digit)
                 ;

             qi::on_error<fail>( start, client::error_handler(_4, _3, _2) );


             BOOST_SPIRIT_DEBUG_NODE(start);
             BOOST_SPIRIT_DEBUG_NODE(symsbols);
             BOOST_SPIRIT_DEBUG_NODE(symbol_of);
             BOOST_SPIRIT_DEBUG_NODE(symbol_endclass);
             BOOST_SPIRIT_DEBUG_NODE(symbol_class);
             BOOST_SPIRIT_DEBUG_NODE(symbol_space);
             BOOST_SPIRIT_DEBUG_NODE(symbol_alpha);
             BOOST_SPIRIT_DEBUG_NODE(symbol_digit);
             BOOST_SPIRIT_DEBUG_NODE(symbol_ident);

             BOOST_SPIRIT_DEBUG_NODE(symbol_parameter);
             BOOST_SPIRIT_DEBUG_NODE(symbol_def_parameter);
             BOOST_SPIRIT_DEBUG_NODE(symbol_def_stmts);
             BOOST_SPIRIT_DEBUG_NODE(symbol_def_local);
             BOOST_SPIRIT_DEBUG_NODE(symbol_def_class);
             BOOST_SPIRIT_DEBUG_NODE(symbol_def_class_inner);


             BOOST_SPIRIT_DEBUG_NODE(expression);
             BOOST_SPIRIT_DEBUG_NODE(term);
             BOOST_SPIRIT_DEBUG_NODE(factor);

             BOOST_SPIRIT_DEBUG_NODE(symbol_if);
             BOOST_SPIRIT_DEBUG_NODE(symbol_else);
             BOOST_SPIRIT_DEBUG_NODE(symbol_endif);

             BOOST_SPIRIT_DEBUG_NODE(symbol_local);
         }

         qi::rule<Iterator, Skipper> assignment_rhs;
         qi::rule<Iterator, Skipper>
                 equality_expr, relational_expr
               , logical_expr, additive_expr, multiplicative_expr
               , unary_expr, primary_expr, variable,  symbol_expr
               ;

         boost::phoenix::function<compile_op> op;

         qi::rule<Iterator, std::string()>
         symbol_alpha,
         symbol_ident;

         qi::rule<Iterator, Skipper>
         symsbols,
         symbol_local,
         symbol_if,
         symbol_else,
         symbol_endif,
         symbol_digit,
         symbol_space,
         symbol_class,
         symbol_endclass,
         symbol_of,
         symbol_parameter,


         symbol_def_expr,
         symbol_def_parameter,
         symbol_def_if,
         symbol_def_if_inner,
         symbol_def_stmts,
         symbol_def_stmts_rep,
         symbol_def_local,
         symbol_def_class_inner,
         symbol_def_class;

         qi::rule<Iterator, Skipper> expression, term, factor;
     };
}

bool my_parser(std::string const str)
{
     typedef std::string::const_iterator iterator_t;

     typedef client::dbase_grammar <iterator_t> grammar;
     typedef client::dbase_skipper <iterator_t> skipper;

     grammar pg;
     skipper skp;

     iterator_t iter = str.begin();
     iterator_t end  = str.end();

     bool r = phrase_parse(iter, end, pg, skp);
     if (r == true) {
         #ifdef USE_QT
         QMessageBox::information(0,"Parser", "Parsing SUCCESS.");
         #else
         std::cout << "SUCCESS" << std::endl;
         #endif
         return true;
     }

     if (iter != end) {
         std::stringstream ss;
         ss << "Parsing ERROR" << std::endl
            << "Remaining: '"
            << std::string(iter, end)
            << std::endl;

         #ifdef USE_QT
         QMessageBox::information(0,"Parser", ss.str().c_str());
         #else
         std::cout << "ERROR" << std::endl;
         #endif
         return false;
     }
    return false;
}

bool parseText(std::string const s, int m)
{
    cout << "starter on" << endl;
    client::code.clear();
    client::code.resize(10);

    cout << "starter onpopo: \n" << s << endl;
    bool r = my_parser(s);
    if (!r) cout << "errrrroorrr\n";
    else    cout << "tttttrrr\n";
    return r;
}
