/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
convert.cpp
*/
#include "convert.hpp"
#include <stdlib.h>

using namespace std;

// Converts Between Different Types

string hexToBin(string input) {

    unsigned int hexVal = strtoul(input.c_str(),NULL,16);
    bitset<4> hexBit(hexVal);
    string binput = hexBit.to_string<char,string::traits_type,string::allocator_type>();
    return binput;
}

string hexToDec(string input) {

    unsigned int decNum;
    stringstream decStream;
    stringstream hexStream;

    hexStream << hex << input;
    hexStream >> decNum;
    decStream << decNum;
    return decStream.str();
}

string intToString(int input) {

    stringstream intInput;

    intInput << input;
    return intInput.str();
}

int stringToInt(string input) {

    return atoi(input.c_str());
}
