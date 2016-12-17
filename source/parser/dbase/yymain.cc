#define BOOST_SPIRIT_DEBUG
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

	std::string st_name1;
	std::string st_name2;

	bool my_not_error = false;

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

		 op_new_class,
		 op_is_bool,

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
			my_not_error = false;
		}
    };
	boost::phoenix::function<client::error_handler_> const error_handler = client::error_handler_();


	struct error_handler_skip_
	{
         template <typename, typename, typename>
         struct result { typedef void type; };

         template <typename Iterator>
         void operator()(
             info const& what
           , Iterator err_pos, Iterator last) const
	{
			my_not_error = false;

             std::stringstream ss;
             ss  << "Error! Expectingkkklkl "
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
     boost::phoenix::function<client::error_handler_skip_> const error_handler_skip = client::error_handler_skip_();
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

             my_skip =  (char_("[ \t\n\r]"))                        |
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

			 qi::on_error<fail>(my_skip, client::error_handler_skip(_4, _3, _2) );

             BOOST_SPIRIT_DEBUG_NODE((my_skip));
        }
    	qi::rule<Iterator> my_skip;
    };

	// ------------------------------------
	// our class for storing stacked data;
	// pre-process, later as reference for
	// the DSL ...
	// ------------------------------------
	class my_value {		// BigInt wrapper
	public:
		my_value() { }

		double  isDouble;	// as double value
		QString isString;	// as BigInt string

		void setTag(int i)	{ tag = i; }
	private:
		int tag;			// tag for differ the type
	};

	class my_class {
	public:
		my_class() { }

		QString cname;		// class name
		QString pname;		// class parent name

		void setTag(int i)	{ tag = i; }	
	private:
		int tag;			// tag for difer type
	};

	class my_bool {
	public:
		my_bool() { }

		QString cname;		// name of var.
		bool    status; 	// is false/true ?
	private:
		int tag;
	};

	class my_ops {
	public:
        byte_code op_code;	// type of opcode
        std::string name;	// name of variable

		QVector<my_class> isClass;		// class ptr, if stacked
		QVector<my_value> isValue;		// class for numeric
		bool			isBoolean;		// class is boolean?

		void setTag(int i) { tag = i; }
	private:
		int tag;			// tag for differ type
    };

	// ---------------------------
	// the "code" holder ...
	// ---------------------------
    QVector<my_ops>  *      code;
	QVector<bool>    global_bool;


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

		void operator()(std::string &n1, std::string &s1) const
		{
			if (n1 == "op1") st_name1 = n1; else
			if (n1 == "op2") st_name2 = n1;
		}

        void operator()(const byte_code&, boost::spirit::unused_type& ut) const
        {
             cout << "unnnnn" << endl;
        }

		void operator()(std::string n1, int    value) const { std::cout << "integer" << endl; }
		void operator()(std::string n1, double value) const { std::cout << "doubles" << endl; }

		void operator()(std::string n1, bool bval) const {
			auto my_pspush = new my_ops;

			my_pspush->op_code   = op_is_bool;
			my_pspush->isBoolean = bval;

			code->append(*(my_pspush));
		}

        void operator()(const byte_code &a) const
		{
			// -------------------------
			// allocate memory ...
			// -------------------------
            auto my_pspush = new my_ops;
			auto my_psval  = new my_value;

			// --------------------
			// set some stuff ...
			// --------------------
            my_pspush->op_code = a;
            my_pspush->name    = "";

			my_psval->isDouble = 0.00;  	// default value = 0.00
			my_psval->setTag(0);			// default type: 0 = value

			// push structure onto vector ...
			my_pspush->setTag(1);
			my_pspush->isValue.append(*(my_psval));

            code->append(*(my_pspush));
		}

		compile_op(QVector<client::my_ops> *_code)
		{
			cout << "weiter00000" << endl;
			code = _code;
			cout << "weiter11111" << endl;
		}

		void operator()(const std::string n1) const {
			if (n1 == "op3") {
				my_not_error = true;

				auto my_pspush  = new my_ops;
				auto my_psclass = new my_class;

				my_psclass->cname = st_name1.c_str();
				my_psclass->pname = st_name2.c_str();

				my_pspush->isClass.append(*(my_psclass));
				code->append(*(my_pspush));
			}
		}
		void operator()(const std::string n1, const std::string n2) const {
			if (n1 == "op1") st_name1 = n2; else
			if (n1 == "op2") st_name2 = n2;

			// ----------------------
			// keyword: parameter ...
			// ----------------------
			else if (n1 == "op4") {
				std::string s1;
				int pos = 0;
				while (1)  {
					if (pos >= n2.size())
					break;
					s1 += n2[pos];
					pos += 2;
				}

				if (s1.size() < 1) {
					throw std::string("sting size < 1");
				}

				auto my_pspush  = new my_ops;
				my_pspush->name = s1.c_str();
				my_pspush->op_code = byte_code::op_is_bool;

				code->append(*(my_pspush));
			}
		}
		void operator()(
			const std::string ops,
			const std::string cname,
			const std::string pname) const
		{
			// -------------------------------------
			// convert all letters to lower case ...
			// -------------------------------------
			QString _ops = ops.c_str();
		}

	};

	template <typename Iterator, typename Skipper = dbase_skipper<Iterator>>
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
				(symbol_def_parameter)	|
				(symbol_def_local)		|
				(symbol_def_expr)		|
				(symbol_def_if)			|
				(symbol_def_class)
			)
			;


			symbol_def_parameter =
			(
				symbol_parameter >
				(	(
						(
							(symbol_alpha
							[ _val = qi::_1, op("op4", qi::_1) ])
						)
						>> *(
							',' > (symbol_alpha
							[ _val = qi::_1, op("op4", qi::_1) ])
						)
					)
					|
					(
						symbol_alpha  >> *(symbol_alpha >
						eps[my_error("missong comma between variable") ])
					)
				)
			)
			;
			symbol_def_local =
			(
				symbol_local >
				(
					(
						(variable [ op("op4","qi::_1") ] )
						>> *(',' > variable)
					)
					|
					(
						variable  >> *(variable) >
						eps[my_error("missong comma between variable") ]
					)
				)
			)
			;

			symbol_alpha %= (
				 (qi::char_("a-zA-Z")     
				[
					qi::_val = phoenix::construct<std::string>(""),
					qi::_val = val(qi::_1)
				] ) >>
				*(qi::char_("a-zA-Z0-9_") [ qi::_val = qi::_val + val(qi::_1) ])
			)
			;

			qualified_id = symbol_alpha >> *('.' > symbol_alpha);
			variable     = qualified_id [_val = qi::_1];


			symbol_expr %=
			(
				(
					eoi > eps[my_error("Syntaxer !!Error!!!")]
				)
				|
				(
					lit("[") >> (eol | eoi) > eps[my_error("funkel bunkel")]
				)
				|
				(
					(symbol_true | symbol_false)
					[
						_val = qi::_1
					]
				)
				|
				(
					symbol_string
					>> *(
						(lit("+") | lit("-"))
						> symbol_expr
					)
				)
				|
				(
					lit("(") >> *symbol_expr > lit(")")
					>> *(
							(lit("+") | lit("-") | lit("*") | lit("/"))
							> symbol_expr
					)
				)
				|
				(
					symbol_new > variable
					>> *(
						(
							lit("(") >> *(symbol_expr) > lit(")")
						)
						|
						(
							(
								lit("+") | lit("-") |
								lit("*") | lit("/")
							)
							>	symbol_expr
						)
					)
				)
				|
				(
					variable
					>> *(
						(
							lit("(") >> *symbol_expr > lit(")")
						)
						|
						(
							symbol_string
							>> *(
								(lit("+") | lit("-"))
								> symbol_expr
							)
						)
						|
						(
							(lit("+") | lit("-") | lit("*") | lit("/"))
							> symbol_expr
						)
					)
				)
				|
				(
					(int_ | double_)
					>> *(
						(lit("+") | lit("-") | lit("*") | lit("/"))
						> symbol_expr
					)
				)
			)
			;

			symbol_expr2expr %=
			(
				(
					(
						((lit("+") | lit("-") | lit("*") | lit("/"))
						> 
						(variable | int_ | double_))
					)
					|
					(
						(symbol_true | symbol_false) [
							_val = qi::_1,
							op("op6", phoenix::construct<bool>(qi::_1))
						]
					)
					|
					(
						(variable | int_ | double_)
						>> (
							(lit("+") | lit("-") | lit("*") | lit("/"))
							> symbol_expr2expr
						)
					)
					|
					(
						conditions >> (expression)
					)
				)
			)
			;

			conditions %=
			(	lit("==") | lit("<=") |
				lit(">=") | lit("=>") |
				lit("=<") | lit("!=") |
				lit("<" ) | lit(">" )
			)
			;

			expression %=
			(
				(
					(variable >> *(symbol_expr2expr)) |
					(((int_) [
						_val = qi::_1,
						op("op5",val(qi::_1))
					] ) >> *(symbol_expr2expr))
				)
				|	(
					(symbol_new > variable)    >> (
					(conditions > symbol_expr2expr))
				)
			)
			;

			symbol_def_if %=
			(
				(
					(symbol_if >> (lit("(")) > expression >> (lit(")")))
					>> 	*(		symsbols)
					>> 	*(	(symbol_else)
			    	>> 	*(		symsbols) )
					>	symbol_endif
				)
			)
			;

            any_stringSB =
			(
				(
					lexeme[
						(
							lit("'")
							>> *(
								((lit("\\") >> char_) | (char_ - lit("'") ))
							)
							> lit("'")
						)
					]
				)
				|
				(
					lexeme[
						(
							char_("\"")
							>> *(
								((lit('\\') >> char_) | (char_ - lit("\"") ))
							)
							> lit("\"")
						)
					]
				)
				|
				(
					lexeme[
						(
							char_("[")
							>> *(
								(
									( char_ - char_("]")   )
									|
									( char_("\\") >> char_ )
								)
							)
							> char_("]")
						)
					]
				)
				|
				(
					char_("[") > eps[my_error("Array Error")]
				)
			)
			;

			symbol_def_class =
			(
				symbol_class > (symbol_alpha [ op("op1",val(qi::_1)) ] ) >
				symbol_of	 > (symbol_alpha [ op("op2",val(qi::_1)) ] )
				[
					op("op3")
				]
			>> *(symsbols) >
				 symbol_endclass [ _val = 1 ]
			)
			;

			symbol_string    = (any_stringSB);
			symbol_def_expr %=
			(
				(variable - (dont_handle_keywords))
				>	lit("=")
				>	(symbol_expr)
			)
			;

			dont_handle_keywords =
			(	symbol_if
			|	symbol_endif
			|	symbol_false
			|	symbol_true
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

			any_SB.name("#string req#");
			any_stringSB.name(" bracket STRING bracket");

			symbol_if			.name("IF");
			symbol_endif		.name("ENDIF");
			symbol_of			.name("OF");
			symbol_new			.name("NEW");
			symbol_else 		.name("ELSE");
			symbol_true 		.name("TRUE");
			symbol_class		.name("CLASS");
			symbol_false		.name("FALSE");
			symbol_endclass 	.name("ENDCLASS");
			symbol_local		.name("LOCAL");
			symbol_return		.name("RETURN");
			symbol_function 	.name("FUNCTION");
			symbol_procedure	.name("PROCEDURE");
			symbol_parameter	.name("PARAMETER");

			conditions			.name("conditions expected");

			symbol_true 	 = ((lexeme[no_case["true" ]] | lexeme[no_case[".t."]]) [_val = true ] );
			symbol_false	 = ((lexeme[no_case["false"]] | lexeme[no_case[".f."]]) [_val = false] );

            symbol_if        =  lexeme[no_case["if"]];
            symbol_of        =  lexeme[no_case["of"]];
			symbol_new       =  lexeme[no_case["new"]];
            symbol_else      =  lexeme[no_case["else"]];
            symbol_class     =  lexeme[no_case["class"]];
            symbol_endif     =  lexeme[no_case["endif"]];
            symbol_local     =  lexeme[no_case["local"]];
			symbol_return    =  lexeme[no_case["return"]];
            symbol_endclass  =  lexeme[no_case["endclass"]];
			symbol_function  =  lexeme[no_case["function"]];
            symbol_parameter =  lexeme[no_case["parameter"]];
			symbol_procedure =  lexeme[no_case["procedure"]];

			qi::on_error<fail>( symsbols, client::error_handler(_4, _3, _2) );
		}

		qi::rule<Iterator, std::string()> any_SB;
		qi::rule<Iterator, std::string()> any_stringSB;

        qi::rule<Iterator, Skipper> assignment_rhs;
        qi::rule<Iterator, Skipper>
			variable,
			symbol_string,
			symbol_expr,
			symbol_expr2expr,
			qualified_id
        ;

         boost::phoenix::function<compile_op> op;
         boost::phoenix::function<error     > my_error;

         qi::rule<Iterator, std::string()>
         symbol_alpha,
         symbol_ident;

		qi::rule<Iterator, bool> symbol_false, symbol_true;

        qi::rule<Iterator, Skipper>
         symsbols, dont_handle_keywords, conditions,
         symbol_local,
         symbol_if, is_function,
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
         qi::rule<Iterator, Skipper> quoted_string, any_string;
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

static void
show_error(void) {
	QMessageBox::critical(0,
	"Internal Error",
	"could not convert object!");
}

// -----------------------------------------------
// now, it is time to interpret the collection ...
// -----------------------------------------------
bool dbase_interpret()
{
	bool bool_last_value = false;

	using namespace client;
	for(auto it  = std::begin(*(code));
			 it != std::end  (*(code)); ++it)
	{	try {
			if ( (it)->op_code == byte_code::op_is_bool) {
				bool flag = (const bool)global_bool.at(0);  // to-do!!
				bool_last_value = flag;
				(it)->isBoolean = bool_last_value;
			}
			//else if (my_prg->op_code == byte_code::op_new_class) {
			//	QMessageBox::information(0,"CLASS","a new classler");
			//}
		}	catch (...) { show_error(); }
	}
	return true;
}

bool parseText(std::string const s, int m)
{
	using namespace client;
	code        = new QVector<my_ops >(10);
	global_bool << true << false << true; // fixme: !!!

	bool r = false;
	try {
		r = my_parser(s);
		if (r) {
			if (client::my_not_error == false) {
				QMessageBox::information(0,"Parser", "Parsing SUCCESS..");
				dbase_interpret();
			}
			else {
				QMessageBox::information(0,"Parser", "Syntax Error!");
			}

			delete client::code; return true ;
		}	delete client::code; return false;
	}
	catch (int &e) {
		if (e == 1) {
			 QMessageBox::information(0,"Parser", "Parsing SUCCESS!!!!!.");
			 dbase_interpret();

			 delete client::code;
			 return true;
		}
		else if (e == 7) {
			 QMessageBox::information(0,"Parser", "Syntax Error!");

			 delete client::code; return true ;
		}	 delete client::code; return false;
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
	delete client::code;
    return r;
}

