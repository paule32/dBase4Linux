#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_ACTIONS_ALLOW_ATTR_COMPAT

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <string>
#include <iostream>

#define USE_QT
#ifdef  USE_QT
#include <QMessageBox>
#endif

using namespace std;
using namespace boost::spirit;

namespace client
{
     namespace fusion = boost::fusion;
     namespace phoenix = boost::phoenix;

     namespace qi = boost::spirit::qi;
     namespace ascii = boost::spirit::ascii;

     template <typename Iterator>
     struct dbase_skipper : public qi::grammar<Iterator>
     {
         dbase_skipper() : dbase_skipper::base_type(my_skip, "dBase")
         {
             using qi::ascii::char_;
             using qi::ascii::space;
             using qi::eol;
             using qi::eoi;

             my_skip = (char_("[ \t\n\r]"))                            |
             ("**" >> *(char_ - eol) >> (eol | eoi | char_("[\n\r]"))) |
             ("&&" >> *(char_ - eol) >> (eol | eoi | char_("[\n\r]"))) |
             ("//" >> *(char_ - eol) >> (eol | eoi | char_("[\n\r]"))) |
             ("/*" >> *(char_ - "*/") >> "*/")
             ;

             BOOST_SPIRIT_DEBUG_NODE((my_skip));
         }
         qi::rule<Iterator> my_skip;
     };

     template <typename Iterator, typename Skipper = dbase_skipper<Iterator>>
     struct dbase_grammar : public qi::grammar<Iterator, Skipper>
     {
         qi::rule<Iterator, Skipper> start, run_app;
         dbase_grammar()  : dbase_grammar::base_type(start)
         {
             using boost::spirit::ascii::no_case;

             using qi::lit;
             using qi::char_;

             using qi::on_error;
             using qi::fail;

             using phoenix::construct;
             using phoenix::val;

             start   = run_app.alias();
             run_app = - symsbols;

             expression =
                     term.alias()
                     >> *(
                       ('+' >> term )
                     | ('-' >> term ))
                     ;
             term =
                     factor.alias()
                     >> *(
                       ('*' >> factor )
                     | ('/' >> factor ))
                     ;

             factor =
                     ( symbol_digit
                     ///| symbol_alpha
                     )
                     >> *(
                     ('('   >> expression >> ')')
                     | ('-' >> factor     )
                     | ('+' >> factor     ))
                     ;

             symsbols =
                 (((symbol_class > symbol_alpha >
                    symbol_of    > symbol_alpha > symbol_endclass >> run_app) |
                   (symbol_class > symbol_alpha >
                    symbol_of    > symbol_alpha > symbol_endclass)
                   )              |
                 ((symbol_alpha  > qi::char_('=') > expression >> run_app) |
                  (symbol_alpha  > qi::char_('=') > expression         ))
                 )
                 ;

             symbol_class    = no_case["class"];
             symbol_of       = no_case["of"];
             symbol_endclass = no_case["endclass"];

             symbol_space =
                 +(qi::char_(" \t\n\r") | eol | eoi)
                 ;

             symbol_alpha =
                  qi::char_("a-zA-Z_") >>
                 *qi::char_("a-zA-Z0-9_")
                 ;

             symbol_digit =
                 +(qi::digit)
                 ;

             on_error<fail>
             (
                 start
               , std::cout
                     << val("Error! Expecting ")
                     << _4                               // what failed?
                     << val(" here: \"")
                     << construct<std::string>(_3, _2)   // iterators to error-pos, end
                     << val("\"")
                     << std::endl
             );


             BOOST_SPIRIT_DEBUG_NODE(start);
             BOOST_SPIRIT_DEBUG_NODE(symsbols);
             BOOST_SPIRIT_DEBUG_NODE(symbol_of);
             BOOST_SPIRIT_DEBUG_NODE(symbol_endclass);
             BOOST_SPIRIT_DEBUG_NODE(symbol_class);
             BOOST_SPIRIT_DEBUG_NODE(symbol_space);
             BOOST_SPIRIT_DEBUG_NODE(symbol_alpha);
             BOOST_SPIRIT_DEBUG_NODE(symbol_digit);


             BOOST_SPIRIT_DEBUG_NODE(expression);
             BOOST_SPIRIT_DEBUG_NODE(term);
             BOOST_SPIRIT_DEBUG_NODE(factor);
         }

         qi::rule<Iterator, std::string()>
         symbol_alpha;

         qi::rule<Iterator, Skipper>
         symsbols,
         symbol_digit,
         symbol_space,
         symbol_class,
         symbol_endclass,
         symbol_of;

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
         std::cout << "Remaining: '" << std::string(iter, end) << std::endl;
         #ifdef USE_QT
         QMessageBox::information(0,"Parser", "Parsing ERROR.");
         #else
         std::cout << "ERROR" << std::endl;
         #endif
         return false;
     }   return false;
}

bool parseText(std::string str, int mode)
{
     return my_parser(str);
}















#ifdef _ASDASD_ASDASD_ASD_S_AS_DASDASDASD
#include "includes/mainwindow.h"
#include "dBaseWindow.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
}

#include <iostream>
#include <sstream>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <map>
#include <stack>
#include <exception>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <type_traits>

#include "symbol.h"
#include "skipper.h"

using namespace std;

int lineno = 1;
std::string var_name;


// --------------------------------
// exception class for don't match
// -------------------------------
class MydBaseMissException: public std::exception
{
    virtual const char* what() const throw()
    {
        std::string msg;
        msg  = "dBaseException occur.";
        return msg.c_str();
    }
} dBaseMissException;

enum dBaseTypes {
    unknown,
    b_value,        // bool
    p_value,        // parameter
    if_value,       // if stmt
    w_value,        // widget type
    m_value,
    c_value,
    i_value         // int type
};

// actual opcode
std::string opCode;

struct MopCodes {
    char   meno[10];    // menomic
    char   opmen[10];   // ...
    ushort mtype;       // machine type 8/16/32 bit
    char   len;         // opcode len
    int    op1;         // opcode // normal
    int    op2;         // ...    //  8 bit
    int    op3;         // ...    // 16 bit
} _MopCodes[] = {
    { "add", "al" ,  8, 1, 0x04    , 0x0     , 0x0 },    // add al, 1-255        ; 0x04 + n(1-255)
    { "add", "ah" ,  8, 2, 0xc480  , 0x0     , 0x0 },    // add ah, 1-255        ;  --
    { "add", "ax" , 16, 2, 0x8366  , 0xc0    , 0x0 },    // add ax, 256-65535
    { "add", "eax", 32, 1, 0x05    , 0x0     , 0x0 },    // add eax, 32-bit -> + 0x01010000
    { "add", "rax", 64, 2, 0x0548  , 0x0     , 0x0 },    // add rax, 64-bit -> + 0x01010000

    { "add", "bl" ,  8, 1, 0xc380  , 0x0     , 0x0 },
    { "add", "bh" ,  8, 2, 0xc780  , 0x0     , 0x0 },
    { "add", "bx" , 12, 2, 0x8166  , 0xc3    , 0x0 },
    { "add", "ebx", 32, 2, 0xc381  , 0xc381  , 0x0 },
    { "add", "rbx", 64, 3, 0xc38348, 0xc38148, 0x0 },

    { "add", "cl" ,  8, 1, 0x03    , 0x0     , 0x0 },
    { "add", "ch" ,  8, 2, 0x03    , 0x0     , 0x0 },
    { "add", "cx" , 12, 1, 0x03    , 0x0     , 0x0 },
    { "add", "ecx", 32, 1, 0x03    , 0x0     , 0x0 },
    { "add", "rcx", 64, 1, 0x03    , 0x0     , 0x0 },

    { "add", "dl" ,  8, 1, 0x03    , 0x0     , 0x0 },
    { "add", "dh" ,  8, 2, 0x03    , 0x0     , 0x0 },
    { "add", "dx" , 12, 1, 0x03    , 0x0     , 0x0 },
    { "add", "edx", 32, 1, 0x03    , 0x0     , 0x0 },
    { "add", "rdx", 64, 1, 0x03    , 0x0     , 0x0 },

    { "add", "cl" ,  8, 1, 0x03    , 0x0     , 0x0 },
    { "add", "ch" ,  8, 2, 0x03    , 0x0     , 0x0 },
    { "add", "cx" , 12, 1, 0x03    , 0x0     , 0x0 },
    { "add", "ecx", 32, 1, 0x03    , 0x0     , 0x0 },
    { "add", "rcx", 64, 1, 0x03    , 0x0     , 0x0 },

    { "add", "dl" ,  8, 1, 0x03    , 0x0     , 0x0 },
    { "add", "dh" ,  8, 2, 0x03    , 0x0     , 0x0 },
    { "add", "dx" , 12, 1, 0x03    , 0x0     , 0x0 },
    { "add", "edx", 32, 1, 0x03    , 0x0     , 0x0 },
    { "add", "rdx", 64, 1, 0x03    , 0x0     , 0x0 },

    { "add", "sp" ,  8, 1, 0x03    , 0x0     , 0x0 },
    { "add", "esp",  8, 2, 0x03    , 0x0     , 0x0 },
    { "add", "cx" , 12, 1, 0x03    , 0x0     , 0x0 },
    { "add", "ecx", 32, 1, 0x03    , 0x0     , 0x0 },
    { "add", "rcx", 64, 1, 0x03    , 0x0     , 0x0 },

    { "nop", ""   ,  8, 1, 0x90    , 0x0     , 0x0 }     // nop
};
const int maxOpCodeListSize = 36;

struct MopCodeTmp {
    char len;
    int  mtype;
    int  mop[3];
    char aop[10];
};

struct MopCodeTmp myOpCode;

class dBaseStmt {
public:
    dBaseTypes  data_type;
    dBaseTypes  data_type_extra;

    virtual ~dBaseStmt() { }
};

class dBaseVariable: public dBaseStmt {
public:
    dBaseVariable(std::string name, double value) {
        this->value_double =  value;
        this->name  = name;

        left  = new dBaseVariable;
        right = new dBaseVariable;
    }
    dBaseVariable() { }

    dBaseVariable *left;
    dBaseVariable *right;

    double value_double;
    std::string  name;

    void new_left () { left  = new dBaseVariable; }
    void new_right() { right = new dBaseVariable; }

    virtual ~dBaseVariable() { }
};

class dBaseAddExpr: public dBaseVariable {
public:
    dBaseAddExpr(double lhs, double rhs)   {
        if (rhs < lhs) {
            new_left (); left ->value_double = rhs;
            new_right(); right->value_double = lhs;
        }   else {
            new_left (); left ->value_double = lhs;
            new_right(); right->value_double = rhs;
        }
    }
    dBaseAddExpr(dBaseAddExpr lhs, double rhs) {
        if (rhs < lhs.value_double) {
            new_left (); left ->value_double = rhs;
            new_right(); right->value_double = lhs.value_double;
        }   else {
            new_left (); left ->value_double = lhs.value_double;
            new_right(); right->value_double = rhs;
        }
    }
    dBaseAddExpr(dBaseVariable& lhs, dBaseVariable &rhs)
    {
        if (rhs.value_double < lhs.value_double) {
            new_left (); left ->value_double = rhs.value_double;
            new_right(); right->value_double = lhs.value_double;
        }   else {
            new_right(); right->value_double = lhs.value_double;
            new_left (); left ->value_double = rhs.value_double;
        }
    }
    dBaseAddExpr(dBaseVariable*& lhs, dBaseVariable *&rhs)
    {
        if (rhs->value_double < lhs->value_double) {
            new_left (); left ->value_double = rhs->value_double;
            new_right(); right->value_double = lhs->value_double;
        }   else {
            new_right(); right->value_double = lhs->value_double;
            new_left (); left ->value_double = rhs->value_double;
        }
    }
    virtual ~dBaseAddExpr() { }
};

class dBaseMulExpr: public dBaseVariable {
public:
    dBaseMulExpr(double lhs, double rhs) {
        if (rhs < lhs) {
            new_left (); left ->value_double = rhs;
            new_right(); right->value_double = lhs;
        }   else {
            new_left (); left ->value_double = lhs;
            new_right(); right->value_double = rhs;
        }
    }
    dBaseMulExpr(dBaseAddExpr lhs, double rhs) {
        if (rhs < lhs.value_double) {
            new_left (); left ->value_double = rhs;
            new_right(); right->value_double = lhs.value_double;
        }   else {
            new_left (); left ->value_double = lhs.value_double;
            new_right(); right->value_double = rhs;
        }
    }
    dBaseMulExpr(dBaseVariable& lhs, dBaseVariable& rhs)
    {
        if (rhs.value_double < lhs.value_double) {
            new_left (); left ->value_double = rhs.value_double;
            new_right(); right->value_double = lhs.value_double;
        }   else {
            new_left (); left ->value_double = lhs.value_double;
            new_right(); right->value_double = rhs.value_double;
        }
    }
    dBaseMulExpr(dBaseAddExpr *& lhs, double rhs)
    {
        if (rhs < lhs->value_double) {
            new_left (); left ->value_double = rhs;
            new_right(); right->value_double = lhs->value_double;
        }   else {
            new_left (); left ->value_double = lhs->value_double;
            new_right(); right->value_double = rhs;
        }
    }
    dBaseMulExpr(dBaseAddExpr *& lhs, dBaseVariable *& rhs)
    {
        if (rhs->value_double < lhs->value_double) {
            new_left (); left ->value_double = rhs->value_double;
            new_right(); right->value_double = lhs->value_double;
        }   else {
            new_right(); right->value_double = lhs->value_double;
            new_left (); left ->value_double = rhs->value_double;
        }
    }
    dBaseMulExpr(dBaseMulExpr *& lhs, dBaseAddExpr *& rhs)
    {
        if (rhs->value_double < lhs->value_double) {
            new_left (); left ->value_double = lhs->value_double;
            new_right(); right->value_double = rhs->value_double;
        }   else {
            new_left (); left ->value_double = rhs->value_double;
            new_right(); right->value_double = lhs->value_double;
        }
    }
    virtual ~dBaseMulExpr() { }
};

std::vector<dBaseStmt*> stmt;
bool eval()
{
    double res_arr[10];
    double sum;
    int flag = 0;

    for (auto it = std::begin(stmt); it != std::end(stmt); ++it)
    {
        dBaseVariable *item_1 = dynamic_cast<dBaseStmt*>(*it);
        if (item_1) {
            cout << item_1->name << " = " << item_1->value_double << endl;
            continue;
        }

        /*
        dBaseAddExpr *item_2 = dynamic_cast<dBaseStmt*>(*it);
        if (item_2) {
            cout << "A===>  " << item_2->value_double << endl;
            break;
        }

        dBaseMulExpr *itemB = dynamic_cast<dBaseStmt*>(*it);
        if (itemB) {
            if (itemB->value_double != 0) {
                res_arr[flag++] = itemB->value_double;
                cout << "B===> " << itemB->value_double << endl;

                if (flag == 3) {
                    double res =  // claculate: multiplication
                    res_arr[2] *  // before any other operations
                    res_arr[1] +  // ...
                    res_arr[0] ;  itemB->value_double = res;

                    sum  = res ;
                    flag = 0;
                    cout << "RES: " << res << endl;
                }
            }
        }
        */
    }
    return false;
}

const int TOKEN_ERROR  = -1;
const int TOKEN_OK     =  0;
const int TOKEN_SYMBOL =  1;
const int TOKEN_NUMBER =  2;
const int TOKEN_PARAMETER = 3;
const int TOKEN_CLASS  =  4;

std::map<std::string, int> token_list = {
    { "class", TOKEN_CLASS },
    { "parameter", TOKEN_PARAMETER }
};

std::string token;
std::string sourcecode;
int spos = -1;

int skip(int c)
{
    if (c == '\t' || c == ' ') {
        return c;
    }   else
    if (c == '\n' || c == '\r') {
        ++lineno;
        return c;
    }
    return -1;
}

int check_skip()
{
    int c = sourcecode[++spos];
    if (spos > sourcecode.length()) {
        throw dBaseMissException;
    }
    return c;
}

// ---------------------------------
// a parser base class for dBase ...
// ---------------------------------
#ifdef template_dbase_parser2
const  int PARSER_DBASE = 1;
class dBase;

template <class T>
class ParserCommon {
public:
    T& operator = (const T& src) {
        sourcecode  = src.sourcecode;
        parser_type = src.parser_type;
    }

    int parser_type;
};

class dBase {
public:
    dBase() { }
    dBase(std::string src) { cout << "aaaaaaaa" << endl; }

    bool dbase_handle_white_space()
    {
        while (1) {
            int c = sourcecode[++spos];
            if (c == ' '  || c == '\t') continue; else
            if (c == '\n' || c == '\r') {
                ++lineno;
                continue;
            }
            else {
                break;
            }
        }
        return true;
    }

    bool dbase_handle_symbol(int ch = 0)
    {
        while (1) {
            int c = sourcecode[++spos];
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
            {
                token = c;
                while (1)
                {
                    c = sourcecode[++spos];
                    if (spos > sourcecode.length()) {
                        //throw dBaseMissException;
                        break;
                    }
                    if (c == ' '  || c == '\t') { return true; }
                    if (c == '\n' || c == '\r') {
                        ++lineno;
                        return true;;
                    }
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                    ||  (c >= '0' && c <= '9') || (c == '_')) {
                        token += c;
                        continue;
                    }
                }
            }
            else return false;
        }        return false;
    }

    bool dbase_handle_char(char ch) {
        return dbase_handle_white_space();
    }
};

template <class T>
class CharParser {
public:
    CharParser<T>() { }
    CharParser<T> operator () (char ch) {
        if (typeid(T) == typeid(dBase)) {
            cout << "char:  " << ch << endl;
            dBase *db  = new dBase;
            int result = db->dbase_handle_char(ch);
            delete db;
        }
        return *this;
    }
};

template <class T>
class Parser: public ParserCommon<T>, public dBase {
public:
    Parser() {}
    Parser<T>(std::string src) {
        sourcecode  = src;
    }
    bool start(void);
    Parser<T> operator << (Parser<T> o) {
        cout << "opser" << endl;
    }
    Parser<T> operator << (Skipper<T> skip) {
        if (typeid(T) == typeid(dBase))  {  }
        return *this;
    }
    Parser<T> operator << (Symbol<T> sym) {
        if (typeid(T) == typeid(dBase))  { }
        return *this;
    }
    Parser<T> operator << (CharParser<T> sym) {
        if (typeid(T) == typeid(dBase))  {    }
        return *this;
    }
};

template <class T>
bool Parser<T>::start(void)
{
    QMessageBox::information(w,"ssss","classsler");
    return true;
}

namespace dBaseParser {
Skipper    <dBase> skip;
Symbol     <dBase> symbol;
CharParser <dBase> char_;
}

bool parseCode(std::string src)
{
    using namespace dBaseParser;
    Parser<dBase> g(src);

    g << skip << symbol(std::string("zuulu"))
      << skip << char_('=')
      << skip;

    try {
        bool result = g.start();
        if (result)
        QMessageBox::information(w,"text parser","SUCCESS"); else
        QMessageBox::information(w,"text parser","ERROR");

        return result;
    }
    catch (exception& e) {
        QMessageBox::information(w,"parser error",e.what());
        return false;
    }   return false;
}
#endif

int skip_white_space(int mode=0)
{
    int c;
    while (1)
    {
        c = sourcecode[++spos];
        if (spos > sourcecode.length()) {
            //throw dBaseMissException;
            break;
        }

        if ((c == '\n') || (c == '\r')) {
            ++lineno;
            continue;
        }

        if ((c == '\t') || (c == ' ')) {
            continue;
        }

        // ------------------
        // assembler comment
        // ------------------
        if (c == ';' && mode) {
            cout << "comment" << endl;
            while (1) {
                c = sourcecode[++spos];
                if (c == '\n' || c == '\r') {
                    ++lineno;
                    break;
                }
            }
            continue;
        }

        if (mode == 2) {
            if (c == ',') continue;
        }

        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
        {
            token = c;
            while (1)
            {
                c = sourcecode[++spos];
                if (spos > sourcecode.length()) {
                    throw dBaseMissException;
                    break;
                }
                if (c == ',') goto etok;
                if (c == ' '  || c == '\t') {
                    if (token == "nop")
                    return TOKEN_SYMBOL; else
                    goto etok;
                }
                if (c == '\n' || c == '\r') {
                    ++lineno;
                    if (token == "nop")
                    return TOKEN_SYMBOL; else
                    goto etok;
                }
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                ||  (c >= '0' && c <= '9') || (c == '_')) {
                    token += c;
                }   else {
                    etok:
                    if (mode == 1) {
                        for (int i = 0; i < maxOpCodeListSize; ++i)   {
                        if (!strcmp(_MopCodes[i].meno,token.c_str())) {
                            strcpy(myOpCode.aop,_MopCodes[i].meno);
                            myOpCode.mop[0] = _MopCodes[i].op1;
                            myOpCode.mop[1] = _MopCodes[i].op2;
                            myOpCode.mop[2] = _MopCodes[i].op3;

                            myOpCode.len    = _MopCodes[i].len;
                            myOpCode.mtype  = _MopCodes[i].mtype;

                            return TOKEN_SYMBOL;
                        }}  return TOKEN_ERROR;
                    }
                    else if (mode == 2) {
                        for (int i = 0; i < maxOpCodeListSize; ++i)   {
                        if (!strcmp(_MopCodes[i].opmen,token.c_str())){
                            strcpy(myOpCode.aop,_MopCodes[i].meno);
                            myOpCode.mop[0] = _MopCodes[i].op1;
                            myOpCode.mop[1] = _MopCodes[i].op2;
                            myOpCode.mop[2] = _MopCodes[i].op3;

                            myOpCode.len   = _MopCodes[i].len;
                            myOpCode.mtype = _MopCodes[i].mtype;

                            return TOKEN_SYMBOL;
                        }}  return TOKEN_ERROR;
                    }
                    else {
                        for(auto const &atoken : token_list)
                        {
                            if (atoken.first == token)
                            return atoken.second;
                        }
                    }
                    return TOKEN_SYMBOL;
                }
            }
        }

        else if (c == '0') {
            token = c;
            while (1)     {
                c = sourcecode[++spos];
                if (spos > sourcecode.length()) {
                    //throw dBaseMissException;
                    break;
                }   else
                if (c == '.') {     // double
                    token += c;
                    while (1)     {
                        c = sourcecode[++spos];
                        if (spos > sourcecode.length()) {
                            //throw dBaseMissException;
                            break;
                        }
                        if (c == ' '  || c == '\t') return TOKEN_NUMBER;
                        if (c == '\n' || c == '\r') {
                            ++lineno;
                            return TOKEN_NUMBER;
                        }
                        if (c >= '0'  && c <= '9' ) {
                            token += c;
                            continue;
                        }   else {
                            --spos;
                            //return skip(TOKEN_NUMBER);
                        }
                    }
                    return TOKEN_ERROR;
                }   else
                if (c == 'x' || c == 'X') {  // hex todo
                }   else
                if (c == '\t' || c == ' ' ) break;
                if (c == '\n' || c == '\r') {
                    ++lineno;
                    break;
                }
            }
            return TOKEN_NUMBER;
        }

        else if (c >= '1' && c <= '9') {
            token = c;
            while (1)     {
                c = sourcecode[++spos];
                if (spos > sourcecode.length()) {
                    //throw dBaseMissException;
                    break;
                }
                if (c == '.') { token += c; continue; }
                if (c == '+') {
                    --spos;
                    return TOKEN_NUMBER;
                }
                if (c == '\t' || c == ' ')  { return TOKEN_NUMBER; }
                if (c == '\n' || c == '\r') {
                    ++lineno;
                    return TOKEN_NUMBER;
                }
                if (c >= '0' && c <= '9') {
                    token += c;
                    continue;
                }   else {
                    --spos;
                    //return skip(TOKEN_NUMBER);
                }
            }
            cout << "ME: " << token << endl;
            return TOKEN_NUMBER;
        }


        if (c == '&') {
            c = sourcecode[++spos];
            if (spos > sourcecode.length()) {
                //throw dBaseMissException;
                break;
            }
            if (c == '&') {             // c++ style comment
                while (1)
                {
                    c = sourcecode[++spos];
                    if (spos > sourcecode.length()) {
                        //throw dBaseMissException;
                        break;
                    }
                    if (c == '\n' || c == '\r') {
                        ++lineno;
                        break;
                    }
                }
            }
        }

        else if (c == '/') {
            c = sourcecode[++spos];
            if (spos > sourcecode.length()) {
                //throw dBaseMissException;
                break;
            }
            if (c == '/') {             // c++ style comment
                while (1)
                {
                    c = sourcecode[++spos];
                    if (spos > sourcecode.length()) {
                        //throw dBaseMissException;
                        break;
                    }
                    if (c == '\n' || c == '\r') {
                        ++lineno;
                        break;
                    }
                }
            }
            else if (c == '*')      // c style comment
            {
                while (1)
                {
                    c = sourcecode[++spos];
                    if (spos > sourcecode.length()) {
                        //throw dBaseMissException;
                        break;
                    }   else
                    if (c == '\n' || c == '\r') {
                        ++lineno;
                        continue;
                    }   else
                    if (c == '/') {
                        break;
                    }
                }
                continue;
            }
        }

        else if (c == '*') {
            while (1) {
                c = sourcecode[++spos];
                //printf("tips: %d, %c\n",c,c);
                if (spos > sourcecode.length()) {
                    //throw dBaseMissException;
                    break;
                }
                if (c == '*') {  // dbase comment **
                    while (1) {
                        int c = sourcecode[++spos];
                        if (spos > sourcecode.length()) {
                            //throw dBaseMissException;
                            break;
                        }   else
                        if (c == '\t' || c == ' ') continue;
                        if (c == '\n' || c == '\r') {
                            ++lineno;
                            break;
                        }   else {
                            continue;
                        }
                    }
                    cout << "dbase coomm" << endl;
                    break;
                }

                if (c == ' '  || c == '\t') { return '*'; }
                if (c == '\n' || c == '\r') {
                    ++lineno;
                    return c;
                }
                else if (c >= '1' && c <= '9') {
                    token = c;
                    static int flag = 0;
                    while (1)     {
                        if (c == '-') {
                            if (flag == 0) {
                                flag = 1;
                                token = std::string("-") + token;
                            }
                        }
                        if ((c >= '0' && c <= '9') || (c == '.')) {
                            token += c;
                            continue;
                        }
                        else if (c == '.') {
                            token += c;
                            continue;
                        }
                        else if (c == ' '  || c == '\t') {
                            cout << ":: " << token << endl;
                            flag = 0;
                            return TOKEN_NUMBER;
                        }
                        else if (c == '\n' || c == '\r') {
                            ++lineno;
                            flag = 0;
                            cout << "---" << endl;
                            return TOKEN_NUMBER;
                        }   else {
                            flag = 0;
                            --spos;
                            cout << "===" << endl;
                            return TOKEN_NUMBER;
                        }
                    }
                }
            }
        }

        else if (c == '-') {
            token = c;
            c = sourcecode[++spos];
            if (c >= '1' && c <= '9') {
                token += c;
                while (1) {
                    c = sourcecode[++spos];
                    if ((c >= '0' && c <= '9') || (c == '.')) {
                        token += c;
                        continue;
                    }   else
                    if (c == '\t' || c == ' ' ) return TOKEN_NUMBER;
                    if (c == '\n' || c == '\r') {
                        ++lineno;
                        return TOKEN_NUMBER;
                    }   else
                    if (c == '.')   continue;
                    if (c == '+') { return '+'; }
                    if (c == '*') { return '*'; }
                    if (c == '-') { return '-'; }
                }
            }
            else if (c == ' '  || c == '\t') { return '-'; }
            else if (c == '\n' || c == '\r') {
                ++lineno;
                return '-';
            }
            else {
                return '-';
            }
        }

        if (c == '=') break;
        if (c == '+') break;

    }
    return c;
}

bool expr(void)
{
    int c  = skip_white_space();
    if (c != TOKEN_NUMBER) {
        return false;
    }   return true;
}

double get_expr(double pre)
{
    double prev  = 0.00;
    double aprev = 0.00;
    double pprev = 0.00;

    prev = atof(token.c_str());
    int c;

    n1:
    c = skip_white_space();
    if (c == '+')
    {
        if (expr())
        {
            cout << "pvor:  " << aprev << endl;
            cout << "prev:  " << prev  << endl;
            cout << "adds:  " << token << endl;

            double r1  = atof(token.c_str());
            double r2  = 0.00;
            double res = 0.00;

            int c = skip_white_space();
            if (c == '*') {
                if (expr()) {
                    r2 = r1 * atof(token.c_str()) + prev;
                    cout << "add2:  " << token << endl;
                    cout << "zwre:  " << r2    << endl;

                    prev = r2;
                    res  = r2;
                }
            }
            else if (c == '-') {
                res = r1 + prev;
                cout << "res1: " << res << endl;
                if (expr()) {
                    res  = res - atof(token.c_str());
                    prev = res;
                    cout << "tok2:  " << res << endl;
                }
            }
            else if (c == '+') {
                double r3 = prev + r1;
                cout << "r22:  " << r3 << endl;
                if (expr()) {
                    res = atof(token.c_str());
                    prev = res;
                    cout << "next: " << prev << endl;
                }
            }
            else {
                double r3 = prev + r1;
                res  = r3;
                prev = res;
                cout << "r2 = " << r3 << endl;
            }

            //expr_queue.push(atof(token.c_str()));

            aprev = atof(token.c_str());
            //double res = prev;

            //auto expr = new dBaseVariable(var_name,res);
            prev = res; //expr->value_double;

            cout << "A:> " << " = " << res << endl;
            goto n1;
        }
    }
    else if (c == '-')
    {
        if (expr())
        {
            cout << "pvor:  " << aprev << endl;
            cout << "prev:  " << prev  << endl;
            cout << "subs:  " << token << endl;

            double r1  = atof(token.c_str());
            double r2  = 0.00;
            double res = 0.00;

            int c = skip_white_space();
            if (c == '*') {
                if (expr()) {
                    r2   = atof(token.c_str());
                    r2   = (r1   * r2);
                    prev = (prev - r2);

                    cout << "sub2:  " << token << endl;
                    cout << "zwre:  " << prev  << endl;

                    res  = prev;
                }
            }
            else if (c == '-') {
                if (expr()) {
                    r2  = atof(token.c_str());
                    res = prev - r1 - r2;


                    cout << "tok3:  " << res << endl;
                }
            }
            else if (c == '+') {
                cout << "plusser" << endl;
                if (expr()) {
                    r2   = prev - r1 - atof(token.c_str());
                    res  = r2;
                    prev = res;
                    //res = prev + r1 + r2;
                    cout << "tok5:  " << res << endl;
                }
            }
            else {
                cout << "pre4:  " << prev << endl;
                res = prev - r1;
                cout << "r5 = " << res << endl;
            }

            //expr_queue.push(atof(token.c_str()));

            aprev = atof(token.c_str());

            //auto expr = new dBaseVariable(var_name,res);
            prev = res; //expr->value_double;

            cout << "A:> " << " = " << res << endl;
            goto n1;
        }
    }
    else if (c == '*')
    {
        if (expr())
        {
            cout << "pvor:  " << aprev << endl;
            cout << "prev:  " << prev  << endl;
            cout << "muls:  " << token << endl;

            double r1  = atof(token.c_str());
            double r2  = 0.00;
            double res = 0.00;

            int c = skip_white_space();
            if (c == '*') {
                if (expr()) {
                    r2   = atof(token.c_str());
                    r2   = (r1   * r2);
                    prev = (prev - r2);

                    cout << "sub2:  " << token << endl;
                    cout << "zwre:  " << prev  << endl;

                    res  = prev;
                }
            }
            else if (c == '-') {
                if (expr()) {
                    r2  = atof(token.c_str());
                    res = prev - r1 - r2;

                    cout << "tok3:  " << res << endl;
                }
            }
            else if (c == '+') {
                cout << "plusser" << endl;
                if (expr()) {
                    double r3 = 0.00;
                    r2 = atof(token.c_str());
                    r3 = r2 * prev;
                    res = r3;
                    prev = res;

                    cout << "tok3:  " << res << endl;
                }
            }
            else {
                cout << "pre4:  " << prev << endl;
                res = prev - r1;
                cout << "r3 = " << res << endl;
            }

            //expr_queue.push(atof(token.c_str()));

            aprev = atof(token.c_str());

            //auto expr = new dBaseVariable(var_name,res);
            prev = res; //expr->value_double;

            cout << "A:> " << " = " << res << endl;
            goto n1;
        }
    }
    return prev;
}

bool parse_code(std::string src)
{
    sourcecode = src;
    int c;

    while (1)
    {
        c = skip_white_space();
        if (c == TOKEN_SYMBOL) {
            var_name = token;
            c  = skip_white_space();
            if (c == '=')
            {
                QMessageBox::information(w,"1212121","var");
                if (expr())  {
                    cout << "222" << endl;
                    double res = get_expr(atof(token.c_str()));
                    dBaseVariable *var = new
                    dBaseVariable(var_name,res);

                    cout << "444" << endl;
                    cout << var_name << " = " << res << endl;
                    continue;
                }
            }
            else break;
        }
        else if (c == TOKEN_PARAMETER) {
            QMessageBox::information(w,"text parser","parameter");
            c = skip_white_space();
            if (c == TOKEN_SYMBOL) {
                QMessageBox::information(w,"text parser","var");
                continue;
            }
            else break;
        }
        else
        break;
    }

    return true ;
}

void Capitalize(std::string &str)
{
    int i = 0;
    int c;
    while (str[i]) {
        c = str[i];
        str[i] = tolower(c);
        i++;
    }
}

static FILE *AsmBinOutput = nullptr;

template <class T>
void endswap(T *objp)
{
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}

void write_code(FILE *f, struct MopCodeTmp op, int num=0, bool nf=false)
{
    int x = op.mop[0];
    char buffer[100];

    // ---------
    // add al ...
    // ---------
    if (x == 0x0548) {                 // add rax
        sprintf(buffer, "%04x",num);
        sscanf (buffer, "%x"  ,&x );

        fwrite(&op.mop[0],2,1,f);
        fwrite(&x,sizeof(int),1,f);
    }
    else if (x == 0x05)                // add eax
    {
        sprintf(buffer, "%08x",num);
        sscanf (buffer, "%x"  ,&x );

        fwrite(&op.mop[0],1,1,f);
        fwrite(&x,sizeof(int),1,f);
    }
    else if (x == 0x8366)               // add ax
    {
        fseek(f,-1,SEEK_CUR);

        if (num < 0x100) {
            fwrite(&x,2,1,f);

            sprintf(buffer, "%01x", num);
            sscanf (buffer, "%x"  , &x );

            int c = 0xc0;

            fwrite(&c,1,1,f);
            fwrite(&x,1,1,f);
        }
        else if (num > 0x100) {
            buffer[1] = 0x05;
            buffer[0] = 0x66;

            fwrite(buffer,2,1,f);

            sprintf(buffer, "%02x", num);
            sscanf (buffer, "%x"  , &x );

            fwrite(&x,2,1,f);
        }
    }
    else if (x == 0xc480) {             // add ah
        sprintf(buffer, "%01x", num);
        sscanf (buffer, "%01x", &x );

        fwrite(&op.mop[0],2,1,f);
        fwrite(&x,2,1,f);
    }
    else if (x == 0x04) {               // add al
        sprintf(buffer, "%01x", num);
        sscanf (buffer, "%01x", &x );

        fwrite(&op.mop[0],1,1,f);
        fwrite(&x,1,1,f);
    }

    // ----------
    // add bl...
    // ----------
    else if (x == 0xc380) {             // add bl
        fwrite(&op.mop[0],2,1,f);
        fwrite(&num,1,1,f);
    }
    else if (x == 0xc780) {             // add bh
        fwrite(&x,2,1,f);
        fwrite(&num,1,1,f);
    }
    else if (x == 0x8166)             // add bx
    {
        fwrite(&x,2,1,f); x = 0xc3;
        fwrite(&x,1,1,f);
        fwrite(&num,2,1,f);
    }
    else if (x == 0xc381)
    {                                    // add ebx
        fwrite(&x,2,1,f);
        fwrite(&num,sizeof(int) ,1,f);
    }
    else if (x == 0x90) {
        cout << "noppser" << endl;
        fwrite(&x,1,1,f);
    }
    else {
        cout << "plupso" << endl;
    }
}

void assemble_code(FILE *f, std::string code)
{
    sourcecode = code;
    int c;
    int mode = 1;
    while (1) {
        c = skip_white_space(1);
        cout << c << endl;
        if (c == TOKEN_SYMBOL) {            // left menomic
            std::string meno = token;
            cout << token << endl;

            if (token != "nop") {
                c = skip_white_space(2);
                if (c == TOKEN_SYMBOL)          // op
                {
                    std::string meno_op = token;
                    cout << "----> " << token << endl;

                    c = skip_white_space(2);     // value
                    if (c == TOKEN_NUMBER) {

                        cout << "add number: " << token << endl;

                        write_code(f,myOpCode,atoi(token.c_str()),true);
                        continue;
                    }
                }
            }
            else {
                myOpCode.mop[0] = 0x90;
                write_code(f,myOpCode);
            }
        }
        else break;
    }
}

bool Assemble(void)
{
    std::string code = R"(
;
; test comment
;
    nop
    add al , 2   ; comment after instruction
; next
    add al, 3
            add ah, 12

            add ax, 257

            add eax, 257

            add rax, 32

            nop

            add bl, 6
            add bh, 9
            add bx, 12

            nop

            add bx, 13

            add ebx, 257

            nop


)";

    if ((AsmBinOutput = fopen("a.out","w+b")) == NULL)
    {
        QMessageBox::information(w,"File Open Error","Output file could not be open.");
        return false;
    }

    assemble_code(AsmBinOutput,code);
    fclose(AsmBinOutput);
    return true;
}

bool parseText(QString text, int mode)
{
    std::string source_code = R"(

// xxxxxx
** comment
** dddd
        // ddsd
&& xxxxx
   /*
   fff */
Azrael = -1.2 + 0.4 //+ 6 - 7 - 2 - 2 + 2 + 3

)";

    spos = -1;
    //return Assemble();

    parse_code(source_code);
    eval();

    //Parser<dBase> parse(text.toStdString());

}
#endif
