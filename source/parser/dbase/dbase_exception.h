#ifndef DBASE_EXCEPTION_H
#define DBASE_EXCEPTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <typeinfo>
#include <set>
#include <utility>
#include <vector>

namespace dBaseParser {
// --------------------------------
// exception class for don't match
// -------------------------------
class MydBaseMissException: public exception
{
    virtual const char* what() const throw()
    {
        return "dBaseException occur.";
    }
} dBaseMissException;

struct my_dbase_throw {
    my_dbase_throw() { }
    my_dbase_throw(int dummy) {
        throw dBaseMissException;
    }
};
}

#endif // DBASE_EXCEPTION_H

