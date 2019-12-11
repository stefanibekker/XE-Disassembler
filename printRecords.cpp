/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
printRecords.cpp
*/
#include "printRecords.hpp"
#include "calculateAddress.cpp"

using namespace std;

//Header record
void printHeaderRec(string label, string beginAdr,ofstream& inStream) {
    //If operand is 0, only print out one "0"
    if(beginAdr.compare("000000") == 0)    
        inStream << setw(6) << label << setw(3) << " " << "START" << "  " << "0" << endl;
        //Delete leading zero and print
    else    
        inStream << setw(6) << label << setw(3) << " " << "START" << "  " << beginAdr.erase(0,beginAdr.find_first_not_of('0')) << endl;
}

void printTextRecord(string label, string mnemonic, string operand, ofstream& inStream, bool xFlag, bool eFlag, bool nFlag, bool iFlag) { //Text record
    //Formatting the output
    string tempMne = mnemonic; 
    mnemonic = " " + mnemonic;
    string tempOp = operand;
    operand = " " + operand;
    if(operand.compare("0") == 0 || operand.compare("00") == 0 || operand.compare("000") == 0|| operand.compare("0000") == 0 || operand.compare("00000") == 0 || operand.compare("000000") == 0)
        operand = "0";
    if(eFlag)
        mnemonic = "+"+tempMne;
    if(operand.compare("0") != 0){
        operand = operand.erase(0,operand.find_first_not_of('0'));
    }  
    if(nFlag)
        operand = "@"+tempOp;
    else if(iFlag)
        operand = "#"+tempOp;
    if(xFlag)
        operand = operand+",X";
    inStream << label << setw(8-label.size()) << " "  << mnemonic << setw(8-mnemonic.size()) << " " << operand << endl;
}
//End record
void printEndRec(string endRec, ofstream& inStream, string label) {    
    //Operand in END directive
    if(endRec.size() > 0)    
        inStream << "        " << "END     " << label << endl;
        //No operand
    else    
        inStream << "        " << "END" << endl;
}
//Calculate and print RESB/W after each text record
void printRes(symtab& inSymtab, string lastTxtRec, string textRecordAddress, ofstream& inStream) { 
    multimap<string,string> resSymtab = inSymtab.getSymTab();
    multimap<string,string>::iterator i = resSymtab.begin();
    multimap<string,string>::iterator tempI;
    //Skip to current location from beginning of symbol table
    while((i->first).compare(lastTxtRec) < 0){    
        i++;
        i++;
    }
    tempI = i;
    //Skip absolute symbols at the start
    while(((++tempI)->second).compare("A") == 0) {        
        i++;
        i++;
        tempI=i;
    }
    string prevSym = i->first;
    i++;
    i++;
    bool orgFlag = false;
    string orgStop = "";
    int remainLen=0;
    string symName="";
    int resLength=0;
    while((i->first).compare(textRecordAddress) <= 0) {
        tempI = i;
        //Skip similar absolute symbols during RESB/W
        if(((++tempI)->second).compare("A") == 0) {        
            i++;
            i++;
            if(i==resSymtab.end() || (i->first).compare(textRecordAddress) > 0) {
                if(prevSym.compare(textRecordAddress) != 0) {
                    symName = inSymtab.getRelSym(prevSym);
                    resLength = subtractAddress(prevSym,textRecordAddress);
                    if(resLength%3==0)
                        inStream << symName << "  RESW    " << intToString(resLength/3) << endl;
                    else
                        inStream << symName << "  RESB    " << intToString(resLength) << endl;
                }
                if(orgStop.compare("") != 0)
                    inStream << "        ORG    " << orgStop << endl;
                break;
            }
            continue;
        }
        resLength = subtractAddress(prevSym,i->first);
        if(orgFlag) {
            symName = inSymtab.getReverseRel(prevSym);
            orgFlag=false;
        }
        else
            symName = inSymtab.getRelSym(prevSym);
            //If two relative symbols that have same address, then it's ORG
        if(resLength == 0) {    
            remainLen = subtractAddress(prevSym,textRecordAddress);
            if(remainLen%3==0)
                inStream << symName << "  RESW    " << intToString(remainLen/3) << endl;
            else
                inStream << symName << "  RESB    " << intToString(remainLen) << endl;
            inStream << "        ORG    " << symName << endl;
            orgFlag = true;
            orgStop = symName+"+"+intToString(remainLen);
        }
        //Otherwise just RESB/W
        else {    
            if(resLength%3==0)
                inStream << symName << "  RESW    " << intToString(resLength/3) << endl;
            else
                inStream << symName << "  RESB    " << intToString(resLength) << endl;
        }
        prevSym = i->first;
        i++;
        i++;
        //If pass the end of symbol table and max length of current text record calculate the remaining data.
        if(i==resSymtab.end() || (i->first).compare(textRecordAddress) > 0) {    
            if(prevSym.compare(textRecordAddress) != 0) {
                symName = inSymtab.getRelSym(prevSym);
                resLength = subtractAddress(prevSym,textRecordAddress);
                if(resLength%3==0)
                    inStream << symName << "  RESW    " << intToString(resLength/3) << endl;
                else
                    inStream << symName << "  RESB    " << intToString(resLength) << endl;
            }
            if(orgStop.compare("") != 0)
                inStream << "        ORG    " << orgStop << endl;
            break;
        }
    }
}
