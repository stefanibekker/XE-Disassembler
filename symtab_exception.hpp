/* 
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
symtab_exception.hpp
*/

#ifndef SYMTAB_EXCEPTION_H
#define SYMTAB_EXCEPTION_H
#include <string>

using namespace std;

class symtab_exception {

public:
    symtab_exception(string s) {
        message = s;
    }

    symtab_exception() {
        message = "An error has occured";
    }

    string getMessage() {
        return message;
    }

private:
    string message;
};
#endif