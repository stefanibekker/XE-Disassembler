/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
Optab.cpp
*/
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>

#include "Optab.hpp"
#include "opcode_error_exception.hpp"

using namespace std;

//get format and mnemonic
pair<string,string> Optab::getOp(string opCode) {    
    if(opcode.count(opCode) == 0)
        return make_pair("","");
    pair<multimap<string,string>::iterator, multimap<string,string>::iterator> iter;
    iter = opcode.equal_range(opCode);
    string maskedOp;
    string opFormat;
    pair<string,string> opData;
    multimap<string,string>::iterator i = iter.first;
    maskedOp = i->second;
    i++;
    opFormat = i->second;
    opData = make_pair(maskedOp,opFormat);
    return opData;
}
//opcode with last two bits set to 11
Optab::Optab() {
    initMap(m);
    opcode.insert(pair<string,string>("1B","ADD"));
    opcode.insert(pair<string,string>("1B","3/4"));
    opcode.insert(pair<string,string>("5B","ADDF"));
    opcode.insert(pair<string,string>("5B","3/4"));
    opcode.insert(pair<string,string>("93","ADDR"));
    opcode.insert(pair<string,string>("93","2"));
    opcode.insert(pair<string,string>("43","AND"));
    opcode.insert(pair<string,string>("43","3/4"));
    opcode.insert(pair<string,string>("B7","CLEAR"));
    opcode.insert(pair<string,string>("B7","2"));
    opcode.insert(pair<string,string>("2B","COMP"));
    opcode.insert(pair<string,string>("2B","3/4"));
    opcode.insert(pair<string,string>("8B","COMPF"));
    opcode.insert(pair<string,string>("8B","3/4"));
    opcode.insert(pair<string,string>("A3","COMPR"));
    opcode.insert(pair<string,string>("A3","2"));
    opcode.insert(pair<string,string>("27","DIV"));
    opcode.insert(pair<string,string>("27","3/4"));
    opcode.insert(pair<string,string>("67","DIVF"));
    opcode.insert(pair<string,string>("67","3/4"));
    opcode.insert(pair<string,string>("9F","DIVR"));
    opcode.insert(pair<string,string>("9F","2"));
    opcode.insert(pair<string,string>("C7","FIX"));
    opcode.insert(pair<string,string>("C7","1"));
    opcode.insert(pair<string,string>("C3","FLOAT"));
    opcode.insert(pair<string,string>("C3","1"));
    opcode.insert(pair<string,string>("F7","HIO"));
    opcode.insert(pair<string,string>("F7","1"));
    opcode.insert(pair<string,string>("3F","J"));
    opcode.insert(pair<string,string>("3F","3/4"));
    opcode.insert(pair<string,string>("33","JEQ"));
    opcode.insert(pair<string,string>("33","3/4"));
    opcode.insert(pair<string,string>("37","JGT"));
    opcode.insert(pair<string,string>("37","3/4"));
    opcode.insert(pair<string,string>("3B","JLT"));
    opcode.insert(pair<string,string>("3B","3/4"));
    opcode.insert(pair<string,string>("4B","JSUB"));
    opcode.insert(pair<string,string>("4B","3/4"));
    opcode.insert(pair<string,string>("03","LDA"));
    opcode.insert(pair<string,string>("03","3/4"));
    opcode.insert(pair<string,string>("6B","LDB"));
    opcode.insert(pair<string,string>("6B","3/4"));
    opcode.insert(pair<string,string>("53","LDCH"));
    opcode.insert(pair<string,string>("53","3/4"));
    opcode.insert(pair<string,string>("73","LDF"));
    opcode.insert(pair<string,string>("73","3/4"));
    opcode.insert(pair<string,string>("0B","LDL"));
    opcode.insert(pair<string,string>("0B","3/4"));
    opcode.insert(pair<string,string>("6F","LDS"));
    opcode.insert(pair<string,string>("6F","3/4"));
    opcode.insert(pair<string,string>("77","LDT"));
    opcode.insert(pair<string,string>("77","3/4"));
    opcode.insert(pair<string,string>("07","LDX"));
    opcode.insert(pair<string,string>("07","3/4"));
    opcode.insert(pair<string,string>("D3","LPS"));
    opcode.insert(pair<string,string>("D3","3/4"));
    opcode.insert(pair<string,string>("23","MUL"));
    opcode.insert(pair<string,string>("23","3/4"));
    opcode.insert(pair<string,string>("63","MULF"));
    opcode.insert(pair<string,string>("63","3/4"));
    opcode.insert(pair<string,string>("9B","MULR"));
    opcode.insert(pair<string,string>("9B","2"));
    opcode.insert(pair<string,string>("CB","NORM"));
    opcode.insert(pair<string,string>("CB","1"));
    opcode.insert(pair<string,string>("47","OR"));
    opcode.insert(pair<string,string>("47","3/4"));
    opcode.insert(pair<string,string>("DB","RD"));
    opcode.insert(pair<string,string>("DB","3/4"));
    opcode.insert(pair<string,string>("AF","RMO"));
    opcode.insert(pair<string,string>("AF","2"));
    opcode.insert(pair<string,string>("4F","RSUB"));
    opcode.insert(pair<string,string>("4F","3/4"));
    opcode.insert(pair<string,string>("A7","SHIFTL"));
    opcode.insert(pair<string,string>("A7","2"));
    opcode.insert(pair<string,string>("AB","SHIFTR"));
    opcode.insert(pair<string,string>("AB","2"));
    opcode.insert(pair<string,string>("F3","SIO"));
    opcode.insert(pair<string,string>("F3","1"));
    opcode.insert(pair<string,string>("EF","SSK"));
    opcode.insert(pair<string,string>("EF","3/4"));
    opcode.insert(pair<string,string>("0F","STA"));
    opcode.insert(pair<string,string>("0F","3/4"));
    opcode.insert(pair<string,string>("7B","STB"));
    opcode.insert(pair<string,string>("7B","3/4"));
    opcode.insert(pair<string,string>("57","STCH"));
    opcode.insert(pair<string,string>("57","3/4"));
    opcode.insert(pair<string,string>("83","STF"));
    opcode.insert(pair<string,string>("83","3/4"));
    opcode.insert(pair<string,string>("D7","STI"));
    opcode.insert(pair<string,string>("D7","3/4"));
    opcode.insert(pair<string,string>("17","STL"));
    opcode.insert(pair<string,string>("17","3/4"));
    opcode.insert(pair<string,string>("7F","STS"));
    opcode.insert(pair<string,string>("7F","3/4"));
    opcode.insert(pair<string,string>("EB","STSW"));
    opcode.insert(pair<string,string>("EB","3/4"));
    opcode.insert(pair<string,string>("87","STT"));
    opcode.insert(pair<string,string>("87","3/4"));
    opcode.insert(pair<string,string>("13","STX"));
    opcode.insert(pair<string,string>("13","3/4"));
    opcode.insert(pair<string,string>("1F","SUB"));
    opcode.insert(pair<string,string>("1F","3/4"));
    opcode.insert(pair<string,string>("5F","SUBF"));
    opcode.insert(pair<string,string>("5F","3/4"));
    opcode.insert(pair<string,string>("97","SUBR"));
    opcode.insert(pair<string,string>("97","2"));
    opcode.insert(pair<string,string>("B3","SVC"));
    opcode.insert(pair<string,string>("B3","2"));
    opcode.insert(pair<string,string>("E3","TD"));
    opcode.insert(pair<string,string>("E3","3/4"));
    opcode.insert(pair<string,string>("FB","TIO"));
    opcode.insert(pair<string,string>("FB","1"));
    opcode.insert(pair<string,string>("2F","TIX"));
    opcode.insert(pair<string,string>("2F","3/4"));
    opcode.insert(pair<string,string>("BB","TIXR"));
    opcode.insert(pair<string,string>("BB","2"));
    opcode.insert(pair<string,string>("DF","WD"));
    opcode.insert(pair<string,string>("DF","3/4"));
}

string Optab::get_machine_code(string code) 
{
    code = to_uppercase(code);
	
	if (code == "+RSUB") throw opcode_error_exception("Cannot use '+' with RSUB");
	
	if (code.size() != 0 && code.at(0) == '+') code.erase(0, 1);
	
	if(m.find(code) == m.end()) {
		throw opcode_error_exception("Cannot find " + code);
	}
	else {
		m_iter = m.find(code);
	}
	
    return m_iter->second.first;
}

int Optab::get_instruction_size(string code)
{
	code = to_uppercase(code);
	
	bool plus = false;
	if (code.size() != 0 && code.at(0) == '+') {
		plus = true;
		code.erase(0, 1);
	}
	
	if(m.find(code) == m.end()) {
		throw opcode_error_exception("Cannot find " + code);
	}
	else {
		m_iter = m.find(code);
	}
	
	int format = m_iter->second.second;
	
	if (plus && code == "RSUB") throw opcode_error_exception("Cannot use '+' with RSUB");
	if (plus && format == 3) return 4;
	if (plus && format != 3) throw opcode_error_exception("Invalid use of '+' operator");
    return format;
}

void Optab::initMap( map<string, pair<string, int> >& theMap )
{
	int number_of_opcodes = 59;
	string mnemonics[] = {	"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF", "MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX", "SUB", "SUBF", "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD"};
	
	string opcode[] = {		"18",  "58",   "90",   "40",  "B4",    "28",   "88",    "A0",    "24",  "64",   "9C",   "C4",  "C0",    "F4",  "3C", "30", "34",  "38",  "48",   "00",  "68",  "50",   "70",  "08",  "6C", 	"74",  "04",  "D0",  "20",  "60",   "98",   "C8",   "44", "D8", "AC",  "4C",   "A4",     "A8",     "F0",  "EC",  "0C",  "78",  "54",   "80",  "D4",  "14",  "7C",  "E8",   "84",  "10",  "1C",  "5C",   "94", 	"B0",  "E0", "F8",  "2C",  "B8",   "DC"};
	
	int	sizes[] = {          3,     3,      2,      3,     2,       3,      3,       2,       3,     3,      2,      1,     1,       1,     3,    3,    3,     3,     3,      3,     3,     3,      3,     3,     3,     3,     3,     3,     3,     3,      2,      1,      3,    3,    2,     3,      2,        2,        1,     3,     3,     3,     3,      3,     3,     3,     3,     3,      3,     3,     3,     3,      2,      2,     3,    1,     3,     2,      3};
	
	for (int i = 0; i < number_of_opcodes; i++)
		theMap.insert(pair<string, pair<string,int> > (mnemonics[i],pair<string,int>(opcode[i],sizes[i])));
}

// CONVERT STRING TO ALL UPPERCASE
string Optab::to_uppercase(string s) {
    transform(s.begin(), s.end(), s.begin(),::toupper);
    return s;
}
