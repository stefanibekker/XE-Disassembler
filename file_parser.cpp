/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
file_parser.cpp
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>

#include "file_parser.hpp"
#include "file_parse_exception.hpp"

using namespace std;

file_parser::file_parser(string instring)
{
	filename = instring;
	lines = 0;
}

file_parser::~file_parser()
{
	
}

void file_parser::read_file()
{
	ifstream infile;
	
	char *filenamecstr = new char[filename.length()+1];
	strcpy(filenamecstr, filename.c_str());
	
    infile.open(filenamecstr, ios::in);
    if(!infile)
	{
        throw file_parse_exception("Cannot open file " + filename);
    }
    else
	{
        string data;
        while(getline(infile, data))
		{
            dataStruct.push_back(tokenizer(data));
            lines++;
        }
    }
	infile.close();
}

string file_parser::get_token(unsigned int row, unsigned int column)
{
	return dataStruct.at(row).at(column);
}

void file_parser::print_file()
{
	vector< vector<string> >::iterator row;
	vector<string>::iterator col;
	
	for (row = dataStruct.begin(); row != dataStruct.end(); row++)
	{
	    for (col = row->begin(); col != row->end(); col++)
		{
			cout.fill(' ');
			cout.width(15);	
			cout << left << *col << " "; //Might not need the last space here
	    }
		cout << endl;
	}
}

int file_parser::size()
{
	// return dataStruct.size();
	return lines;
}

vector<string> file_parser::tokenizer(string target)
{
	vector<string> quotes;					// Storage for any quotes found
	vector<string> temp;					// Storage for Tokens
	vector<string> tokenVector(4, " ");		// Default blank slate vector to return
	string delimiters = " \t\n";			// Delimiters to separate tokens
	
	string error;							// Converts lines to string for errors
	ostringstream convert;
	convert << (lines + 1);
	error = convert.str();
	
	if((target.size() == 0) || (target.at(0) == 10) || (target.at(0) == 13))		// string is empty
		return tokenVector;
	if(target.at(0) == '.') 								// entire string is a comment
	{
		tokenVector[3] = target;
		return tokenVector;
	} 
	
//Check Label===========================================================================================================================================
	if((target.at(0) != '\t') && (target.at(0) != ' ')) // checks to see if there is a label
	{
		if (!((target.at(0) <= 'z' && target.at(0) >= 'a') || (target.at(0) <= 'Z' && target.at(0) >= 'A'))) // checks to make sure first character is a letter
		{
			
			throw file_parse_exception("Line " + error + ": Label must start with letter");
		}		

		int last = target.find_first_not_of(delimiters,0);		// makes label a token to be separated
		int first = target.find_first_of(delimiters, last);

		string token = target.substr(last, first - last);
		
		vector<string>::iterator labelIterator;
		
		for (labelIterator = labels.begin(); labelIterator != labels.end(); labelIterator++) // checks for duplicates
		{
			if(token.compare(*labelIterator) == 0)
				throw file_parse_exception("Line " + error + ": Duplicate labels");
		}
		
		for (unsigned int i = 0; i < token.length(); i++) // checks for improper characters
		{
			if (!((token[i] >= 'a' && token[i] <= 'z') || (token[i] >= 'A' && token[i] <= 'Z') || (token[i] >= '0' && token[i] <= '9')))
			{
				throw file_parse_exception("Line " + error + ": Label can only contain 'a-z', 'A-Z', or '0-9'.");
			}
		}
		
		tokenVector[0] = token; // put the label in the first column
		
		labels.push_back(token); // put the label in the vector<string> labels
		
		target.erase(last, first - last); // erases label from target
	}
	
//Check for ' and .=====================================================================================================================================

	for (unsigned int i = 0; i < target.length(); i++)		// Scans through rest of line to check for quotes and .comments
	{
		if ((target[i] == 39) || (target[i] == '.'))
		{
			if (target[i] == '.')							// If it finds the comment marker it puts the rest of the line into column 4
			{
				tokenVector[3] = target.substr(i, target.length() - i);
				target.erase(i, target.length() - i);
				break;
			}
			for (unsigned int j = i+1; j < target.length(); j++)	// If it finds quotes it removes them from target and saves them to the quotes vector
			{
				if(target[j] == 39)
				{
					quotes.push_back(target.substr(i + 1, j + 1 - i));
					target.erase(i + 1, j + 1 - i);
					break;
				}
				if(j == (target.length() - 1))
					throw file_parse_exception("Line " + error + ": Quote requires end quote.");	// If the quotes don't have end quotes it throws an exception
			}
			
		}
	}
	
// At this point target should contain no label, no comment, and any quotes have been erased and put into
// a separate string vector to be appended later.
	int last = target.find_first_not_of(delimiters,0);
	int first = target.find_first_of(delimiters, last);
	
	while(first != -1 || last != -1)	// Create tokens and push them into temp
	{
		string token = target.substr(last, first-last);
		temp.push_back(token);
		last = target.find_first_not_of(delimiters,first);
		first = target.find_first_of(delimiters, last);
	}
	
	if (temp.size() == 0)	// If there are no tokens you are done
		return tokenVector;
	
	else if (temp.size() == 1) // If there is one token append any quotes back onto the string and put the token into the opcode column
	{
		for (unsigned int i = 0; i < temp.at(0).length(); i++)
		{
			if ((temp.at(0)[i] == 39) && (quotes.size() != 0))
			{
				i += quotes.at(0).size();
				temp.at(0).append(quotes.at(0));
				quotes.erase(quotes.begin());
			}
		}
		tokenVector[1] = temp.at(0);
	}
	
	else if (temp.size() == 2) // Opcodes shouldn't have quotes so put the first token into the opcode column and put quotes back for second token, if it had any, and put the second token into the operand column
	{
		tokenVector[1] = temp.at(0);
		
		for (unsigned int i = 0; i < temp.at(1).length(); i++)
		{
			if ((temp.at(1)[i] == 39) && (quotes.size() != 0))
			{
				i += quotes.at(0).size();
				temp.at(1).append(quotes.at(0));
				quotes.erase(quotes.begin());
			}
		}
		
		tokenVector[2] = temp.at(1);
	}
	
	else throw file_parse_exception("Line " + error + ": Too many non-comment tokens");	// If there are more than 2 tokens it's an error since the label and .comment should have already been taken care of
	
	
	return tokenVector;	// return this nightmare
}
