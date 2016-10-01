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

using namespace std;

int lineno = 1;

template <class T>
class SafeQueue
{
public:
    SafeQueue(void)
    : q()
    , m()
    , c()
    {}

    ~SafeQueue(void)
    {}

    void push(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    T pop(void)
    {
        std::unique_lock<std::mutex> lock(m);
        while(q.empty())
        {
        c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};

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
const int TOKEN_CLASS  =  3;

std::map<std::string, int> token_list = {
    { "class", TOKEN_CLASS }
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
}

int check_skip()
{
    int c = sourcecode[++spos];
    if (spos > sourcecode.length()) {
        throw dBaseMissException;
    }
    return c;
}

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
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                ||  (c >= '0' && c <= '9') || (c == '_')) {
                    token += c;
                }   else {
                    --spos;
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
                            return skip(TOKEN_NUMBER);
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
                    return skip(TOKEN_NUMBER);
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
                        if ((c >= '0' && c <= '9' || c == '.')) {
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
                    if (c >= '0' && c <= '9' || c == '.') {
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


std::string var_name;
SafeQueue<double> expr_queue;

double get_expr(double pre)
{
    double prev  = 0.00;
    double aprev = 0.00;
    double pprev = 0.00;

//    auto exp1 = new dBaseVariable(var_name,pre);
//    stmt.push_back(exp1);
    prev = atof(token.c_str());

    n1:
    int c = skip_white_space();
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
    while (1)
    {
        int c = skip_white_space();
        if (c == TOKEN_SYMBOL) {
            var_name = token;
            c  = skip_white_space();
            if (c == '=')
            {
                while (1)
                {
                    if (expr())  {
                        double res = get_expr(atof(token.c_str()));
                        cout << var_name << " = " << res << endl;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main()
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

    bool result = parse_code(source_code);
    if (result)
    cout << "SUCCESS" << endl; else
    cout << "ERROR"   << endl;

    return 0;
}
