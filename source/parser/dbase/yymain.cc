#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_ACTIONS_ALLOW_ATTR_COMPAT

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include <math.h>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/lexical_cast.hpp>

#include "antifreeze.h"

#include <qstring.h>
#include "source/includes/editorgutter.h"

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
		op_is_assign_val,

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

    // ------------------------------------
    // our class for storing stacked data;
    // pre-process, later as reference for
    // the DSL ...
    // ------------------------------------
    class my_value {                // BigInt wrapper
    public:
        my_value() { }

        double  isDouble;       // as double value
        QString isString;       // as BigInt string

        void setTag(int i)      { tag = i; }
    private:
        int tag;                        // tag for differ the type
    };

    class my_class {
    public:
		my_class() { }

		QString name;          // class name
		QVariant parent;

		void setTag(int i)      { tag = i; }   
    private:
	    int tag;                        // tag for difer type
    };

    class my_bool {
    public:
        my_bool() { }

        QString cname;          // name of var.
        bool    status;         // is false/true ?
    private:
        int tag;
    };
 
    class my_ops {
    public:
		my_ops() {
			isInit = false;
		}
		byte_code op_code;      // type of opcode

		QString     name ;      // name of variable
        int         what ;
		QVariant    value;
		bool       isInit;

        void setTag(int i) { tag = i; }
    private:
        int tag;                        // tag for differ type
    };
 
    class my_callstack {
    };
 
    // -----------------------------
    // the "code" holder ...
    // -----------------------------
	QVector<my_ops>           code;
	QVector<my_ops>::iterator code_iterator;

	QVector<my_callstack> callfunc;
 
	QVector<bool>       global_bool;

	int math_flag = 0;
	QString last_math_op;

	QString _end_token;
	double  _end_result = 0.00;

	struct assign_expr_val  {
		assign_expr_val() { }
		void operator()(QString const &t1) const
		{
			bool found = false;
			_end_token = t1;

			if (code.isEmpty()) {
				my_ops ops;
				ops.name  = t1;
				ops.value = _end_result;
				code.append(ops);
			}

			for (code_iterator  = code.begin();
				 code_iterator != code.end();
				 code_iterator++) {

				 if (t1 == code_iterator->name)
				 {
					 _end_token  = t1;
					 _end_result = code_iterator->value.toDouble();
					 found       = true;
					 break;
				 }
			}

			if (found == false)
			{	my_ops ptr;
				ptr.name  = t1;
				ptr.value = _end_result;
				code.append(ptr);
				_end_result = ptr.value.toDouble();
			}
		}
	};
	phoenix::function<assign_expr_val> expr_assign_val;

	struct compile_op2
	{
		typedef double result_type;
		compile_op2() { }
		template<typename T1, typename T2>
		double operator()(T1 const &t1, T2 const &t2) const
		{
			QString s1 = typeid(T1).name();
			QString s2 = typeid(T2).name();

			QString n1 = QString(t1);

			double res = _end_result;
			bool found = false;

//			_end_result = res;
			_end_token  = n1;

			MsgBox("typsers",QString("--->%1<-->%2--->%3")
			.arg(s1)
			.arg(n1)
			.arg(res));

MsgBox("---www-----",
QString("oooooo1: %1\n2: %2").arg(n1).arg(res));

			if (QString("7QString") == s1)
			{
				if ((QString("d") == s2)
				||  (QString("i") == s2))
				{
					if (code.isEmpty()) {
						my_ops ops;
						ops.name  = n1;
						ops.value = res;
						code.append(ops);
					}

					for (code_iterator  = code.begin();
						 code_iterator != code.end();
						 code_iterator++) {

						 if (s1 == code_iterator->name)
						 {
							 found = true;
							 res   = code_iterator->value.toDouble();
							 _end_token = code_iterator->name;

							 MsgBox("opratortringggerreer",QString("%1>-->>> %2").arg(t1).arg(res));
							 break;
						 }
					}
				}
			}
			_end_result = res;
			return res;
		}
	};
	phoenix::function<compile_op2> op2;

	struct compile_op
    {
		template <typename T1, typename T2, typename T3>
        void operator()(T1 const &t1, T2 const &t2, T3 const &t3) const
        {
			const double  r = t2;
			
			if (QString(t1) == QString("math=")) { _val = _end_result  = r; } else

			if (QString(t1) == QString("math+")) { _val = _end_result += r; } else
			if (QString(t1) == QString("math-")) { _val = _end_result -= r; } else
			if (QString(t1) == QString("math*")) { _val = _end_result *= r; } else
			if (QString(t1) == QString("math/")) { _val = _end_result /= r; } else

			if (QString(t1) == QString("mathcos")) { _val = _end_result = ::cos(r); } else
			if (QString(t1) == QString("mathsin")) { _val = _end_result = ::sin(r); } else
			if (QString(t1) == QString("mathtan")) { _val = _end_result = ::tan(r); }
		}
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
				const QChar c = QString(t).toStdString().c_str()[0];
				if (c == '\n' || c == '\r') {
					++line_no;
				}
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

		dbase_skipper operator()(Iterator) {
		}

		char c;
		rule<Iterator> my_skip;
		phoenix::function<line_no_struct> line_func;
	};

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

	// ----------------------
	// math ...
	// ----------------------
	struct math_func_cos	{
		template <typename T>
		double operator()(T const &t) const {
			return ::cos(t);
		}
	};
	struct math_func_sin	{
		template <typename T>
		double operator()(T const &t) const {
			return ::sin(t);
		}
	};
	struct math_func_tan	{
		template <typename T>
		double operator()(T const &t) const {
			return ::tan(t);
		}
	};
	struct math_func_var	{
		math_func_var() { }
		double operator()(QString const &t1) const
		{
			bool found = false;
			double res = 0.00;

MsgBox("opratorr",QString("->%1 : %2>-->>> %3").arg(t1).arg(_end_token).arg(_end_result));

			if (code.isEmpty())
			{	my_ops ptr;
				ptr.name  = _end_token;
				ptr.value = _end_result;
				code << ptr;
			}
			
			for (code_iterator  = code.begin();
				 code_iterator != code.end();
				 code_iterator++) {

				 if (_end_token == code_iterator->name)
				 {
					 found = true;
					 res = code_iterator->value.toDouble();
					 MsgBox("oprator Qstringggerreer",QString("->%1>-->>> %2").arg(_end_token).arg(res));
					 break;
				 }
			}

			if (found == false)
			{
				res = code_iterator->value.toDouble();
			}


			MsgBox("opratorringggerreer",QString("->%1 : %2>-->>> %3").arg(t1).arg(_end_token).arg(res));
			return res;
		}
	};
	// math functions ...
	phoenix::function<math_func_cos> func_math_cos;
	phoenix::function<math_func_sin> func_math_sin;
	phoenix::function<math_func_tan> func_math_tan;
	phoenix::function<math_func_var> func_math_var;

	struct str_func_assign  {
		str_func_assign() { }
		void operator()(QString const &t1) const
		{
			QString s1;
			if (t1.isEmpty())
			s1 = _end_token;

			s1 = QString("-->%1<---->%2<-----")
			.arg(_end_result)
			.arg(s1);
			//MsgBox("operator _assign",s1.toLatin1());
		}
	};
	// string functions ...
	phoenix::function<str_func_assign> func_str_assign;

	// -------------------------------------
	// lhs and rhs values for conditions ...
	// -------------------------------------
	QString LHS_variable;
	QString RHS_variable;

	double  LHS_double;
	double	RHS_double;

	bool	exec_flag = true;

	int     VAR_condition;
	int     VAR_type1;
	int     VAR_type2;

	QString string_value;

	struct lhs_save_var_ {
		typedef void result_type;
		lhs_save_var_() { }
		void operator()(QString const &t1) const {
			LHS_variable = t1;
			VAR_type1    =  1;
		}
		void operator()(double const &t1) const {
			LHS_double = t1;
		}
	};
	struct rhs_save_var_ {
		typedef void result_type;
		rhs_save_var_() { }
		void operator()(QString const &t1) const {
			RHS_variable = t1;
			VAR_type2    =  2;
		}
	};
	struct lhs_save__ar_ {
		typedef void result_type;
		lhs_save__ar_() { }
		void operator()(double const &t1) const {
			LHS_double = t1;
			VAR_type1  =  3;
		}
	};
	struct rhs_save__ar_ {
		typedef void result_type;
		rhs_save__ar_() { }
		void operator()(double const &t1) const {
			RHS_double = t1;
			VAR_type2  =  4;
		}
	};
	struct var_cond_ {
		typedef void result_type;
		var_cond_() { }
		void operator()(int const &cond) const {
			VAR_condition = cond;
		}
	};
	struct chk_cond_ {
		typedef void result_type;
		chk_cond_() { }
		void operator()(QString const &t1, QString const &t2) const {
		}
		void operator()(QString const &t1, double const &t2) const {
		}
		void operator()(double const &t1, QString const &t2) const {
		}
		void operator()(double const &t1, double const &t2) const {
			if (VAR_condition == 1) {
				if (t1 == t2) {
					if (exec_flag == false)
					exec_flag = true;
				}	else {
					exec_flag = false;
				}
			}
		}
	};
	phoenix::function<var_cond_    > var_cond;
	phoenix::function<chk_cond_    > chk_cond;

	phoenix::function<lhs_save_var_> lhs_save_var;
	phoenix::function<rhs_save_var_> rhs_save_var;

	phoenix::function<lhs_save__ar_> lhs_save__ar;
	phoenix::function<rhs_save__ar_> rhs_save__ar;


	struct any_string_value_ {
		typedef QChar result_type;
		template <typename T>
		QString operator()(T const &t1) const {
			string_value.clear();
			string_value.append(QString(t1));
			return t1;
		}
	};
	boost::phoenix::function<any_string_value_> any_string_value;

	struct print_string_ {
		typedef QString result_type;
		template <typename T>
	    QString operator()(T const &t1) const {
			if (exec_flag) {
				MsgBox("PRINTERTEXT",string_value);
				string_value.clear();
			}
			return string_value;
		}
	};
	phoenix::function<print_string_> print_string;


	template <typename Iterator, typename FPT, typename Skipper = dbase_skipper<Iterator>>
	struct dbase_grammar : public qi::grammar<Iterator, FPT, Skipper>
	{
        qi::rule<Iterator, FPT, Skipper> start;
        dbase_grammar()  : dbase_grammar::base_type(start)
        {
			using boost::spirit::qi::real_parser;
		    using boost::spirit::qi::real_policies;
		    real_parser<FPT,real_policies<FPT> > real;

		    using boost::spirit::qi::_1;
		    using boost::spirit::qi::_2;
		    using boost::spirit::qi::_3;
		    using boost::spirit::qi::no_case;
		    using boost::spirit::qi::_val;

            using qi::lit;
            using qi::char_;
            using qi::lexeme;
 
            using qi::on_error;
            using qi::fail;
 
            start %= * symsbols;
 
            symsbols %=
            (
                (symbol_def_parameter)  |
                (symbol_def_local)      |
                (symbol_def_expr)       
            )
            ;

            symbol_term =
                term                   [_val =  _1, op("math=",_val,0)]
                >> *(  ('+' >> term    [_val += _1, op("math=",_val,0)])
                    |  ('-' >> term    [_val -= _1, op("math=",_val,0)])
                    )
                ;

            term =
                factor                 [_val =  _1, op("math=",_val,0)]
                >> *(  ('*' >> factor  [_val *= _1, op("math=",_val,0)])
                    |  ('/' >> factor  [_val /= _1, op("math=",_val,0)])
                    )
                ;

            factor =
                primary                [_val =  _1, op("math=",_val,0)]
                ;

            primary =
                real                 [_val =  _1, op("math=",_val,0)]
                | '(' >> symbol_term [_val =  _1, op("math=",_val,0)] >> ')'
                | ('-' >> primary    [_val = -_1, op("math=",_val,0)])
                | ('+' >> primary    [_val =  _1, op("math=",_val,0)])

				| ( math_cos >> '(' >> symbol_term >> ')')  [ _val = func_math_cos(_2) ]
				| ( math_sin >> '(' >> symbol_term >> ')')  [ _val = func_math_sin(_2) ]
				| ( math_tan >> '(' >> symbol_term >> ')')  [ _val = func_math_tan(_2) ]

				| ( variable [  _val = op2(_1,_end_result) ])
					>> *( ('+' >> symbol_term  [ _val += _1, op ("math=",_val,1) ])
						| ('-' >> symbol_term  [ _val -= _1, op ("math=",_val,2) ])
						)
                ;

			math_cos = lexeme[no_case["cos"]]; math_cos.name("math_cos");
			math_sin = lexeme[no_case["sin"]]; math_sin.name("math_sin");
			math_tan = lexeme[no_case["tan"]]; math_tan.name("math_tan");
 
 
            symbol_def_parameter =
            (
                symbol_parameter >
            	(   (
                        (
                            (symbol_alpha)
                        >> *(
                                ',' > (symbol_alpha)
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
            	(   (
                        (
                            (symbol_alpha)
                    	)
                        >> *(
                                ',' > (symbol_alpha
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
 
            symbol_alpha %=
            (
                (symbol_alpha_alone
                [
                    _val = qi::_1
            	]   )
            );
            symbol_alpha_alone %= (
                (qi::char_("a-zA-Z")    
                [ _val = qi::_1 ] ) >> *(qi::char_("a-zA-Z0-9_")
                [ _val = qi::_1 ] )
            )
            ;


			qualified_id %= ((qi::alpha | qi::char_("_")) >> *qi::char_("a-zA-Z0-9_"))
			[_val = qi::_1];

            variable = (qualified_id [_val = qi::_1 ] );

            conditions %=
            (   (lit("==") [ var_cond(1) ]) | (lit("<=") [ var_cond(5) ]) |
                (lit(">=") [ var_cond(2) ]) | (lit("=>") [ var_cond(6) ]) |
                (lit("=<") [ var_cond(3) ]) | (lit("!=") [ var_cond(7) ]) |
            	(lit("<" ) [ var_cond(4) ]) | (lit(">" ) [ var_cond(8) ])
            )
            ;

            expression =
            (
				( 
					(variable [ lhs_save_var(qi::_1) ]) >> conditions >>
					(variable [ rhs_save_var(qi::_1) ])
				)
				|
				(
					(variable [ lhs_save_var(qi::_1) ]) >> conditions >>
					(real     [ rhs_save__ar(qi::_1) ])
				)
				|
				(
					(real     [ lhs_save__ar(qi::_1) ]) >> conditions >>
					(variable [ rhs_save_var(qi::_1) ])
				)
				|
				(
					(real     [ lhs_save__ar(qi::_1) ]) >> conditions >>
					(real     [ rhs_save__ar(qi::_1) ])
				)
            )
            ;

			symbol_matched_if =
			(
				(
				    (symbol_if)  >> lit("(")
				>   (expression) >> lit(")")
				>>  (symbol_matched_if >> symbol_else >> symbol_matched_if)
				)
				|
				(
					*(symsbols)
				)
			)
			;

			symbol_unmatched_if =
			(
				(
				  	  (symbol_if)  >> (lit("("))
					> (expression) >>  lit(")")		>> symbol_matched_if
					>> symbol_endif
				)
				|
				(
					  (symbol_if)  >> (lit("("))
					> (expression) >>  lit(")")     >> symbol_unmatched_if
					>> symbol_endif
				)
				|
				(
					  (symbol_if)  >> (lit("("))
					> (expression) >>  lit(")")
					>> symbol_matched_if >> symbol_else >> symbol_unmatched_if
					>> symbol_endif
				)
			)
			;

            symbol_def_if %= ( symbol_matched_if | symbol_unmatched_if );

			any_stringSB %= (
				any_mystring >> *("+" >> any_mystring) [ _val = _1 ]
			)
			;
			any_mystring %=
            (
		        lexeme["'"  >> +(char_ - "'" ) >> "'" ][ _val = _1, any_string_value(_1) ] |
		        lexeme["\"" >> +(char_ - "\"") >> "\""][ _val = _1, any_string_value(_1) ] |
		        lexeme["["  >> +(char_ - "]" ) >> "]" ][ _val = _1, any_string_value(_1) ]
            )
            ;

            symbol_string    %= (any_stringSB  [ _val = _1, any_string_value(_1) ] );
			symbol_def_print  =
			(
				((symbol_print) >> (symbol_string [ print_string(qi::_1) ]))
			)
			;

            symbol_expr %=
            (
	            symbol_term [ _val = _1 ]
			)
            ;

            symbol_def_expr =
            (
                   (variable    [ op2(_1,_end_result) ] )
				>> lit("=")
                >> (symbol_expr [ op2(_end_token,_1) ] )
            )
            ;

            symbol_def_class =
            (
                 symbol_class > (symbol_alpha) >
                 symbol_of    > (symbol_alpha)
            >> *(symsbols) >
                 symbol_endclass [ _val = 1 ]
            )
            ;


            dont_handle_keywords =
            (   symbol_if
            |   symbol_endif
            |   symbol_false
            |   symbol_true
			|	symbol_print
            |   symbol_of
            |   symbol_new
            |   symbol_else
            |   symbol_class
            |   symbol_endclass
            |   symbol_local
            |   symbol_return
            |   symbol_function
            |   symbol_procedure
            |   symbol_parameter
            )
            ;

            any_SB.name("#string req#");
            any_stringSB.name(" bracket STRING bracket");

			symbol_print			.name("PRINT");
            symbol_if               .name("IF");
            symbol_endif            .name("ENDIF");
            symbol_of               .name("OF");
            symbol_new              .name("NEW");
            symbol_else             .name("ELSE");
            symbol_true             .name("TRUE");
            symbol_class            .name("CLASS");
            symbol_false            .name("FALSE");
            symbol_endclass         .name("ENDCLASS");
            symbol_local            .name("LOCAL");
            symbol_return           .name("RETURN");
            symbol_function         .name("FUNCTION");
            symbol_procedure        .name("PROCEDURE");
            symbol_parameter        .name("PARAMETER");

            conditions              .name("conditions expected");

            symbol_true      = ((lexeme[no_case["true" ]] | lexeme[no_case[".t."]]) [_val = true ] );
            symbol_false     = ((lexeme[no_case["false"]] | lexeme[no_case[".f."]]) [_val = false] );
 
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
			symbol_print     =  lexeme[no_case["print"]];

            qi::on_error<fail>( symsbols, client::error_handler(_4, _3, _2) );
		}

        qi::rule<Iterator, QString(), Skipper>
			any_SB,symbol_string,
        	any_stringSB,
			any_mystring;
 
        qi::rule<Iterator, Skipper>
            symbol_expr2expr
			
        ;

		qi::rule<Iterator, double, Skipper>
			symbol_expr;
 
        boost::phoenix::function<compile_op     > op;
		boost::phoenix::function<error          > my_error;
        boost::phoenix::function<line_no_struct > line_func;
 
        qi::rule<Iterator, QString(), Skipper>
        	variable,
        	qualified_id,

        	symbol_alpha, symbol_alpha_alone,
        	symbol_ident;
 
        qi::rule<Iterator, bool, Skipper> symbol_false, symbol_true;
        qi::rule<Iterator,double, Skipper>
			expression,
			symbol_term,
			term, factor, primary;

		qi::rule<Iterator, double,Skipper>
			math_cos,
			math_sin,
			math_tan;
 
        qi::rule<Iterator, Skipper>
			symbol_unmatched_if,
			symbol_matched_if,

        	symsbols, dont_handle_keywords, conditions,
			symbol_local,
			symbol_print,
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
			symbol_def_print,
			symbol_def_parameter,
			symbol_def_procedure, symbol_def_function, symbol_def_return,
			symbol_def_if,
			symbol_def_if_inner,
			symbol_def_stmts,
			symbol_def_stmts_rep,
			symbol_def_local,
			symbol_def_class_inner,
			symbol_def_class;
 
         qi::rule<Iterator, Skipper> quoted_string, any_string;
	};


	template <typename Iterator, typename FPT, typename Skipper = dbase_skipper<Iterator>>
	bool parse(Iterator &iter,
		       Iterator &end,
		       const dbase_grammar<Iterator, FPT, Skipper> &g,
		       FPT &result)
	{
		typedef dbase_skipper <Iterator> skipper;
		skipper skips;
		bool r =
		boost::spirit::qi::phrase_parse(
		   iter, end, g, skips, result);
		   return r;
	}

}  // client namespace

extern class MyEditor *global_textedit;
bool my_parser(std::string const str)
{
	typedef std::string::const_iterator iterator_t;
    const   std::string s = str.c_str();

    iterator_t iter = s.begin();
    iterator_t end  = s.end  ();

	typedef client::dbase_skipper <iterator_t> skipper;
	typedef client::dbase_grammar <iterator_t, double, skipper> grammar;
	grammar pg;

	bool r = client::parse(iter, end, pg, client::_end_result);

	QVariant v = client::_end_result;
	if (r == true) {
		int line = global_textedit->lines;
		QString succ = QString("Parsing SUCCESS!\nres: %1\nLines: %2").
		arg(v.toString()).
		arg(line);
		MsgBox("Information",succ);
	}	return r;
}

bool parseText(std::string const s, int m)
{
	using namespace client;

	antifreeze::init();

    // -------------------
	// set values to 0 ...
	// -------------------
	line_no      = 1;
	_end_result  = 0.00;
	my_not_error = true;

	code.clear();
    code.resize(0);

    bool r = false;
	try {
		r = my_parser(s);
		if (r) {
			if (my_not_error == true) {
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

