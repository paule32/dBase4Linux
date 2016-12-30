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

#include <stdio.h>
#include <string.h>

#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include <QMessageBox>
#include <QString>
#include <QVariant>
#include <QMutex>

#include <qstring.h>

#include <QDebug>

#define MsgBox(t,txt)  QMessageBox::information(0,t,txt)
#define phxNew(t,o)    (o)

using namespace std;

using boost::phoenix::function;
using boost::phoenix::ref;
using boost::phoenix::size;

using namespace boost::spirit;
using namespace boost::spirit::qi;

namespace boost { namespace spirit { namespace traits
{
    template <> struct is_container<QString> : mpl::true_ {};
    template <> struct container_value<QString> : mpl::identity<QChar> {};
    template <>
    struct push_back_container<QString, QChar>
    {
        static bool call(QString& c, QChar const& val)
        {
            c.append(val);
            return true;
        }
    };
}}}

namespace client
{
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

	QString st_name1;
	QString st_name2;
	QString last;

	QMutex mutex;

	bool my_not_error = true;

    enum byte_code
    {
		 op_birth,
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
		 op_is_ident,
		 op_is_number,
		 op_is_arith,

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

	// ----------------------------------------------
	// little struct, that handles the new-lines ...
	// ----------------------------------------------
	int line_no = 0;
	struct
	line_no_struct
    {
	   	template <typename T>
        void operator()(T const &t) const
        {
			if (QString(t) > 0)
			{
				mutex.lock();
				const QChar c = QString(t).toStdString().c_str()[0];
				if (c == '\n' || c == '\r') {
					++line_no;
				}
				mutex.unlock();
			}
        }
	};
	// ------------------------------------------------
	// custom skipper ...
	// for comments
	// ------------------------------------------------
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

			my_skip =  (char_("[ \t\n\r]") [ _val = qi::_1, line_func(qi::_1) ] )     |
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

		char c;
		rule<Iterator> my_skip;
		phoenix::function<line_no_struct> line_func;
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
        QString   name   ;	// name of variable

		QVector<my_class> isClass;		// class ptr, if stacked
		QVariant          isValue;		// class for numeric

		void setTag(int i) { tag = i; }
	private:
		int tag;			// tag for differ type
    };

	class my_callstack {
	};

	// -----------------------------
	// the "code" holder ...
	// -----------------------------
    QVector<my_ops*>           code;
	QVector<my_callstack>  callfunc;

	QVector<bool>       global_bool;


	int srcLine = 0;		// internal line
    QString last_token;


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
	   	template <typename T1, typename T2, typename T3 = byte_code>
        void operator()(T1 const &t1, T2 const &t2, T3 const &t3 = byte_code::op_birth) const
		{
			QString TA = typeid(T1).name(); 
			QString TB = typeid(T2).name();

			QString s1;
			
			auto my_tmp = new my_ops;
//			MsgBox("testung",QString("%1\n%2\n%3\n%4").arg(TA).arg(TB).arg(t1).arg(t2));

			if (t3 > 0)
			{
MsgBox("lupter",QString("%1 : %2").arg(t1).arg(t2));

				if (QString(t1) == "opm+") { my_tmp->op_code = byte_code::op_add; } else
				if (QString(t1) == "opm-") { my_tmp->op_code = byte_code::op_sub; } else
				if (QString(t1) == "opm*") { my_tmp->op_code = byte_code::op_mul; } else
				if (QString(t1) == "opm/") { my_tmp->op_code = byte_code::op_div; }

				my_tmp->name = "arith";
				code.append(my_tmp);
				return;
			}
			if ((TA == "PKc")
			&& ( TB == "b"))
			{
				int vb = QString("%1").arg(t2).toInt();
				QVariant var = vb;

				my_tmp->op_code = byte_code::op_is_bool;
				my_tmp->isValue = var;
				my_tmp->name    = "bool";
				code.append(my_tmp);
			}   else
			if ((TA == "PKc")
			&& ( TB == "i"))
			{
				my_tmp->op_code = byte_code::op_is_number;
				my_tmp->isValue = QString("%1").arg(t2).toInt();
				my_tmp->name    = "int";
				code.append(my_tmp);
			}   else
			if ((TA == "PKc")
			&& ( TB == "7QString"))
			{
				int pos = 0;
				while (1) {
					s1  += QString(t2)[pos];
					pos += 2;
					if (pos > QString(t2).size())
					break;
				}

				my_tmp->op_code = byte_code::op_is_ident;
				my_tmp->isValue = s1;
				my_tmp->name    = s1;

				code.append(my_tmp);
			}
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
							//[ _val = qi::_1, op("op4", qi::_1) ]
							)
						)
						>> *(
							',' > (symbol_alpha
							//[ _val = qi::_1, op("op4", qi::_1) ]
							)
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
						(variable  /*[ op("op4","qi::_1") ]*/ )
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

			symbol_alpha %=
			(
				(symbol_alpha_alone
				[
					_val   = qi::_1,
					op("op7",qi::_1)
				]	)
			);
			symbol_alpha_alone %= (
				(qi::char_("a-zA-Z")     
				[ _val =        val(qi::_1) ] ) >> *(qi::char_("a-zA-Z0-9_")
				[ _val = _val + val(qi::_1) ] )
			)
			;

			qualified_id = ((symbol_alpha
			[_val = qi::_1] )) >> *('.'  > (symbol_alpha
			[_val = qi::_1] ));

			variable = (qualified_id
			[	_val =   qi::_1,
				op("op7",qi::_1) ]
			)	;

			math_add = char_('+') [ op("opm+",qi::_1,byte_code::op_add) ];
			math_sub = char_('-') [ op("opm-",qi::_1,byte_code::op_sub) ];
			math_mul = char_('*') [ op("opm*",qi::_1,byte_code::op_mul) ];
			math_div = char_('/') [ op("opm/",qi::_1,byte_code::op_div) ];

			symbol_term %=
				term						[ _val  = qi::_1 ]
				>> *( (math_add >> term 	[ _val += qi::_1 ] )
					| (math_sub >> term 	[ _val -= qi::_1 ] )
					)
				;
			term =
				factor						[ _val  = qi::_1 ]
				>> *( (math_mul >> factor	[ _val *= qi::_1 ] )
					| (math_div >> factor	[ _val /= qi::_1 ] )
					)
				;
			factor =
				(
					int_
					[
						_val  = qi::_1,
						op("opm",qi::_1,
						byte_code::op_is_number)
					]
				)
				|
				( '(' >> symbol_term		[ _val = qi::_1 ] >> ')' )	|
				( math_add >> factor 		[ _val = qi::_1 ] )			|
				( math_sub >> factor 		[ _val = qi::_1 ] )
				;

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
					((symbol_true | symbol_false)
					[
						_val    = qi::_1,
						op("op3", qi::_1)
					])
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
						//	(lit("+") | lit("-") | lit("*") | lit("/"))
						//	>
							symbol_term
						)
					)
				)
				|
				(
					symbol_term
					[
						_val   = qi::_1,
						op("opz",qi::_1,1)
					]
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
						(symbol_true | symbol_false)
						//[
							//_val = qi::_1,
							//op("op6", qi::_1)
						//]
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
						_val =   qi::_1,
						op("op1",qi::_1)
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
				symbol_class > (symbol_alpha /*[ op("op1",val(qi::_1)) ]*/ ) >
				symbol_of	 > (symbol_alpha /*[ op("op2",val(qi::_1)) ]*/ )
		/*		[
					op("op3")
				]*/
			>> *(symsbols) >
				 symbol_endclass [ _val = 1 ]
			)
			;

			symbol_string   = (any_stringSB);
			symbol_def_expr =
			(
				((variable
				[
					_val   = qi::_1,
					op("op7",qi::_1)
				])
					- (dont_handle_keywords))
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

		qi::rule<Iterator, QString(), Skipper> any_SB;
		qi::rule<Iterator, QString(), Skipper> any_stringSB;

        qi::rule<Iterator, Skipper> assignment_rhs;
        qi::rule<Iterator, Skipper>
			symbol_string,
			symbol_expr,
			symbol_expr2expr
        ;

        boost::phoenix::function<compile_op> op;
        boost::phoenix::function<error     > my_error;

		boost::phoenix::function<line_no_struct> line_func;

        qi::rule<Iterator, QString(), Skipper>
		variable,
		qualified_id,
        symbol_alpha, symbol_alpha_alone,
        symbol_ident;

		qi::rule<Iterator, bool, Skipper> symbol_false, symbol_true;
		qi::rule<Iterator,  int, Skipper> symbol_term, term, factor;
		qi::rule<Iterator,  int, Skipper> math_add, math_sub, math_mul, math_div;

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

         qi::rule<Iterator, Skipper> expression;
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

// -----------------------------------------------
// now, it is time to interpret the collection ...
// -----------------------------------------------
bool dbase_interpret()
{
	using namespace client;
	int i,a;
	class my_ops *mptr;

	static int lval = 0;

	QVariant  last_val;
	byte_code last_op = byte_code::op_birth;

	std::reverse(code.begin(), code.end());
	for (i = 0; i < code.count(); ++i)
	{
		mptr = code[i];
		if (mptr == NULL)
		throw QString("nullptr in code vector.");

		switch (mptr->op_code)
		{
			case byte_code::op_add: { MsgBox("last_op","adder"); last_op = byte_code::op_add; continue; } break;
			case byte_code::op_sub: { last_op = byte_code::op_sub; continue; } break;
			case byte_code::op_mul: { last_op = byte_code::op_mul; continue; } break;
			case byte_code::op_div: { last_op = byte_code::op_div; continue; } break;

			case byte_code::op_is_bool:
			{
				MsgBox("isbool",
				QString("----> %1").arg(mptr->isValue.toInt()));
				last_op  = byte_code::op_is_bool;

				if (mptr->isValue == "0")
				last_val = 0; else
				last_val = 1;
			}	break;
			case byte_code::op_is_number:
			{
				QMessageBox::information(0,"isnumber",
				QString("----> %1").arg(mptr->isValue.toInt()));

				int nval = mptr->isValue.toInt();

				if (last_op == byte_code::op_add) lval += nval; else
				if (last_op == byte_code::op_sub) lval -= nval; else
				if (last_op == byte_code::op_div) lval /= nval; else
				if (last_op == byte_code::op_mul) lval *= nval;

				mptr->isValue = lval;
				last_val	  = lval;

				last_op 	  = byte_code::op_is_number;
			}	break;
			case byte_code::op_is_ident:
			{
				//if (last_val.type() == QVariant::Int)
				{
					mptr->isValue = last_val;
					QMessageBox::information(0,"isident",
					QString("--> %1 = %2")
					.arg(mptr->name)
					.arg(last_val.toInt()));
				}
			}	break;
		}
	}

	return true;
}

bool parseText(std::string const s, int m)
{
	using namespace client;
	line_no      = 1;
	global_bool << true << false << true; // fixme: !!!
	my_not_error = true;
	code.clear();

	bool r = false;
	try {
		r = my_parser(s);
		if (r) {
			if (my_not_error == true) {
				QString succ = QString("Parsing SUCCESS!\n\nLines: %1").arg((line_no/6)+1);
				QMessageBox::information(0,"Parser", succ);
				dbase_interpret();
				return true ;
			}
			else {
				QMessageBox::information(0,"Parser", "Syntax Error!");
			}
		}	return r;
	}
	catch (int &e) {
		if (e == 1) {
			 QMessageBox::information(0,"Parser", "Parsing SUCCESS!!!!!.");
			 dbase_interpret();

			 return true;
		}
		else if (e == 7) {
			 QMessageBox::information(0,"Parser", "Syntax Error!!");

			 return true ;
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
	catch (QString &e) {
		QMessageBox::information(0,"Exception",e);
		return false;
	}
	catch (std::bad_alloc &e) {
		QMessageBox::information(0,"Parser", "Parsing ERROR\nstd::bad_alloc");
		return false;
	}
	catch (...) {
		QMessageBox::information(0,"Parser", "Memory Error");
		return false;
	}
    return r;
}

