#ifndef SKIPPER_H
#define SKIPPER_H

#include <iostream>
#include "symbol.h"

using namespace std;

template <class T>      // template forward
class Symbol;           // declaration

template <class T>
class Skipper {
public:
    Skipper<T>() {
        cout << "skipser" << endl;
    }
    Skipper<T> operator << (Symbol<T> sym) {
        cout << "sduduudud" << endl;
        return *this;
    }
};

#endif // SKIPPER_H

