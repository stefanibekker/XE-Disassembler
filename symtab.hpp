/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
symtab.hpp
*/
#ifndef SYMTAB_H
#define SYMTAB_H

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

class symtab {
private:
    multimap<std::string,std::string> symTab;
    multimap<std::string,std::string> litTab;
    multimap<std::string,std::string> regTab;
    map<string,pair<int,bool> > m;
	map<string, pair<int, bool> >::iterator m_iter;
	void initMap( map<string, pair<int, bool> >& theMap );
   
public:
    symtab(std::ifstream& inSym);
    string getRelSym(std::string inSym);
    string getAbsSym(std::string inSym);
    string getReverseRel(std::string inSym);
    pair<std::string, std::string> getLit(std::string litAdr);
    multimap<std::string, std::string> getSymTab();
    multimap<std::string, std::string> getAbsTab();
    string getReg(std::string regNum);
    symtab(); 
    int get_address(string);  
	bool get_type(string);
    void input_label(string, int, bool);
};

#endif