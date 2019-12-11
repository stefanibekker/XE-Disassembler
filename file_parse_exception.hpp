/*  
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
file_parse_exception.hpp
*/    

#ifndef FILE_PARSE_EXCEPTION_H
#define FILE_PARSE_EXCEPTION_H
#include <string>

using namespace std;

class file_parse_exception {

public:
    file_parse_exception(string s) {
        message = s;        
        }
        
    file_parse_exception() {
        message = "An error has occurred";
        }
        
    string getMessage() {
        return message;
    }
    
private:
    string message;
};    
#endif