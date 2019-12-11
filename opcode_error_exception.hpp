  /*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
opcode_error_exception.hpp
*/

#ifndef OPCODE_ERROR_EXCEPTION_H
#define OPCODE_ERROR_EXCEPTION_H
#include <string>

using namespace std;

class opcode_error_exception {

public:
    opcode_error_exception(string s) {
        message = s;
    }

    opcode_error_exception() {
        message = "An error has occured";
    }

    string getMessage() {
        return message;
    }

private:
    string message;
};
#endif