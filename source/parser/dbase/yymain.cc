#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_ACTIONS_ALLOW_ATTR_COMPAT

#include <math.h>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/ref.hpp>
#include <boost/phoenix.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>

#include <boost/lockfree/stack.hpp>
#include "antifreeze.h"

#include <iostream>
#include <string>
#include <vector>

#include <qstring.h>
#include <QVector>
#include "source/includes/editorgutter.h"

using namespace std;
using namespace boost::phoenix;

using boost::phoenix::function;
using boost::phoenix::ref;
using boost::phoenix::size;

using namespace boost::spirit;
using namespace boost::spirit::qi;

namespace phx = boost::phoenix;
namespace ph  = boost::phoenix::placeholders;

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

	int  if_depth  = 0;
	bool exec_flag = true;


	QString st_name1;
	QString st_name2;
	QString last;

	QMutex mutex;

	bool my_not_error = true;
    bool isLastClass = false;
    
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
		op_assign,

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

	struct mini_ast;
    typedef
        boost::variant<
            boost::recursive_wrapper<mini_ast>
          , std::string
		  , double
        >
    ast_node;

	struct mini_ast {
		mini_ast() {}
		mini_ast(mini_ast const & ast) {
			name   = ast.name;
			value  = ast.value;
		}

		enum byte_code bc;
		double value;
		std::string name;
		std::string op_name;
		std::vector<ast_node> child;
	};

	struct mini_ast dbase_ast;
}

namespace client
{
	int const tabsize = 4;
	int roh_int = 0;

    void tab(int indent)
    {
        for (int i = 0; i < indent; ++i)
            std::cout << ' ';
    }

    struct mini_ast_printer
    {
        mini_ast_printer(int indent = 0)
          : indent(indent)
        {
        }

        void operator()(mini_ast const& ast) const;
		void operator()(double   const& val) const;
        int indent;
    };

    struct mini_ast_node_printer : boost::static_visitor<>
    {
        mini_ast_node_printer(int indent = 0)
          : indent(indent)
        {
        }

        void operator()(mini_ast const& ast) const {
            mini_ast_printer(indent+tabsize)(ast);
		}

		void operator()(double const & val) const  {
			mini_ast_printer(indent+tabsize)(val); }

        void operator()(std::string const& text) const {
            tab(indent+tabsize);
            std::cout << "text: \"" << text << '"' << std::endl;
        }

        int indent;
    };

	void mini_ast_printer::operator()(double const & val) const
	{
		std::cout << "left : { " << val << " }" << std::endl;
		std::cout << "right: { " << val << " }" << std::endl;
	}
    void mini_ast_printer::operator()(mini_ast const & ast) const
    {
        tab(indent);
		mini_ast mast = ast;

		if (roh_int == 1) {
			if (mast.op_name.size() < 1) {
				mast.op_name = std::string("assign");
			}
				
			mast.name    = ast.name;
			mast.value   = ast.value;

			std::cout	<< mast.op_name << ": "
						<< ast.name    << " [ "
						<< ast.value   << " ] "
						<< std::endl;
		}	else
			std::cout << "root" << std::endl;

        tab(indent);
        std::cout << '{' << std::endl;

        BOOST_FOREACH(ast_node const& node, ast.child) {
			roh_int = 1;
			boost::apply_visitor(mini_ast_node_printer(indent), node);
		}

        tab(indent);
        std::cout << '}' << std::endl;
    }

	mini_ast& vm_add(mini_ast &mast, std::string name, double value)
	{
		mini_ast ast;
		ast.value = value;
		ast.bc    = byte_code::op_assign;
		ast.op_name = std::string("assign");
		ast.name  = name;
		mast.child.push_back(ast);
		return mast;
	}

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
 
const int whatIsClass     = 2;
const int whatIsLocal     = 3;
const int whatIsParameter = 4;

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
			_end_token = n1;

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

			if (QString(t1) == QString("math+")) { _val = _end_result += r; } else
			if (QString(t1) == QString("math-")) { _val = _end_result -= r; } else
			if (QString(t1) == QString("math*")) { _val = _end_result *= r; } else
			if (QString(t1) == QString("math/")) { _val = _end_result /= r; } else

			if (QString(t1) == QString("mathcos")) { _val = _end_result = ::cos(r); } else
			if (QString(t1) == QString("mathsin")) { _val = _end_result = ::sin(r); } else
			if (QString(t1) == QString("mathtan")) { _val = _end_result = ::tan(r); }

			mini_ast dbase_tmp;
			dbase_tmp.name    = std::string("root");
			dbase_tmp.op_name = std::string("assign");

			dbase_ast = vm_add(dbase_tmp,_end_token.toStdString(),_end_result);
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
		qi::rule<Iterator> my_skip;
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

	struct token_value_get {
		typedef void result_type;
		token_value_get() { }
		void operator()(QString const &t1, double const &t2) const {
			QString s1 = t1;
			if (s1.size()         < 1) s1 = _end_token;
			if (_end_token.size() < 1) _end_token = s1;

            if (isLastClass == false) {
			    MsgBox("info",QString("--->%1\n---> %2 : %3").arg(s1).arg(t2).arg(_end_result));
			    _end_result = t2;
			}
		}
	};
	phoenix::function<token_value_get> get_token_value;

	// ----------------------
	// math ...
	// ----------------------
	struct math_func_cos	{
		template <typename T>
		double operator()(T const &t) const {
			return ::cos(t);
		}
    };
    struct math_func_sin {
		template <typename T>
		double operator()(T const &t)  const {
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
					 break;
				 }
			}

			if (found == false)
			{
				res = code_iterator->value.toDouble();
			}

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
		}
	};
	// string functions ...
	phoenix::function<str_func_assign> func_str_assign;

    QString code_str;
    QString mem_str;
    int code_pos = 0;
    
    int     lab_count = 1;
    int     ife_count = 1;

	QString string_value;

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

int if_occur = 1;
int ifA = 1;
int ifB = 1;

	struct print_string_ {
		typedef QString result_type;
		template <typename T>
	    QString operator()(T const &t1) const
	    {
	        mem_str.append(
	            QString("mem_%1 = \"%2\"\n")
	                .arg(lab_count++)
	                .arg(t1));
	                
	        code_str.append(
	            QString("push LC%1\n").arg(lab_count-1) +
	            QString("call print\n"));

			return t1;
		}
	};
	phoenix::function<print_string_> print_string;

    //
    struct expr_condition {
        QString op;
        bool exec_flag;

        QVariant lhs; int lmem;
        QVariant rhs; int rmem;
    };
    
    struct expr_condition exprcond;
    struct lhs_symbol_expr_ {
        template <typename T>
	    void operator()(T const &t1) const {
            exprcond.lhs = t1;
            exprcond.lmem++;
        }
    };
    struct rhs_symbol_expr_ {
        template <typename T>
	    void operator()(T const &t1) const {
            exprcond.rhs = t1;
            exprcond.rmem++;
        }
    };
 
    struct my_endif_if_
    {
        template <typename T1, typename T2>
	    void operator()(T1 const &t1, T2 const &t2) const
	    {
	        if (if_occur-1 == 1) {
    	        code_str
    	            .append(QString("=== _jmp _L%2:\n")
	                .arg(--if_occur-1));
	        }
	        else if (if_occur-1 <= 0) {
	            code_str
	                .append(QString("---\nL%1:\nret\n")
	                .arg(--if_occur));
	        }
	        else {
	            int c;
	            int L1 = --if_occur;
	            
	            static int if_flag = 1;
                static int if_array[2048];
                
                bool found = false;
                
                for (c = 0; c < 2048; c++) {
                    if ((if_array[c] == 'u')
                    &&  (c == 2)) {
                        found = true;
                        break;
                    }
                }
	            
	            if (found)
	            c = 1; else
	            c = 0;
	            
	            code_str
	                .append(QString("---\n_jmp _L%2:\nL%1:\n")
	                .arg(L1-c)
	                .arg(L1-c));
	                
	            if (if_flag) {
	                if (L1-1 <= 0) {
	                    if_flag = 0;
	                    for (c = 0; c < 2048; c++)
	                    if_array[c] = 'o';
	                }
	            }
	        }
	            
	        ifA = if_occur;
	            
	        QString str;
	        str .append(code_str)
	            .append("L0:\nret\n")
	            .append(mem_str);
	            
	        MsgBox("2222---2222",str);
	    }
    };
    struct my_check_if_
    {                           
        template <typename T1, typename T2>
	    void operator()(T1 const &t1, T2 const &t2) const
        {
            mem_str
                .append(QString("mem_%2 = %1\n")
                .arg(exprcond.lhs.toDouble())
                .arg(lab_count++));
            mem_str
                .append(QString("mem_%2 = %1\n")
                .arg(exprcond.rhs.toDouble())
                .arg(lab_count++));
            
            code_str
                .append(QString("cmp mem[%3], mem[%4]\n")
                .arg(lab_count-2)
                .arg(lab_count-1));
                
            ifB = ++if_occur;
            if (ifB == ifA)
            --ifB;
            
            int c;
            if (ifB-1 < 1)
            c = ifB; else
            c = ifB - 1;
            
            code_str
                .append(QString("jne L%1\n")
                .arg(c));
                            
            MsgBox("imformer",code_str);
        }        
    };
    
    phoenix::function<my_endif_if_> endif_if;
    phoenix::function<my_check_if_> check_if;
    
    phoenix::function<lhs_symbol_expr_> lhs_set_expr;
    phoenix::function<rhs_symbol_expr_> rhs_set_expr;
    //
     
    struct add_parameter_
    {
        typedef void result_type;
        void operator()(QString str) const
        {
            bool found = false;
            _end_token = str;
            
            if (code.isEmpty()) {
			    my_ops ops;
			    ops.name = str;
			    ops.what = whatIsLocal;
			    ops.value = true;
			    code.append(ops);
		    }

            for (code_iterator  = code.begin();
			    code_iterator != code.end();
			    code_iterator++) {

			    if (str == code_iterator->name)
			    {
				    _end_token = str;
				    found      = true;
				    break;
			    }
		    }

		    if (found == false)
		    {	my_ops ptr;
			    ptr.name = str;
			    ptr.what = whatIsParameter;
			    ptr.value = true;
			    code.append(ptr);
		    }
        }
    };

    struct add_local_
    {
        typedef void result_type;
        void operator()(QString str) const
        {
        MsgBox("localer",str);
		    bool found = false;
		    _end_token = str;

		    if (code.isEmpty()) {
			    my_ops ops;
			    ops.name = str;
			    ops.what = whatIsLocal;
			    code.append(ops);
		    }

		    for (code_iterator  = code.begin();
			     code_iterator != code.end();
			     code_iterator++) {

			    if (str == code_iterator->name)
			    {
				    _end_token = str;
				    found      = true;
				    break;
			    }
		    }

		    if (found == false)
		    {	my_ops ptr;
			    ptr.name = str;
			    ptr.what = whatIsLocal;
			    code.append(ptr);
		    }
        }
    };
    
    class MyClassList {
    public:
        QString name;
        QVariant ctype;
    };

    QVector<MyClassList> ClassContainer;
    struct add_class_
    {
        typedef void result_type;
        void operator()(QString str) const
        {
            MyClassList tmp;
            tmp.name = str;
            tmp.ctype = 1;
            ClassContainer << tmp;
            
            isLastClass = true;
            MsgBox("info",str);
        }
    };
    
    struct check_token_ {
        typedef void result_type;
        void operator()(QString str, double val) const
        {
            if (isLastClass == false)
            {
                get_token_value(str,val);
                op2(str,val);
                get_token_value(_end_token,val);
            }
            else {
			    bool found = false;
			    _end_token = str;

			    if (code.isEmpty()) {
				    my_ops ops;
				    ops.name = str;
				    ops.what = whatIsClass;
				    code.append(ops);
			    }

			    for (code_iterator  = code.begin();
				     code_iterator != code.end();
				     code_iterator++) {

				    if (str == code_iterator->name)
				    {
					    _end_token = str;
					    found      = true;
					    break;
				    }
			    }

			    if (found == false)
			    {	my_ops ptr;
				    ptr.name = str;
				    ptr.what = whatIsClass;
				    code.append(ptr);
			    }
            }
            
            isLastClass = false;
        }
    };
    phoenix::function<check_token_>   check_token;
    phoenix::function<add_parameter_> add_parameter;
    phoenix::function<add_local_>     add_local;
    phoenix::function<add_class_>     add_class;
    
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
                symbol_parameter > (variable >> *qi::space)
                [
                    add_parameter(phx::construct<QString>(_1))
                ]
                >> *( ',' > variable >> *qi::space)
                [
                    add_parameter(phx::construct<QString>(_1))
                ]
            )
            ;
            
            symbol_def_local =
            (
            	(
            	    symbol_local > (variable >> *qi::space)
                    [
                        add_local(phx::construct<QString>(_1))
                    ]
                    >> *(',' > variable >> *qi::space)
                    [
                        add_local(phx::construct<QString>(_1))
                    ]
                )
            )
            ;
 
            symbol_alpha %=
            (
                (symbol_alpha_alone)
                [
                    _val = qi::_1
            	]
            );
            symbol_alpha_alone %= (
                (qi::char_("a-zA-Z")    
                [ _val = qi::_1 ] ) >> *(qi::char_("a-zA-Z0-9_")
                [ _val = qi::_1 ] )
            )
            ;


			qualified_id %= ((qi::alpha | qi::char_("_")) >> *qi::char_("a-zA-Z0-9_"))
			[_val = qi::_1];

            variable = (qualified_id) [_val = qi::_1 ];


            lhs_symbol_expr %= symbol_expr [ _val = _1];
            rhs_symbol_expr %= symbol_expr [ _val = _1];
            
			symbol_matched_if %=
			(			    
			    (lhs_symbol_expr >> "==" >> rhs_symbol_expr)
			    [
			        lhs_set_expr(phx::construct<double>(qi::_1)),
			        rhs_set_expr(phx::construct<double>(qi::_2)),
			        check_if(exprcond.lmem, exprcond.rmem)
			    ]
			    |
			    (*qi::space >> lit("!")        >>
			     *qi::space >> lhs_symbol_expr >>
			     *qi::space)
			    [
			        lhs_set_expr(false),
			        rhs_set_expr(false)
			    ]
			    |
			    (*qi::space >> lhs_symbol_expr >> *qi::space)
			    [
			        lhs_set_expr(true),
			        rhs_set_expr(true)
			    ]
			)
			;

            symbol_def_if %=
            (
                symbol_if
                    >> *qi::space >> *lit("(")
                    >> *qi::space >> symbol_matched_if
                    >> *qi::space >> *lit(")")
                    >> *qi::space >> *(symsbols)
                    >> *qi::space >>
                symbol_endif [ endif_if(1,1) ]
            )
            ;
            
            symbol_def_else %=
            (
                (symbol_else >> *qi::space)
            )
            ;
            
            while_cond %=
            (
                  ("!" >> variable >> "=" >> symbol_expr)
                | (       variable >> "=" >> symbol_expr)
            )
            ;
            symbol_def_while %=
            (
                symbol_while >> "(" >> while_cond >> ")" >> *symsbols >>
                symbol_endwhile
            )
            ;

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
            symbol_expr =
            (
	            (
	                (symbol_new >> variable >> "(" >> ")")
	                [
	                    add_class(phx::construct<QString>(_1))
	                ]
	            )
	            |
	            (symbol_term [ _val = _1 ] )
			)
            ;

            symbol_def_expr =
            (
                (
						(symbol_def_if)
					|   (symbol_def_else)
					|   (symbol_def_while)
					
					|
					(	(symbol_print)
					>>	(
                			(symbol_string [ print_string(qi::_1) ] )
		            	)
					)
		            |
					(      (variable >> lit("=")
		        		>>  symbol_expr)
							[
							    check_token(_1,_2)
							]
		            )
				)
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
			|   symbol_while | symbol_endwhile
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
			symbol_while            .name("WHILE");
			symbol_endwhile         .name("ENDWHILE");
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
            symbol_endwhile  =  lexeme[no_case["endwhile"]];
            symbol_while     =  lexeme[no_case["while"]];
            
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
			symbol_expr,
			lhs_symbol_expr,
			rhs_symbol_expr
	    ;
 
        boost::phoenix::function<compile_op     > op;
		boost::phoenix::function<error          > my_error;
        boost::phoenix::function<line_no_struct > line_func;
 
        qi::rule<Iterator, QString()>
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
			symbol_matched_if
		;

        qi::rule<Iterator, Skipper>

        	symsbols, dont_handle_keywords, conditions,
			symbol_local,
			symbol_print,
			
			symbol_while, symbol_endwhile,
			
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
 
            symbol_def_while, while_cond,
            symbol_def_else,
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

	skipper sk;
	grammar pg;

	bool r = client::parse(iter, end, pg, client::_end_result);

	QVariant v = client::_end_result;
	if (r == true) {
		int line = global_textedit->lines;
		QString succ = QString("Parsing SUCCESS!\nres: %1\nLines: %2").
		arg(v.toString()).
		arg(line);
		MsgBox("Information",succ);

//		client::mini_ast_printer ast;
//		ast(client::dbase_ast);

	}	return r;
}

bool parseText(std::string s)
{
	using namespace client;

    // -------------------
	// set values to 0 ...
	// -------------------
	line_no      = 1;
	_end_result  = 0.00;
	my_not_error = true;

	code.clear();
    code.resize(0);
    
    // if conditions ...
    client::exec_flag = true;
    
    client::lab_count = 0;
    client::ife_count = 0;
    
    client::code_str.clear();
    client::mem_str .clear();
            
    client::exprcond.lmem = 0;
    client::exprcond.rmem = 0;

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

