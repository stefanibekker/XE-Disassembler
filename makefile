# Stefani Bekker - cssc0424
# Sierra Morehardt - cssc0419
# Austin Aguiar - cssc0480
# CS530, Spring 2019
# Assignment 3 SIC/XE Disassembler
# makefile

OBJS = main.o symtab.o Optab.o printRecords.o convert.o file_parser.o bitMask.o 
CC = g++

xed: $(OBJS)
	$(CC) $(OBJS) -o xed

main.o: main.cpp Optab.hpp symtab.hpp printRecords.hpp convert.hpp file_parse_exception.hpp file_parser.hpp opcode_error_exception.hpp sicxe_asm.hpp symtab_exception.hpp
	$(CC) -c main.cpp

converter.o: convert.cpp convert.hpp
	$(CC) -c convert.cpp
   
Optab.o: Optab.cpp Optab.hpp opcode_error_exception.hpp
	$(CC) -c Optab.cpp

Symtab.o: symtab.cpp symtab.hpp symtab_exception.hpp
	$(CC) -c symtab.cpp

printRecords.o: printRecords.cpp printRecords.hpp
	$(CC) -c printRecords.cpp

file_parser.o: file_parser.cpp file_parser.hpp
	$(CC) -c file_parser.cpp

bitMask.o: bitMask.cpp bitMask.hpp
	$(CC) -c bitMask.cpp

clean:
	rm sample.sic sample.lis xed *.o


#xed: main.cpp convert.cpp printRecords.cpp bitMask.cpp file_parser.cpp Optab.cpp symtab.cpp
	#g++ main.cpp convert.cpp printRecords.cpp bitMask.cpp file_parser.cpp Optab.cpp symtab.cpp -o xed

#clean:
	#rm sample.sic sample.lis xed