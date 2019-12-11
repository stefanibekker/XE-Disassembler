/* 
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
sicxe_asm.hpp
*/

#ifndef SICXE_ASM_H
#define SICXE_ASM_H
#include <map>
#include <vector>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;

class sicxe_asm {
    public:
        // Ctor
        // Takes in filename string and reads code line by line to detect errors and set addresses
        sicxe_asm(string);
		
    private:
        // your variables and private methods go here
		
		Optab opcode_lookup;
		symtab symbol;
		
		string filename;
		string program_name;
		string base;
		int base_set;
		int base_unset;
		
		int address;
		unsigned int current_line;
		
		bool on_second_pass;
		
		vector< vector<string> > dataStruct;
        
	// PROCEDURAL METHODS
		void first_pass(file_parser);
		void second_pass();
		void create_object_file();
		
		void print_list_file();
		
	// SUB PROCEDURAL METHODS
	
		// FIRST PASS
		
			void get_to_start(file_parser);
			void handle_start(file_parser);
			void handle_equ(string, string);
			void handle_end(file_parser);
			void handle_byte(file_parser);	
			void handle_word();	
			void handle_resb(file_parser);
			void handle_resw(file_parser);
			
		// SECOND PASS
			
			void handle_f1(string);
			void handle_f2(string);
			void handle_f3(string);
			void handle_f4(string);
			void handle_directive(string);
			
			// SUB SUB PROCEDURAL METHODS
			
				int handle_clear(int);
				int handle_shift(int);
				int handle_svc(int);
				int handle_tixr(int);
				int handle_defaultf2(int);
				
		// OBJECT CODE
		
	// HELPER METHODS
	
		string to_uppercase(string);
		
		int string_to_int(string);
		int hex_string_to_int(string);
		string int_to_hex_string(int);
		string int_to_string(int);
		
		string address_format(string);
		bool is_hex(string);
		string get_token(unsigned int, unsigned int);
		int handle_expression_token(string);
		int handle_constant(string);
		int get_register(char);
		
		void print_contents();
		
};

#endif