#ifndef PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H
#define PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H

#include <string>
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

class Asembler
{
public:
    Asembler(string, string);
    ~Asembler();
    int next_instruction();
    int get_code_of_instriction(string);
    void extern_function(string);
    void global_function(string);

private:
    int op_code;
    int line;
    ifstream file;
    string input_name;
    string output_name;
    list<string> global;
    list<string> extern_;
};

#endif
