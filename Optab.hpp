/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
Optab.hpp
*/
#ifndef OPTAB_H
#define OPTAB_H

#include <map>
#include <string>
#include <utility>
#include <sstream>

using namespace std;

class Optab {
private:
    multimap<std::string,std::string> opcode;
    map<string,pair<string,int> > m;
	map<string, pair<string, int> >::iterator m_iter;
	void initMap( map<string, pair<string, int> >& theMap );
    string to_uppercase(string);
    
public:
    Optab();
    pair<std::string,std::string> getOp(std::string opCode);
    string get_machine_code(string);  
    int get_instruction_size(string);
};

#endif
