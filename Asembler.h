#ifndef PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H
#define PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class Asembler
{
public:
    Asembler(string in_name, string out_name);
    ~Asembler();
    int next_operation();

private:
    int op_code;
    int line;
    ifstream fajl;
    string input_name;
    string output_name;
};

// op_code:
//-2 ========= Greska pri otvaranju fajla
//-3 ========= Nedefinisana asemblerska naredba

#endif
