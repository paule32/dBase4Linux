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

#include <QMessageBox>

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


	struct error
	{
		template <typename A, typename B = unused_type, typename C = unused_type>
		struct result { typedef void type; };

		error() { }
		void operator()(const std::string &msg) const {
			_pass = false;
			throw msg;
		}
		void operator()(const int &val) const {
			static int if_counter = 0;

			if (val == 10) { ++if_counter; QMessageBox::information(0,"ssssss", "info111"); }
			if (val == 11) { --if_counter; QMessageBox::information(0,"ssssss", "info122"); }

			if (if_counter < 0)
			throw std::string("missing ENDIF");

			if (val < 10)			
			throw int(val);
		}
	};

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

        void operator()(byte_code a, byte_code b, byte_code c)
        {
			std::cout << "ifffererer" << std::endl;
//             code.push_back(a);
//             code.push_back(b);
//             code.push_back(c);
         }
     };


	template <typename Iterator, typename Skipper = dbase_skipper<Iterator>>
	struct dbase_keyword : public qi::grammar<Iterator, Skipper>
	{
											// keyword - help-id
		struct keywordSymbols : qi::symbols<std::string, unsigned int>
		{
			keywordSymbols()
			{
				add
				(std::string("if"), 		1)
				(std::string("of"), 		2)
				(std::string("else"),		3)
				(std::string("class"),		4)
				(std::string("endif"),		5)
				(std::string("local"),		6)
				(std::string("return"), 	7)
				(std::string("endclass"),	8)
				(std::string("function"),	9)
				(std::string("parameter"),	10)
				(std::string("procedure"),	11)
				;
			}
		}
		my_keywords;

		dbase_keyword() : dbase_keyword::base_type(start)
		{
			using qi::_1;
        	using ascii::char_;
        	using phoenix::val;

        	start %=
				*( keyword  [cout << val("Keyword as a number: ") << _1 << endl]
                 | invalid  [cout << val("Invalid keyword: ")     << _1 << endl]
                 )
				 ;

			keyword = my_keywords >> !(char_("a-zA-Z0-9_"));
			invalid = +ascii::graph;
		}

		qi::rule<Iterator, Skipper> start;
		qi::rule<Iterator, int()> keyword;
		qi::rule<Iterator, std::string()> invalid;
	};

	template <typename Iterator, typename KeyID = dbase_keyword<Iterator>, typename Skipper = dbase_skipper<Iterator>>
	struct dbase_grammar : public qi::grammar<Iterator, Skipper>
	{
        qi::rule<Iterator, Skipper> start;
        dbase_grammar()  : dbase_grammar::base_type(start)
        {
            using boost::spirit::ascii::no_case;

            using qi::lit;
            using qi::char_;
            using qi::lexeme;

            using qi::on_error;
            using qi::fail;

            start %= * symsbols;

			symsbols %=
			(
				(symbol_def_if) 		|
				(symbol_def_expr)
			)
			;


			qualified_id %= symbol_alpha >> *('.' > symbol_alpha);
			variable     %= qualified_id;

			symbol_expr %=
			(
				(
					eoi > eps[my_error("Syntaxer !!Error!!!")]
				)
				|
				(
					symbol_new > variable > '('
					>> *(symbol_expr)
					> ')'
				)
				|
				((variable | int_ | double_)
				>> *(
						(
							lit('(') >> eoi
							> eps[ my_error("SYntax Error!!") ]
						)
						|
						(
							lit("=") > symbol_expr
						)
						|
						(
							(lit("+") | lit("-") | lit("*") | lit("/"))
							> symbol_expr
						)
					)
				)
			)
			;

			symbol_expr2expr =
			(
				(
					(
						(
							(variable
							>> *(
									(
										(lit("(") >> *(expression) > lit(")"))
										>> *(
											(lit("+") | lit("-") | lit("*") | lit("/"))
											> expression
										)
									)
									|
									(
										(lit("+") | lit("-") | lit("*") | lit("/"))
										> expression
									)
									|
									(
										(	lit("==") | lit("<=") |
											lit(">=") | lit("=>") |
											lit("=<") | lit("!=") |
											lit("<" ) | lit(">" )
										)
										>	expression
									)
								)
							)
						)
						|
						(
							(int_ | double_
							>> *(
									(
										(lit("+") | lit("-") | lit("*") | lit("/"))
										> expression
									)
									|
									(
										(	lit("==") | lit("<=") |
											lit(">=") | lit("=>") |
											lit("=<") | lit("!=") |
											lit("<" ) | lit(">" )
										)
										>	expression
									)
								)
							)
						)
					)
				)
			)
			;

			expression =
			(
				(
					lit("(") >> *(symbol_new) > (symbol_expr2expr) > lit(")")
				)
				|
				(
					*  symbol_new
					> (symbol_expr2expr)
				)
			)
			;

			symbol_def_if %=
			(
				((symbol_if > expression)
				>>	 (
					   *(symbol_def_if)
					 | *(symbol_def_expr)
					 )
				>>	*(   (symbol_else)
					>>	 (
						   *(symbol_def_if)
						 | *(symbol_def_expr)
						 )
					)
				)
				>	symbol_endif
			)
			;
 
			symbol_def_expr %=
			(
				(variable | int_ | double_)
				> lit('=')
				> symbol_expr
			)
			;

			symbol_alpha %=
				  qi::char_("a-zA-Z_") >>
				*(qi::char_("a-zA-Z0-9_"))
				;

			dont_handle_keywords =
			(	symbol_if
			|	symbol_endif
			|	symbol_of
			|	symbol_new
			|	symbol_else
			|	symbol_class
			|	symbol_endclass
			|	symbol_local
			|	symbol_return
			|	symbol_function
			|	symbol_procedure
			|	symbol_parameter
			)
			;

			symbol_if			.name("IF");
			symbol_endif		.name("ENDIF");
			symbol_of			.name("OF");
			symbol_new			.name("NEW");
			symbol_else 		.name("ELSE");
			symbol_class		.name("CLASS");
			symbol_endclass 	.name("ENDCLASS");
			symbol_local		.name("LOCAL");
			symbol_return		.name("RETURN");
			symbol_function 	.name("FUNCTION");
			symbol_procedure	.name("PROCEDURE");
			symbol_parameter	.name("PARAMETER");

            symbol_if        = lexeme[no_case["if"]];
            symbol_of        = lexeme[no_case["of"]];
			symbol_new       = lexeme[no_case["new"]];
            symbol_else      = lexeme[no_case["else"]];
            symbol_class     = lexeme[no_case["class"]];
            symbol_endif     = lexeme[no_case["endif"]];
            symbol_local     = lexeme[no_case["local"]];
			symbol_return    = lexeme[no_case["return"]];
            symbol_endclass  = lexeme[no_case["endclass"]];
			symbol_function  = lexeme[no_case["function"]];
            symbol_parameter = lexeme[no_case["parameter"]];
			symbol_procedure = lexeme[no_case["procedure"]];

			qi::on_error<fail>( start, client::error_handler(_4, _3, _2) );
		}

        qi::rule<Iterator, Skipper> assignment_rhs;
        qi::rule<Iterator, Skipper>
			variable,
			symbol_expr,
			symbol_expr2expr,
			qualified_id
        ;

         boost::phoenix::function<compile_op> op;
         boost::phoenix::function<error     > my_error;

         qi::rule<Iterator, std::string()>
         symbol_alpha,
         symbol_ident;

         qi::rule<Iterator, Skipper>
         symsbols, dont_handle_keywords,
         symbol_local,
         symbol_if,
         symbol_else,
         symbol_endif,
         symbol_digit,
         symbol_space,
         symbol_class,
         symbol_endclass,
         symbol_of,
		 symbol_new,
         symbol_parameter, symbol_def_string,
		 symbol_procedure, symbol_function, symbol_proc_stmts, symbol_return,

         symbol_def_expr,
         symbol_def_parameter,
		 symbol_def_procedure, symbol_def_function, symbol_def_return,
         symbol_def_if,
         symbol_def_if_inner,
         symbol_def_stmts,
         symbol_def_stmts_rep,
         symbol_def_local,
         symbol_def_class_inner,
         symbol_def_class;

         qi::rule<Iterator, Skipper> expression, term, factor;

         qi::rule<Iterator, std::string(), Skipper, qi::locals<char> > quoted_string, any_string;
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
	return r ;
}

bool parseText(std::string const s, int m)
{
    client::code.clear();
    client::code.resize(10);

	bool r = false;
	try {
		r = my_parser(s);
		if (r) {
			QMessageBox::information(0,"Parser", "Parsing SUCCESS.");
			return true;
		}	return false;
	}
	catch (int &e) {
		if (e == 1) {
			 QMessageBox::information(0,"Parser", "Parsing SUCCESS.");
			 return true ;
		}
		else if (e == 7) {
			 QMessageBox::information(0,"Parser", "Syntax Error!");
			 return true;
		}	 return false;
	}
	catch (std::string &e) {
		std::stringstream ss; ss
		<< "Parsing ERROR"
		<< std::endl
		<< e
		<< std::endl;
		QMessageBox::information(0,"Parser", ss.str().c_str());
	}
	catch (...) {
		QMessageBox::information(0,"Parser", "Parsing ERROR\nunknown.");
	}
    return r;
}
