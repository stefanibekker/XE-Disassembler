/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
main.cpp
*/
#include "symtab.hpp"
#include "bitMask.hpp"
#include "calculateAddress.hpp"
#include "convert.hpp"
#include "file_parse_exception.hpp"
#include "file_parser.hpp"
#include "opcode_error_exception.hpp"
#include "Optab.hpp"
#include "printRecords.hpp"
#include "sicxe_asm.hpp"
#include "symtab.hpp"
#include "symtab_exception.hpp"

#include <algorithm>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

#define SET_N 0x20
#define SET_I 0x10
#define SET_X 0x8
#define SET_B 0x4
#define SET_P 0x2
#define SET_E 0x1

using namespace std;

string remove_spaces(const string &s){
    int last = s.size() - 1;
    while (last >= 0 && s[last] == ' ')
    --last;
    return s.substr(0, last + 1);
    }

// Runs second pass
void sicxe_asm::second_pass(){	
	
	base_set = -1;
	base_unset = -1;
	
	for (current_line = 5; current_line < dataStruct.size(); current_line++) {
		
		string opcode = to_uppercase(get_token(current_line, 3));
		
		try {
			int format = opcode_lookup.get_instruction_size(opcode);
			
			if (format == 1) {
				handle_f1(opcode);
			}
			else if (format == 2) {
				handle_f2(opcode);
			}
			else if (format == 3) {
				handle_f3(opcode);
			}
			else if (format == 4) {
				handle_f4(opcode);
			}
		}
		catch (opcode_error_exception e) {	// should only happen for directives
			handle_directive(opcode);
		}
		
	}
	
}

void sicxe_asm::print_list_file() {		// Creates .lis file

    string delimiter = ".";
    filename = filename.substr(0, filename.find(delimiter));
    filename.append(".lis");
	
    ofstream outfile;
	
	char *filenamecstr = new char[filename.length()+1];
	strcpy(filenamecstr, filename.c_str());
	
    outfile.open(filenamecstr, ios::out);

    vector< vector<string> >::iterator row;
    vector<string>::iterator col;

    for (row = dataStruct.begin(); row != dataStruct.end(); row++)
    {
        for (col = row -> begin(); col != row -> end(); col++)
        {
            outfile.fill(' ');
            outfile.width(15);
            outfile << left << *col << " ";
        }
        outfile << endl;
    }
	
    outfile.close();
}

// Finds start opcode
void sicxe_asm::get_to_start(file_parser parser) {
	
	while(to_uppercase(parser.get_token(current_line - 1, 1)) != "START") {
		if (parser.get_token(current_line - 1, 1) != " ")
			throw symtab_exception("First Opcode must be start");	// Checks to make sure first opcode is start
	
		vector<string> line(6, " ");
		line[0] = int_to_string(current_line);
		line[1] = address_format(to_uppercase(int_to_hex_string(address)));
		dataStruct.push_back(line);
		current_line++;
	}
}

// Handles the start line	
void sicxe_asm::handle_start(file_parser parser) {
	
	string operand = parser.get_token(current_line - 1, 2);
	
	program_name = parser.get_token(current_line - 1, 0);	//Gets program name
	
	if (operand[0] == '$') {					// Figures out where the address counter should start
		operand.erase(0, 1);
		if (operand.size() > 5) throw symtab_exception("Hex address out of range");
		address = hex_string_to_int(operand);
	}
	else address = string_to_int(operand);
	
	vector<string> line(6, " ");
	
	line[0] = int_to_string(current_line);
	line[1] = address_format(to_uppercase(int_to_hex_string(0)));
	line[2] = parser.get_token(current_line - 1, 0);
	line[3] = parser.get_token(current_line - 1, 1);
	line[4] = parser.get_token(current_line - 1, 2);
	
	dataStruct.push_back(line);
	current_line++;
}
	
void sicxe_asm::handle_equ(string label, string operand) {
	
	
	if (label == " ") throw symtab_exception("EQU requires label");
	if (operand == " ") throw symtab_exception("EQU requires operand");

	int address = 0;
	bool forward_reference = false;
	
	try {
		address = handle_expression_token(operand);
		if ((address < 0) || (address > 1048575)) throw opcode_error_exception("Improper Operand");
	}
	catch (symtab_exception e) {
		forward_reference = true;
	}
		
	if (!forward_reference)
		symbol.input_label(label, address, false);
	
}		

// Handles END directive and makes sure all lines after end are empty	
void sicxe_asm::handle_end(file_parser parser) {
	
	string operand = parser.get_token(current_line - 1, 2);
	
	if (operand != program_name) throw symtab_exception("Program names must match");
	
	vector<string> line(6, " ");
	
	line[0] = int_to_string(current_line);
	line[1] = address_format(to_uppercase(int_to_hex_string(address)));
	line[2] = parser.get_token(current_line - 1, 0);
	line[3] = parser.get_token(current_line - 1, 1);
	line[4] = parser.get_token(current_line - 1, 2);
	dataStruct.push_back(line);
	current_line++;
	
	for (int i = (current_line - 1); i < parser.size(); i++) {
		vector<string> line(6, " ");
	
		if ((parser.get_token(current_line - 1, 0) != " ") || (parser.get_token(current_line - 1, 1) != " ") || (parser.get_token(current_line - 1, 2) != " "))
			throw symtab_exception("Should not be anything but comments after END");
		
		line[0] = int_to_string(current_line);
		line[1] = address_format(to_uppercase(int_to_hex_string(address)));
		dataStruct.push_back(line);
		current_line++;
	}
	
}	

// Handles BYTE directive	
void sicxe_asm::handle_byte(file_parser parser) {
	
	string operand = parser.get_token(current_line - 1, 2);
	
	if (operand == " ") throw symtab_exception("Must have operand");
	
	if ((operand[0] == 'x') || (operand[0] == 'X')) {
		operand.erase(0, 2);
		if (operand.at(operand.length() - 1) == ' ') operand.erase(operand.length() - 1, 1);
		operand.erase(operand.length() - 1, 1);
		
		if((operand.length() & 1) == 1) throw symtab_exception("Need even number of characters");	// Checks to make sure there is an even number of hex values
		
		if (!is_hex(operand)) throw symtab_exception("Not a proper Hex value");
		
		address += operand.length() >> 1;
	}
	
	else if ((operand[0] == 'c') || (operand[0] == 'C')) {
		if (operand.at(operand.length() - 1) == ' ') operand.erase(operand.length() - 1, 1);
		address += (operand.length() - 3);	// Each character except the quotes and 'c' count as one byte
	}
	
	else {
		throw symtab_exception("Improper operand");	// Operand must start with c or x
	}
}	

// Handles WORD directive	
void sicxe_asm::handle_word() {
	
	address += 3;	// Word always increases by 3
}	

// Handles RESB directive	
void sicxe_asm::handle_resb(file_parser parser) {
	
	string operand = parser.get_token(current_line - 1, 2);
	
	if (operand == " ") throw symtab_exception("Must have operand");		// Checks to make sure there is an operand
	
	address += string_to_int(operand);
	if (address > 1048575) throw symtab_exception("Address too large");
}		

// Handles RESW directive	
void sicxe_asm::handle_resw(file_parser parser) {
	
	string operand = parser.get_token(current_line - 1, 2);
	
	if (operand == " ") throw symtab_exception("Must have operand");		// Checks to make sure there is an operand
	
	address += (3 * string_to_int(operand));
	if (address > 1048575) throw symtab_exception("Address too large");
}

// Makes machine_code for Format 1 opcodes
void sicxe_asm::handle_f1(string opcode) { 
	
	dataStruct.at(current_line).at(5) = opcode_lookup.get_machine_code(opcode);
	
}
	
// Makes machine_code for Format 2 opcodes	
void sicxe_asm::handle_f2(string opcode) { 
	
	int machine_code = 0;
	
	machine_code += hex_string_to_int(opcode_lookup.get_machine_code(opcode));
	machine_code <<= 4;
	
	if (opcode == "CLEAR") machine_code = handle_clear(machine_code);
	else if ((opcode == "SHIFTR") || (opcode == "SHIFTL")) machine_code = handle_shift(machine_code);
	else if (opcode == "SVC") machine_code = handle_svc(machine_code);
	else if (opcode == "TIXR") machine_code = handle_tixr(machine_code);
	else machine_code = handle_defaultf2(machine_code);
		
	dataStruct.at(current_line).at(5) = to_uppercase(int_to_hex_string(machine_code));
}
	
// Makes machine_code for Format 3 opcodes	
void sicxe_asm::handle_f3(string opcode) { 
	
	int machine_code = 0;
	int destination = 0;
	int offset;
	bool nix_checker = false;
	
	if (opcode == "RSUB") {
		dataStruct.at(current_line).at(5) = "4F0000";
		return;
	}
	
	string operand = get_token(current_line, 4);
	int address = hex_string_to_int(get_token(current_line, 1));
	
	machine_code += hex_string_to_int(opcode_lookup.get_machine_code(opcode));
	machine_code <<= 4;
	
	if (operand.at(0) == '@') { // Check if indirect
		machine_code |= SET_N;
		nix_checker = true;
		operand.erase(0, 1);
	}
	else if (operand.at(0) == '#') { // Check if Immediate
		machine_code |= SET_I;
		nix_checker = true;
		operand.erase(0, 1);
	}
	else { // If neither indirect or immediate set both
		machine_code |= SET_N;
		machine_code |= SET_I;
	}
	
	if ((operand.size() > 1) && ((operand.substr(operand.size() - 2, 2) == ",x") || (operand.substr(operand.size() - 2, 2) == ",X"))) { // Check if indexed
		machine_code |= SET_X;
		if (nix_checker) throw symtab_exception("Improper NIX bits");
		operand.erase(operand.size() - 2, 2);
	}
	
	if (operand.at(0) == '$') {
		offset = handle_constant(operand);
		
		if (offset > 4095) throw symtab_exception("Offset too high");
	}
	else if ((operand.at(0) == '-') || ((operand.at(0) >= '0') && (operand.at(0) <= '9'))) {
		offset = handle_constant(operand);
		
		if ((offset > 2047) || (offset < -2048)) throw symtab_exception("Offset too high or too low");
	}
	
	else {
	
		destination = handle_expression_token(operand);
		if (destination > 1048575) throw opcode_error_exception("Operand value too high");
		
		offset = destination - (address + 3);
	
		if ((offset <= 2047) && (offset >= -2048)) {
			machine_code |= SET_P;
		}
		
		else {
			machine_code |= SET_B;
			
			if (base_set == -1) throw symtab_exception("Offset too low or too high and no base");
			
			offset = destination - base_set;
			
			if ((offset < 0) || (offset > 4095)) throw symtab_exception("Base offset too high or negative");
		}
	}
	
	if (opcode == "LDB") { // Needs to be after calculating offset in case ldb wants to use base.
		base_set = handle_expression_token(operand);
		if (base_set > 1048575) throw opcode_error_exception("Operand value too high");
		if (base_set != base_unset) throw symtab_exception("BASE and LDB mismatch");
	}
	
	machine_code <<= 12;
	
	if (offset < 0) { // Offset is negative
		string result = int_to_hex_string(offset);
		offset = hex_string_to_int(result.substr(result.size() - 3, 3));
	}
	
	machine_code += offset;
	
	string s = to_uppercase(int_to_hex_string(machine_code));
	s = string(6-s.size(),'0').append(s);
	
	dataStruct.at(current_line).at(5) = s;
	
}
	
// Makes machine_code for Format 4 opcodes	
void sicxe_asm::handle_f4(string opcode) { 

	int machine_code = 0;
	int location;
	bool nix_checker = false;
	
	string operand = get_token(current_line, 4);
	
	machine_code += hex_string_to_int(opcode_lookup.get_machine_code(opcode));
	machine_code <<= 4;
	
	if (operand.at(0) == '@') { // Check if indirect
		machine_code |= SET_N;
		nix_checker = true;
		operand.erase(0, 1);
	}
	else if (operand.at(0) == '#') { // Check if Immediate
		machine_code |= SET_I;
		nix_checker = true;
		operand.erase(0, 1);
	}
	else { // If neither indirect or immediate set both
		machine_code |= SET_N;
		machine_code |= SET_I;
	}
	
	if ((operand.size() > 1) && ((operand.substr(operand.size() - 2, 2) == ",x") || (operand.substr(operand.size() - 2, 2) == ",X"))) { // Check if indexed
		machine_code |= SET_X;
		if (nix_checker) throw symtab_exception("Improper NIX bits");
		operand.erase(operand.size() - 2, 2);
	}

	machine_code |= SET_E;
	
	machine_code <<= 20;
	
	location = handle_expression_token(operand);
	
	if (location > 1048575) throw symtab_exception("Operand location too high");
	if (location < 0) throw symtab_exception("Operand must be positive");
	
	machine_code += location;		
	
	string s = to_uppercase(int_to_hex_string(machine_code));
	s = string(8-s.size(),'0').append(s);
	
	dataStruct.at(current_line).at(5) = s;		
}
	
// Handles directive opcodes	
void sicxe_asm::handle_directive(string opcode) {

	if ((opcode == " ") || (opcode == "RESB") || (opcode == "RESW") || (opcode == "START") || (opcode == "END")) return;
	else if (opcode == "BASE") {
		base_unset = handle_expression_token(get_token(current_line, 4));
		
		if ((base_unset < 0) || (base_unset > 1048575))throw symtab_exception("Improper Base Value");
	}
	else if (opcode == "NOBASE") {
		base_set = -1;
		base_unset = -1;
	}
	else if (opcode == "BYTE") { // Handles Byte to machine_code, expects correct input
	
		string operand = get_token(current_line, 4);
		string machine_code = "";
		
		if ((operand.at(0) == 'c') || (operand.at(0) == 'C')) {
			
			operand.erase(0,2);
			
			int i = 0;
			while (operand.at(i) != 39) {
				
				machine_code += int_to_hex_string(operand.at(i));
				i++;
			}
			
			dataStruct.at(current_line).at(5) = to_uppercase(machine_code);
		}
		else {
			
			operand.erase(0,2);
			
			if (operand.at(operand.length() - 1) == ' ') operand.erase(operand.length() - 1, 1);
			operand.erase(operand.length() - 1, 1);
			
			if (!is_hex(operand)) throw symtab_exception("Operand not in hex");
			
			dataStruct.at(current_line).at(5) = to_uppercase(operand);
		}
		
	}
	else if (opcode == "WORD") {
	
		string operand = get_token(current_line, 4);
		int temp = handle_constant(operand);
	
		if (temp > 16777215) throw symtab_exception("Given value too high");
		
		string s = to_uppercase(int_to_hex_string(temp));
	
		if (temp >=0)
			s = string(6-s.size(),'0').append(s);
		else s.erase(0, 2);
	
		dataStruct.at(current_line).at(5) = s;
	}
	else if (opcode == "EQU") {
		string label = get_token(current_line, 2);
		string operand = get_token(current_line, 4);
		
		try {
			symbol.get_address(label);
		}
		catch (symtab_exception e) {	
			handle_equ(label, operand);
		}
		
	}		
}

// Handles formatting the machine code for CLEAR instruction where its operand is given as R1 and second register is set to 0	
int sicxe_asm::handle_clear(int machine_code) {
	
	string operand = get_token(current_line, 4);
	
	if (operand.size() != 1) throw symtab_exception("Improper operand");
	
	machine_code += get_register(operand.at(0));
	machine_code <<= 4;
	
	return machine_code;
}
	
// Handles both left and right SHIFT format 2 instructions where its operand is given as R1,n
// n - 1 is put into the second register	
int sicxe_asm::handle_shift(int machine_code) {
	
	string operand = get_token(current_line, 4);
	
	if (operand.size() < 3) throw symtab_exception("Improper operand");
	
	machine_code += get_register(operand.at(0));
	machine_code <<= 4;
	
	operand.erase(0,1);
	if (operand.at(0) != ',') throw symtab_exception("Improper operand"); // Makes sure second character is a comma
	operand.erase(0,1);
	
	int n = string_to_int(operand) - 1;
	if (n < 0) throw symtab_exception("Integer in operand too small");
	if (n > 15) throw symtab_exception("Integer in operand too large");
	machine_code += n;
	
	return machine_code;
}

// Handles formatting the machine code for SVC instruction where its operand is an integer between 0 and 15
// The second register is set to 0;	
int sicxe_asm::handle_svc(int machine_code) {
	
	string operand = get_token(current_line, 4);
	
	int n = string_to_int(operand);
	if (n < 0) throw symtab_exception("Integer in operand too small");
	if (n > 15) throw symtab_exception("Integer in operand too large");
	machine_code += n;
	
	machine_code <<= 4;
	
	return machine_code;
}

// Handles formatting the machine code for TIXR instruction where its operand is given as R1 and second register is set to 0	
int sicxe_asm::handle_tixr(int machine_code) {
	
	string operand = get_token(current_line, 4);
	
	if (operand.size() != 1) throw symtab_exception("Improper operand");
	
	machine_code += get_register(operand.at(0));
	machine_code <<= 4;
	
	return machine_code;
}

// Handles formatting the machine code for typical format 2 instructions where its operand is given as R1,R2
int sicxe_asm::handle_defaultf2(int machine_code) {
	
	string operand = get_token(current_line, 4);
	
	if (operand.size() != 3) throw symtab_exception("Improper operand");
	
	machine_code += get_register(operand.at(0)); // putting first register into machine_code
	machine_code <<= 4;
	
	operand.erase(0,2);
	
	machine_code += get_register(operand.at(0)); // putting second register into machine_code
	
	return machine_code;
}

// Converts string to upper_case
string sicxe_asm::to_uppercase(string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

// Converts string to int
int sicxe_asm::string_to_int(string s) {
	for (unsigned int i = 0; i < s.length(); i++)
		if ((s[i] < '0') || (s[i] > '9')) 
			throw symtab_exception("int must contain 0-9");
	
	istringstream instr(s);
	int n;
	instr >> n;
	return n;
}

// Convert hex string to int
int sicxe_asm::hex_string_to_int(string s) {
	
	if (!is_hex(s)) throw symtab_exception("Hex must contain 0-9, a-f, or A-F");
	
	char *scstr = new char[s.length()+1];
	strcpy(scstr, s.c_str());
	
	int value = strtoul(scstr, 0, 16);
	
	return value;
}

// Convert int to hex string
string sicxe_asm::int_to_hex_string(int integer) {
	stringstream sstream;					// start string stream
	sstream << hex << integer;			// convert int to hex
	string hex_string = sstream.str();	// convert result to string

	return hex_string;
}

// Convert int to string
string sicxe_asm::int_to_string(int integer) {
		
		ostringstream convert;
		convert << integer;
		string result = convert.str();
		
		return result;
}

// Formats string of hex to be five characters long for use in displaying information.
string sicxe_asm::address_format(string s) {
	
	if (s.size() > 5) {
		throw symtab_exception("Hex address out of range");
	}

	// turn into size of 5-characters
	string result = string(5-s.size(),'0').append(s);
	return result;
}

// Checks if string is a hex value
bool sicxe_asm::is_hex(string s) {
	for (unsigned int i = 0; i < s.length(); i++)
		if ((s[i] < '0') || ((s[i] > '9') && (s[i] < 'A')) || ((s[i] > 'F') && (s[i] < 'a')) || (s[i] > 'f'))
			return false;
	return true;
}

// Ease of use method for accessing elements in the dataStruct
string sicxe_asm::get_token(unsigned int row, unsigned int column) // get tokens in main data structure
{
	return dataStruct.at(row).at(column);
}

// Takes a token string from an operand and returns its value
int sicxe_asm::handle_expression_token(string operand) {
	
	int result;
	
	if (operand.size() == 0) throw opcode_error_exception("Improper operand");
	
	if ((operand.at(0) == '$') || ((operand.at(0) >= '0') && (operand.at(0) <= '9')) || (operand.at(0) == '-'))
		return handle_constant(operand);
	
	else if (((operand.at(0) >= 'A') && (operand.at(0) <= 'Z')) || ((operand.at(0) >= 'a') && (operand.at(0) <= 'z'))) {
		
		result = symbol.get_address(operand);
	}
	
	else throw opcode_error_exception("Improper operand");	
	
	return result;
}

// Handles a constant token taken from an operand and spits out whatever value it holds as an integer
int sicxe_asm::handle_constant(string s) {
	int result;
	
	if (s.substr(0,2) == "-$") throw symtab_exception("Can't make hex negative by placing - in front of it");
	
	if (s.at(0) == '$') {
		s.erase(0,1);
		
		result = hex_string_to_int(s);
	}
	
	else if (s.at(0) == '-') {
		s.erase(0,1);
		
		result = ~handle_constant(s) + 1;
	}
	
	else if ((s.at(0) >= '0') && (s.at(0) <= '9')) {
		result = string_to_int(s);
		
	}
	else throw opcode_error_exception("Not a valid constant");
	
	return result;
}

// Simple method that returns the proper integer that corresponds to registers A, X, S, and T
int sicxe_asm::get_register(char letter) {
		
	if ((letter == 'a') || (letter == 'A')) return 0;
	if ((letter == 'x') || (letter == 'X')) return 1;
	if ((letter == 's') || (letter == 'S')) return 4;
	if ((letter == 't') || (letter == 'T')) return 5;
		
	throw symtab_exception("Improper register");
}

// Prints contents to terminal the same way the .lis file would, For testing purposes
void sicxe_asm::print_contents() {
	
	vector< vector<string> >::iterator row;
	vector<string>::iterator col;
	
	for (row = dataStruct.begin(); row != dataStruct.end(); row++)
	{
	    for (col = row->begin(); col != row->end(); col++)
		{
			cout.fill(' ');
			cout.width(15);	
			cout << left << *col << " ";
	    }
		cout << endl;
	}
}

int main(int argc, char* argv[]) {

	vector<string> modRec;
    string baseAddress;
	string baseTxtLine;
	string equLine;
	string headRec;
	string literal = "";
	string litLength = "";
	string modLine;
	string operand;
	string testEQU;
	string textRecord;
    string textRecordAddress;
	map<string,string> baseTxtRec;
	Optab thisOpTab;
	list<string> addresses;
	int tempCounter = 0;

    // Open and Check for Input File
    if(argc <= 1 || argc > 2) {

        cout << "Usage: dasm <filename>.obj" << endl;
        exit(1);
    }

    string inFile = argv[1];
    int extPosition = inFile.find_last_of(".");

    // Error Checking
    if(inFile.substr(extPosition+1) != "obj") {

        cout << "Wrong file extension!" << endl;
        exit(1);
    }

    ifstream objFile(argv[1], ios::in);
    
    if(!objFile) {

        cout << "File not found!" << endl;
        exit(1);
    }

    if(objFile.peek() == istream::traits_type::eof()) {
        
        cout << "File is empty!" << endl;
        exit(1);
    }

    string fileName = inFile.substr(0,extPosition);
    ifstream symFile((fileName + ".sym").c_str(), ios::in);

    if(!symFile) {

        cout << "File not found!" << endl;
        exit(1);
    }
    
    if(symFile.peek() == istream::traits_type::eof()) {
        
        cout << "File is empty!" << endl;
        exit(1);
    }
    
    ofstream sicFile((fileName + ".sic").c_str());
    
    symtab thisSymtab(symFile);

    // Retrieve Mod Record to Know if Operand is an Address or a Constant
    while(getline(objFile,modLine)) {

        if(modLine[0] != 'M')
            continue;
        modRec.push_back(modLine.substr(1,6));
    }

    objFile.clear();
    objFile.seekg(0,objFile.beg);

    // Retrieve Text Record for Finding LDB
    while(getline(objFile,baseTxtLine)) {

        if(baseTxtLine[0] != 'T')
            continue;
        baseTxtRec.insert(pair<string,string>(baseTxtLine.substr(1,6),baseTxtLine.substr(9)));
    }

    objFile.clear();
    objFile.seekg(0,objFile.beg);

    // Load Text Record for Finding EQU
    while(getline(objFile,equLine)) {

        if(equLine[0] != 'T')
            continue;
        testEQU = testEQU + equLine.substr(9);
    }

    objFile.clear();
    objFile.seekg(0,objFile.beg);
    
    // Print Header Record
    getline(objFile,headRec);
    string label = headRec.substr(1,6);
    string programName = label;
    string beginAdr = headRec.substr(7,6);
    string progLength = headRec.substr(13,6);
    printHeaderRec(programName,beginAdr,sicFile);

    multimap<string,string> absMap = thisSymtab.getAbsTab();
    int pos = 0;
    int tempPos = pos;
    multimap<string,string>::iterator equIt;

    // Parse EQU by Loading All Absolute Symbols and Removing Any Which Show Up in Object Code
    while(pos < testEQU.size()) {
        
        int equCount = 1;

        // Stop Checking if Moving Past End
        try {

            // Check Up to Three Bytes
            while((equIt = absMap.find(testEQU.substr(pos,2))) == absMap.end()) {    
                
                if(pos < 4)
                    break;
                pos += 2;
            }

             // Check Up to Three Consecutive Bytes
            if(equIt == absMap.end())
                while((equIt = absMap.find(testEQU.substr(tempPos,equCount*2))) == absMap.end()) {
                    
                    if(equCount < 3)
                        break;
                    ++equCount;
                }
                
        } catch(out_of_range& e) {

            break;
        }

        pos += equCount*2;
        tempPos = pos;

        if(equIt != absMap.end())
            absMap.erase(equIt);
    }

    // Set Remaining Absolute Symbols to EQU
    for(multimap<string,string>::iterator it = absMap.begin(); it != absMap.end(); ++it)
        printTextRecord(it -> second, "EQU", it -> first, sicFile, false, false, false, false);
    
    string lastTxtRec = beginAdr;
    string operandMnemonic = ""; 
    
    // Read Every Text Record
    while(getline(objFile,textRecord)) {
        
        // Check for RESB/W at the Last Record
        if(textRecord[0] != 'T') {

            textRecordAddress = progLength;

            if(lastTxtRec.compare(textRecordAddress) != 0)
                printRes(thisSymtab,lastTxtRec,textRecordAddress,sicFile);

            break;
        }

        int counter = 9;
        textRecordAddress = textRecord.substr(1,6);
        string textRecordLength = textRecord.substr(7,2);
        string loc = textRecordAddress;
        int maxLoc = textRecord.length();
        
        // RESB,RESW at End of Record
        if(lastTxtRec.compare(textRecordAddress) != 0)
            printRes(thisSymtab,lastTxtRec,textRecordAddress,sicFile);

        lastTxtRec = incrementAddress(textRecordAddress,textRecordLength,0);
        
        // Stop at Last Character in Object File
        while(counter < maxLoc) {

            try{

                label = "";
                string relSym = thisSymtab.getRelSym(loc);

                // Check if Symbol or RESB/W
                if(relSym.compare("") != 0) {
                    
                    // RESB/W
                    if((textRecord.substr(counter,2)).compare("00") == 0) {   

                        int dataCount = 1;
                        string resData = textRecord.substr(counter, dataCount*2);
                        string tempResData = resData;

                        // Check Until a Non-Zero is Found
                        while((tempResData.erase(0,tempResData.find_first_not_of('0'))).compare("") == 0) {

                            ++dataCount;
                            resData = textRecord.substr(counter, dataCount*2);
                            tempResData = resData;
                        }

                        --dataCount;

                        // Determine Exact Size of Data Reserved
                        if(dataCount % 3 == 0){
							printTextRecord(relSym, "RESW", intToString(dataCount/3), sicFile, false, false, false, false);
						}
                        
                        else{
							printTextRecord(relSym, "RESB", intToString(dataCount), sicFile, false, false, false, false);
						}
                            
                        
                        loc = incrementAddress(loc,intToString(dataCount), 0);
                        counter += dataCount * 2;
                        continue;
                    }

                    // Instruction
                    else                            
                        label = relSym;
                }

                pair<string,string> litData = thisSymtab.getLit(loc);

                // Check for Literal
                if((litData.first).compare("") != 0) {
                    litLength = litData.second;
                    literal = litData.first;
					
                     // Hex Literal
                    
                    if((literal.substr(1,1)).compare("X") == 0) {
					  tempCounter = counter;

                        counter += stringToInt(litLength);
                        loc = incrementAddress(loc,"1", 0);
                    }
                    // Character Literal
                    else {
						tempCounter = counter;

                        counter += stringToInt(litLength) * 2;
                        loc = incrementAddress(loc,litLength, 0);
                    }

                    printTextRecord("", ".LTORG", "", sicFile, false, false, false, false);
                    continue;
                }

                // Determine Instruction
                bool eFlag = false;
                bool nFlag = false;
				bool xFlag = false;
                bool iFlag = false;
                bool isInstruction = false;
				string opCode = "";

				if(tempCounter != 0){
					opCode = textRecord.substr(tempCounter,2);
					tempCounter = 0;
				} else {
					opCode = textRecord.substr(counter,2);
				}

                string format = "";
                // Get Opcode by Masking Last 2 Bits
                pair<string,string> opData = thisOpTab.getOp(maskOpBit(opCode));
                string tempAdr = "";

                if((opData.first).compare("") != 0) {
					//Parse Instruction
                    isInstruction = true;
                    operandMnemonic = opData.first;
                    format = opData.second;
                    // Format One
                    if(format.compare("1") == 0)
                        operand = "";
                    // Format Two
                    else if(format.compare("2") == 0) {

                        string firstRegNum = textRecord.substr(counter + 2, 1);
                        string secondRegNum = textRecord.substr(counter + 3, 1);

                        if(secondRegNum.compare("0") == 0)
                            operand = thisSymtab.getReg(firstRegNum);
                        else
                            operand = thisSymtab.getReg(firstRegNum) + "," + thisSymtab.getReg(secondRegNum);
                        if(operand.compare("") == 0 || operand.compare(",") == 0)
                            isInstruction = false;
                    }
					//Format Three
                    else {        
                        string niFlags = hexToBin(opCode.substr(1,1));
                        string xbpeFlags = hexToBin(textRecord.substr(counter+2,1));
                        pair<string,string> symName("","");
                        if(niFlags[2] == '1' && niFlags[3] == '1') {
                            if(xbpeFlags[0] == '1')
                                xFlag = true;
                        }
						//Immediate
                        else if(niFlags[2] == '0' && niFlags[3] == '1')     
                            iFlag = true;
						//Indirect
                        else if(niFlags[2] == '1' && niFlags[3] == '0')     
                            nFlag = true;
						//Extended
                        if(xbpeFlags[1] == '0' && xbpeFlags[2] == '0' && xbpeFlags[3] == '1') {
                            tempAdr = textRecord.substr(counter+3,5);
                            eFlag = true;
                        }
						//Program Counter
                        else if(xbpeFlags[1] == '0' && xbpeFlags[2] == '1' && xbpeFlags[3] == '0') {
                            tempAdr = textRecord.substr(counter+3,3);
                            tempAdr = incrementAddress(loc,tempAdr,3);
                        }
						//Base
                        else if(xbpeFlags[1] == '1' && xbpeFlags[2] == '0' && xbpeFlags[3] == '0') {
                            tempAdr = textRecord.substr(counter+3,3);
                        }
						//Direct
                        else if(xbpeFlags[1] == '0' && xbpeFlags[2] == '0' && xbpeFlags[3] == '0'){
                            tempAdr = textRecord.substr(counter+3,3);
                        }        
                        else
                            isInstruction = false;
                        //Literal
                        symName = thisSymtab.getLit(tempAdr);
						//Symbol
                        if((symName.first).compare("") == 0) {
                            symName.first = thisSymtab.getRelSym(tempAdr);
							//Constant
                            if((symName.first).compare("") == 0)
                                symName.first = tempAdr;
                        }
                        operand = symName.first;
                        //Check Mod Record, if Immediate, for Constant or Address
                        if(iFlag && xbpeFlags[1] != '1' && xbpeFlags[2] != '1') {
                            if(find(modRec.begin(),modRec.end(),incrementAddress(loc,"1",0)) == modRec.end())
                                operand = hexToDec(tempAdr);
                        }
                    }
                }
                //Check if BYTE/WORD
                if(!isInstruction) {
                    int count = 1;
                    string dataValue="";
                    string dataSym="";
					//Check Up to Three Consecutive Bytes
                    while(count <= 3) {    
                        dataValue = textRecord.substr(counter,count*2);
                        dataSym = thisSymtab.getAbsSym(dataValue);
                        ++count;
                    }
                    if(dataSym.compare("") != 0) {
                        if(count==3){
							printTextRecord(dataSym,"WORD",dataValue,sicFile,false,false,false,false);
						}
                            
                        else{
							printTextRecord(dataSym,"BYTE",dataValue,sicFile,false,false,false,false);
						}
                            
                        loc = incrementAddress(loc,intToString(count),0);
                        counter += count*2;
                        continue;
                    }
					//If Byte Can't be Determined, It's an Error from Guessing
                    else {
                        sicFile << "NULL    " << "NULL    " << "NULL" << endl;
                        counter += 2;
                        loc = incrementAddress(loc,"1",0);
                    }
                }
				//Format Four
                if(format.size() != 1 && eFlag)
                    format = format.substr(2,1);
				//Format 1 & 2
                else
                    format = format.substr(0,1);
                //Increment for Next Parsing Location
                loc = incrementAddress(loc,format,0);
                counter += stringToInt(format)*2;
                
                if(operandMnemonic.compare("RSUB") == 0)
                    operand = "";

                    operand = remove_spaces(operand);
					if(literal != ""){
						operand = literal;
					}
                printTextRecord(label,operandMnemonic,operand,sicFile,xFlag,eFlag,nFlag,iFlag);
				literal = "";
				litLength = "";
                //Load Base Register
                if(operandMnemonic.compare("LDB") == 0) {
                    baseAddress = calculateBase(tempAdr,baseTxtRec,iFlag);
                    if(nFlag)
                        baseAddress = calculateBase(baseAddress,baseTxtRec,false);
						
                    printTextRecord("","BASE",operand,sicFile,false,false,false,false);
                }
                 std::list<string>::iterator i = addresses.begin();
                 i++;   
            }
            catch(out_of_range& e) {
                sicFile << "NULL    " << "NULL    " << "NULL" << endl;
                break;
            }
        }
    }

    //End Record
    while(textRecord[0]!='E')
        getline(objFile,textRecord);
    string endRec = textRecord;
    printEndRec(endRec,sicFile,programName);

	sicxe_asm compileProgram("sample.sic"); 
    symFile.close();
    objFile.close();
    sicFile.close();

    return 0;
}

sicxe_asm::sicxe_asm(string file) {
		
		filename = file;
		on_second_pass = false;
	
	try {
	
		file_parser parser(file); 	// creates file data_struct with separated tokens and minor error checking.
		parser.read_file();
		first_pass(parser);
		on_second_pass = true;
		second_pass();
		print_list_file();		// creates .lis file
	}
	
	catch (file_parse_exception e) {	// catches and prints file_parser errors
		cout << e.getMessage() << endl;
	}
	catch (opcode_error_exception e) {	// catches and prints Optab errors
		if (on_second_pass) current_line -= 4; // needed for handling expressions, sytab exception interferes with handle_equ()
		string error = ("ERROR: Line " + int_to_string(current_line) + ": " + e.getMessage());
		cout << error << endl;
	}
	catch (symtab_exception e) {		// catches and prints symtab errors as well as bulk of exceptions in sicxe_asm.cpp, had trouble with generic exception usage and wanted to focus on the program itself.
		if (on_second_pass) current_line -= 4;
		string error = ("ERROR: Line " + int_to_string(current_line) + ": " + e.getMessage());
		cout << error << endl;
	}
	
	
	
}

// Runs first pass
void sicxe_asm::first_pass(file_parser parser) {
		
	current_line = 1;			// current line being checked
	address = 0;				// current address in int, convert to hex string for output
	
	get_to_start(parser);		// moves lines along until start opcode is reached
	handle_start(parser);		// handles the start line
		
	for (int i = (current_line - 1); i < parser.size(); i++) {	// go through line by line
		
		vector<string> line(6, " ");
		line[0] = int_to_string(current_line);				//line
		line[1] = address_format(to_uppercase(int_to_hex_string(address)));	//address
		
		string opcode = to_uppercase(parser.get_token(current_line - 1, 1));	// makes opcode uppercase to generalize if statements
		
		if (opcode == "EQU") {		// Handles line when opcode is EQU
			
			string label = parser.get_token(current_line - 1, 0);
			string operand = parser.get_token(current_line - 1, 2);
			
			handle_equ(label, operand);
		}
		
		else if (opcode == "END") {	// Handles line when opcode is END, breaks the for loop, all other lines handled by handle_end() method
			handle_end(parser);
			break;
		}
		
		else {
			
			if (parser.get_token(current_line - 1, 0) != " ")		// if there is a label it adds it to the symtable
				symbol.input_label(parser.get_token(current_line - 1, 0), address, false);
			
			if ((opcode == " ") || (opcode == "BASE") || (opcode == "NOBASE"));									// Do nothing with line if no opcode, Base is handled and used in pass 2
			else if (opcode == "BYTE") handle_byte(parser);		// Handles line when opcode is BYTE
			else if (opcode == "WORD") handle_word();		// Handles line when opcode is WORD
			else if (opcode == "RESB") handle_resb(parser);		// Handles line when opcode is RESB
			else if (opcode == "RESW") handle_resw(parser);		// Handles line when opcode is RESW
			else address += opcode_lookup.get_instruction_size(opcode);					// If not a directive, look up the opcodes format and add it to the address
		}
		
		line[2] = parser.get_token(current_line - 1, 0);	//label
		line[3] = parser.get_token(current_line - 1, 1);	//opcode
		line[4] = parser.get_token(current_line - 1, 2);	//operand
		
		dataStruct.push_back(line);
		current_line++;
	}
}