/*
Stefani Bekker - cssc0424
Sierra Morehardt - cssc0419
Austin Aguiar - cssc0480
CS530, Spring 2019
Assignment 3 SIC/XE Disassembler
calculateAddress.cpp
*/
#include "calculateAddress.hpp"

using namespace std;

/* Calculate Base Register */
string calculateBase(string tempAdr, map<string,string> baseTxtRec, bool iFlag) {

    stringstream fullAddress;
    fullAddress << setfill('0') << setw(6) << tempAdr;
    string fullTempAdr = fullAddress.str();

    /* Immediate Operand */
    if(iFlag)
        return fullTempAdr;

    /* Either Simple or Indirect */
    map<string,string>::iterator i = baseTxtRec.begin();
    string lastAdr = i -> first;
    string lastCont = i -> second;

    /* Skip to Target Address */
    while((i -> first).compare(fullTempAdr) <= 0) {

        lastAdr = i -> first;
        lastCont = i -> second;
        i++;

        /* Break at End */
        if(i == baseTxtRec.end())
            break;
    }

    /* Full Temporary Address and Last Address are Identical */
    if(fullTempAdr.compare(lastAdr) == 0)
        return lastCont.substr(0, 6);

    /* Full Temporary Address is Greater Than Last Address */
    else if(fullTempAdr.compare(lastAdr) > 0)
        return lastCont.substr(2 * subtractAddress(lastAdr,fullTempAdr), 6);
    
    return NULL;
}

string incrementAddress(string loc, string incrementLength, int progCount) {

    stringstream finalLoc;
    stringstream hexLength;
    unsigned int length;
    int location;

    /* Negative Displacement */
    if(progCount == 3 && incrementLength[0] >= '8')
        incrementLength = "FFFFF" + incrementLength;

    stringstream locStream;
    locStream << hex << loc;
    locStream >> location;
    hexLength << hex << incrementLength;
    hexLength >> length;
    finalLoc << hex << uppercase << setfill('0') << setw(6) << static_cast<int>(location) + length + progCount;
    return finalLoc.str();
}

int subtractAddress(string lower, string upper) {

    stringstream address1;
    stringstream address2;
    stringstream diff;
    int firstAdr;
    int secondAdr;
    int result;

    address1 << hex << lower;
    address1 >> firstAdr;
    address2 << hex << upper;
    address2 >> secondAdr;
    diff << hex << secondAdr - firstAdr;
    diff >> result;
    return result;
}