#ifndef PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H
#define PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

static int number = 0;

class Asembler
{
public:
    Asembler(string, string);
    ~Asembler();
    int next_instruction();
    int get_code_of_instriction(string);
    void extern_function(string);
    void global_function(string);
    void section_function(string);
    void word_function(string);
    void skip_function(string);

    void print_symbol_table();
    void print_vector();

    class Symbol
    {
    public:
        Symbol(string name, bool isGlobal, bool isSection)
        {
            this->name = name;
            this->isGlobal = isGlobal;
            this->isSection = isSection;
            this->seciton = 0;
            if (!isSection)
                this->size = -1;
            else
                this->size = 0;
        }

    private:
        string name;
        int seciton;
        int value;
        bool isGlobal;
        bool isSection;
        int number;
        int size;

        friend class Asembler;
    };

private:
    int op_code;
    int line;
    int locationCounter;
    ifstream file;
    fstream output;
    string input_name;
    string output_name;
    string currentSection;
    list<string> global;
    list<string> extern_;
    list<Symbol> symbolTable;
    vector<char> for_write;
    unordered_map<string, int> backPatching;

    char nonce = 'R';

    void add_to_symbol_table(Symbol, bool);
};

#endif
