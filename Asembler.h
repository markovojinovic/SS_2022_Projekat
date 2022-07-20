#ifndef PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H
#define PROBA_ZA_PROJEKAT_IZ_SS_ASEMBLER_H

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

static int number = 1;

class Asembler
{
public:
    Asembler(string, string);
    ~Asembler();

    void print_symbol_table();
    int start_reading();

    class Symbol
    {
    public:
        Symbol(string name, bool isGlobal, bool isSection, int section, int val = 0)
        {
            this->name = name;
            this->isGlobal = isGlobal;
            this->isSection = isSection;
            this->seciton = section;
            if (!isSection)
                this->size = -1;
            else
                this->size = 0;
            this->value = val;
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

    class Info
    {
    public:
        Info()
        {
            this->locationInCode = 0;
            this->typeOfDefinition = 0;
            this->numberInSybolTable = 0;
            this->locationInMemory = 0;
            this->name = "";
            this->writted = false;
        }
        Info(string name, int location, int mem, int type, int num)
        {
            this->locationInCode = location;
            this->typeOfDefinition = type;
            this->numberInSybolTable = num;
            this->locationInMemory = mem;
            this->name = name;
            this->writted = false;
        }

    private:
        int locationInCode;
        int locationInMemory;
        int typeOfDefinition;
        int numberInSybolTable;
        string name;
        bool writted;

        friend class Asembler;
    };

private:
    int op_code;
    int line;
    int locationCounter;
    int memoryCounter;
    int currentSectionNumber;
    bool stopProcess;
    ifstream file;
    fstream output;
    string input_name;
    string output_name;
    string currentSection;
    list<string> global;
    list<string> extern_;
    list<Symbol> symbolTable;
    vector<char> for_write;
    list<Info> backPatching;

    char nonce = 'R';

    int next_instruction();
    int get_code_of_instriction(string);

    void extern_function(string);
    void global_function(string);
    void section_function(string);
    void word_function(string);
    void skip_function(string);
    void ascii_function(string);
    void equ_function(string);

    void label_function(string);

    void halt_instruction();
    void iret_instruction();
    void ret_instruction();
    void call_instruction(string);

    void reg_instruction(int, int, string);
    void jmp_instruction(int, string);
    void ldr_instruction(string);
    void str_instruction(string);
    void push_pop_instruction(string, int);

    int add_to_symbol_table(Symbol, bool);
    void parse_reg_instruction(string, int &, int &, bool);
    void data_adressing(string, string &, string &, bool &, char &);
    void jump_adressing(string, string &, string &, bool &, char &);

    void print_vector();
    void back_patching(string);

    string int_to_hex(int);
};

#endif