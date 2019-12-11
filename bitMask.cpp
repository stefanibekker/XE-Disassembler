/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
bitMask.cpp
*/
#include "bitMask.hpp"
#include <bitset>

using namespace std;

/* Change Last Two Bits to 11 */
string maskOpBit(std::string opCode) {

    unsigned int opValue;
    stringstream opStream;
    stringstream opString;

    opStream << hex << opCode;
    opStream >> opValue;
    bitset<8> opBin(opValue);
    opBin.set(0);
    opBin.set(1);
    opString << setfill('0') << setw(2) << hex << uppercase << opBin.to_ulong();
    string result = opString.str();
    return result;
}