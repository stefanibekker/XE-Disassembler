 /*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
convert.hpp
*/
#ifndef CONVERT_H
#define CONVERT_H

#include <sstream>
#include <string>
#include <bitset>

int stringToInt(std::string inString);
std::string intToString(int inInt);
std::string hexToBin(std::string hexString);
std::string hexToDec(std::string hexString);

#endif
