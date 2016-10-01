#include "includes/mainwindow.h"
#include "dBaseWindow.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
}

#include <iostream>
#include <cstddef>
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


struct Encod {       // contains mnemonic information
    char  Mnem [8];  //   The mnemonic
    char  Byte1[9];  //   Initial byte information
    uchar OpType;    //   The operand type
    uchar Byte2;     //   The second byte or reg field
    uchar Mclass;    //   The class of mneomoic
};

const int _BX = 1;
const int _SI = 2;
const int _DI = 4;
const int _BP = 8;

enum RegList {
    EAX = 'EAX', EBX = 'EBX', ECX = 'ECX', EDX = 'EDX', ESP = 'ESP', EBP = 'EBP', ESI = 'ESI', EDI = 'EDI',
    AX  = 'AX' , BX  = 'BX' , CX  = 'CX' , DX  = 'DX' , SP  = 'SP' , BP  = 'BP' , SI  = 'SI' , DI  = 'DI' ,
    AH  = 'AH' , BH  = 'BH' , CH  = 'CH' , DH  = 'DH' ,
    AL  = 'AL' , BL  = 'BL' , CL  = 'CL' , DL  = 'DL'
};

const int   MaxReg = 24;

const uchar AddrMode_1 = _BX + _SI;
const uchar AddrMode_2 = _BX + _DI;
const uchar AddrMode_3 = _BP + _SI;
const uchar AddrMode_4 = _BP + _DI;
const uchar AddrMode_5 = _SI;
const uchar AddrMode_6 = _DI;
const uchar AddrMode_7 = _BP;
const uchar AddrMode_8 = _BX;

// ---------------------------------------------------------------------------
// Each instruction has an operand-type associated with it.  I have provided a
// sample instruction in comments to demonstrate the differences among them.
// ---------------------------------------------------------------------------
const int NO_OPERAND =            0;   // NOP
const int REG_MEM_REGISTER =      1;   // MOV BX,ES:[BP+5+SI] or MOV CX,DX
const int IMMEDIATE_AL_AX =       2;   // CMP AX,56
const int IMMEDIATE_REG_MEM =     3;   // CMP BX,78 or CMP SS:[BP+8],0Ah
const int DIRECT_IN_SEGMENT =     4;   // CALL 567H
const int INDIRECT_IN_SEGMENT =   5;   // CALL [78]
const int DIRECT_INTRASEGMENT =   6;   // JMP 6543:8765
const int INDIRECT_INTRASEGMENT = 7;   // JMP FAR [BX]
const int REGISTER_MEMORY =       8;   // PUSH BL or PUSH ES:[BX]
const int A16_BIT_REGISTER =      9;   // PUSH BX
const int ESC =                  10;   // ESC ES:[BP+DI+7],101011b
const int IMMEDIATE_PORT =       11;   // IN AL,0ABCh
const int PORT_ADDRESS_IN_DX =   12;   // IN AL,DX
const int INT =                  13;   // INT 67H or INT 3
const int EIGHT_BIT_REL =        14;   // JL 90 or JMP SHORT 90
const int MEMORY_AL_AX =         15;   // MOV ES:[SI],AL
const int AL_AX_MEMORY =         16;   // MOV AX,CS:[DI]
const int REG_MEM_SR =           17;   // MOV BX,ES or MOV [2],SS
const int SR_REG_MEM =           18;   // MOV ES,BX or MOV SS,[2]
const int SEGMENT_REGISTER =     19;   // PUSH ES
const int ANOTHER_INSTRUCTION =  20;   // REP MOVSB (MOVSB is the other instruction)
const int RET =                  21;   // RET or RET 6
const int IMMEDIATE_REGISTER =   22;   // MOV BX,67

struct MopCodes {
    char   meno[10];    // menomic
    char   opmen[10];   // ...
    ushort mtype;       // machine type 8/16/32 bit
    int    op;          // opcode
} _MopCodes[] = {
    { "add", "al" ,  8, 0x04   },    // add al, 1-255        ; 0x04 + n(1-255)
    { "add", "ah" ,  8, 0x80c4 },    // add ah, 1-255        ;  --
    { "add", "ax" , 16, 0x6605 },    // add ax, 256-65535
    { "add", "eax", 32, 0x05   },    // add eax, 32-bit -> + 0x01010000
    { "add", "rax", 64, 0x4805 },    // add rax, 64-bit -> + 0x01010000
    { "nop", ""   ,  0, 0x90   }     // nop
};


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
            item_1 = nullptr;
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

int skip_white_space()
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

        if (c == ';') return ';';
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
                if (c == ' '  || c == '\t') goto etok;
                if (c == '\n' || c == '\r') {
                    ++lineno;
                    goto etok;
                }
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                ||  (c >= '0' && c <= '9') || (c == '_')) {
                    token += c;
                }   else {
                    etok:
                    for(auto const &atoken : token_list)
                    {
                        if (atoken.first == token)
                        return atoken.second;
                    }   return TOKEN_SYMBOL;
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
        if (c == ',') break;

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

void assemble_code(FILE *f, std::string code)
{
    sourcecode = code;
    int c;
    while (1) {
        c = skip_white_space();
        if (c == ';') {
            cout << "comment" << endl;
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
    add al, 2   ; comment after instruction

)";

    if ((AsmBinOutput = fopen("a.out","w+b")) == NULL)
    {
        QMessageBox::information(w,"File Open Error","Output file could not be open.");
        return false;
    }

    cout << "putzi" << endl;
    assemble_code(AsmBinOutput,code);
    fclose(AsmBinOutput);
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

    return Assemble();

    //return parse_code(source_code);

    //Parser<dBase> parse(text.toStdString());

}
