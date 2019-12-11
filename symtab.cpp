/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
symtab.cpp
*/
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <limits>

#include "symtab.hpp"
#include "symtab_exception.hpp"

using namespace std;
//creates a copy of the symTab
multimap<string,string> symtab::getSymTab() { 
    multimap<string,string> symbolMap;
    for(multimap<string,string>::iterator i=symTab.begin();i!=symTab.end();++i)
        symbolMap.insert(pair<string,string>(i->first,i->second));
    return symbolMap;
}

//name of relative symbol
string symtab::getRelSym(string input) { 
    stringstream inputSymbol;
    inputSymbol << setfill('0') << setw(6) << input;
    string newInSym = inputSymbol.str();
    pair<multimap<string,string>::iterator,multimap<string,string>::iterator> iter;
    iter = symTab.equal_range(newInSym);
    string symName="";
    multimap<string,string>::iterator tempI;
    for(multimap<string,string>::iterator i=iter.first;i!=iter.second;++i) {
        tempI = i;
        if(((++i)->second).compare("R") == 0) {
            symName = tempI->second;
            break;
        }
    }
    return symName;
}

//creates a seperate table of only absolute symbols
multimap<string,string> symtab::getAbsTab() {
    multimap<string,string> absoluteMap;
    multimap<string,string>::iterator tempI;
    for(multimap<string,string>::iterator i=symTab.begin();i!=symTab.end();++i) {
        tempI = i;
        if(((++i)->second).compare("A") == 0)
            absoluteMap.insert(pair<string,string>(tempI->first,tempI->second));
    }
    return absoluteMap;
    
}

//loads symbols from the input file
symtab::symtab(ifstream& input) {  
    input.ignore(numeric_limits<streamsize>::max(), '\n');
    input.ignore(numeric_limits<streamsize>::max(), '\n');
    string symbol;
    while(getline(input,symbol)) {
        if(symbol.empty()) {
            input.ignore(numeric_limits<streamsize>::max(), '\n');
            input.ignore(numeric_limits<streamsize>::max(), '\n');
            //Literal
            while(getline(input,symbol)) { 
                if(symbol.empty())
                    break;
                string literalAddress = symbol.substr(24,6);
                litTab.insert(pair<string,string>(literalAddress,symbol.substr(8,7)));
                litTab.insert(pair<string,string>(literalAddress,symbol.substr(19,1)));
            }
            break;
        }

        //symbol
        string symbolAddress = symbol.substr(8,6); 
        symbolAddress.erase(remove(symbolAddress.begin(),symbolAddress.end(),' '),symbolAddress.end());
        symTab.insert(pair<string,string>(symbolAddress,symbol.substr(0,6)));
        symTab.insert(pair<string,string>(symbolAddress,symbol.substr(16,1)));
    }
    //insert registers into table
    regTab.insert(pair<string,string>("0","A")); 
    regTab.insert(pair<string,string>("1","X"));
    regTab.insert(pair<string,string>("2","L"));
    regTab.insert(pair<string,string>("3","B"));
    regTab.insert(pair<string,string>("4","S"));
    regTab.insert(pair<string,string>("5","T"));
    regTab.insert(pair<string,string>("6","F"));
    regTab.insert(pair<string,string>("8","PC"));
    regTab.insert(pair<string,string>("9","SW"));
}

//get the name of absolute symbol
string symtab::getAbsSym(string input) {
    pair<multimap<string,string>::iterator,multimap<string,string>::iterator> iter;
    iter = symTab.equal_range(input);
    string symbolName="";
    multimap<string,string>::iterator tempI;
    for(multimap<string,string>::iterator i=iter.first;i!=iter.second;++i) {
        tempI = i;
        if(((++i)->second).compare("A") == 0) {
            symbolName = tempI->second;
            break;
        }
    }
    return symbolName;
}
//get the name of literal and its address
pair<string,string> symtab::getLit(string literalAddress) {
    if(litTab.count(literalAddress) == 0)
        return make_pair("","");
    if(literalAddress.size() != 6) {
        stringstream newLiteralAdr;
        newLiteralAdr << setfill('0') << setw(6) << literalAddress;
        literalAddress = newLiteralAdr.str();
    }
    pair<multimap<string,string>::iterator,multimap<string,string>::iterator> iter;
    iter = litTab.equal_range(literalAddress);
    string literal;
    string literalLength;
    pair<string,string> literalData;
    multimap<string,string>::iterator i = iter.first;
    literal = i->second;
    i++;
    literalLength = i->second;
    literalData = make_pair(literal,literalLength);
    return literalData;
}
//get the last relative symbol of the same address(for ORG)
string symtab::getReverseRel(string input) {
    pair<multimap<string,string>::iterator,multimap<string,string>::iterator> iter;
    iter = symTab.equal_range(input);
    multimap<string,string> tempMap;
    for(multimap<string,string>::iterator ri=iter.first;ri!=iter.second;++ri)
        tempMap.insert(pair<string,string>(ri->first,ri->second));
    multimap<string,string>::reverse_iterator ri2;
    for(ri2=tempMap.rbegin();ri2!=tempMap.rend();++ri2) {
        if((ri2++->second).compare("R") == 0) {
            return ri2->second;
            break;
        }
    }
    return NULL;
}

//get name of register
string symtab::getReg(string registerNumber) {
    multimap<string,string>::iterator i = regTab.find(registerNumber);
    if(i == regTab.end())
        return "";
    return i->second;
}

symtab::symtab() {
	
}

int symtab::get_address(string label) {
	
	if(m.find(label) == m.end()) {
		throw symtab_exception("Cannot find " + label);
	}
	
	else {
		m_iter = m.find(label);
	}
	
    return m_iter->second.first;
}

bool symtab::get_type(string label) {
	
	if(m.find(label) == m.end()) {
		throw symtab_exception("Cannot find " + label);
	}
	
	else {
		m_iter = m.find(label);
	}
	
	return m_iter->second.second;
}

void symtab::input_label(string label, int address, bool absolute) {
	// makes label only 8 characters
	if(label.length() > 8)	
		label.erase(7, label.length() - 7);
	
	if(m.find(label) == m.end()) {
		m.insert(pair<string, pair<int,int> > (label,pair<int,int>(address,absolute)));
	}
	
	else {
		throw symtab_exception("Cannot add multiple labels of the same name.");
	}
}

