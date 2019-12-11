/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
calculateAddress.hpp
*/
#ifndef CALCULATEADDRESS_H
#define CALCULATEADDRESS_H

#include <sstream>
#include <string>
#include <bitset>
#include <iomanip>
#include <map>

std::string incrementAddress(std::string loc, std::string incrementLength, int pcOpt);
int subtractAddress(std::string lower, std::string upper);
std::string calculateBase(std::string tempAdr, std::map<std::string,std::string> baseTxtRec, bool iFlag);

#endif
