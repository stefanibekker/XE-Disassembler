/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
printRecords.hpp
*/
#ifndef PRINTRECORDS_H
#define PRINTRECORDS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include "symtab.hpp"
#include "calculateAddress.hpp"
#include "convert.hpp"

void printHeaderRec(std::string label, std::string beginAdr,std::ofstream& inStream);
void printTextRecord(std::string label, std::string mnemonic, std::string operand, std::ofstream& inStream, bool xFlag, bool eFlag, bool nFlag, bool iFlag);
void printEndRec(std::string endRec, std::ofstream& inStream, std::string label);
void printRes(symtab& inSymtab, std::string lastTxtRec, std::string textRecordAddress,std::ofstream& inStream);

#endif
