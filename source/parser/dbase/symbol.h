#ifndef SYMBOL_H
#define SYMBOL_H

#include <iostream>
#include "skipper.h"

using namespace std;

template <class T>
class Symbol {
public:
    Symbol<T>() { }
    Symbol<T>(std::string sym) {
        cout << "symser:  " << sym << endl;
    }
    Symbol<T> operator () (std::string sym) {
        cout << "SYM:  " << sym << endl;
        return *this;
    }
    Symbol<T> operator << (Skipper<T> sk) {
        cout << "skipsly" << endl;
    }
};

#endif // SYMBOL_H
