#ifndef LINKER_H
#define LINKER_H
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include <vector>
#include "Asembler\Asembler.h"

class linker
{
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
            this->another = false;
            this->isExtern = false;
        }
        Symbol() {}

    private:
        string name;
        int seciton;
        int value;
        bool isGlobal;
        bool isSection;
        bool another;
        int number;
        int size;
        bool isExtern;

        friend class linker;
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
        Info(string name, int location, int mem, int type, int num, string izr = "")
        {
            this->locationInCode = location;
            this->typeOfDefinition = type;
            this->numberInSybolTable = num;
            this->locationInMemory = mem;
            this->name = name;
            this->writted = false;
            this->izraz = izr;
        }

    private:
        int locationInCode;
        int locationInMemory;
        int typeOfDefinition;
        int numberInSybolTable;
        string name;
        string izraz;
        bool writted;

        friend class Asembler;
        friend class linker;
    };

    class section
    {
    public:
        vector<char> code;
        string ime;
        bool served;
        int pomeraj;
        section(string im)
        {
            this->ime = im;
            this->served = false;
            this->pomeraj = -1;
        }
    };

    class file_
    {
    public:
        vector<section> sections;
        string ime;
        list<Symbol> symTable;
        list<Info> relTable;
        file_(string im) { this->ime = im; }
    };

private:
    bool stopProcess;
    list<string> global;
    list<string> extern_;
    ifstream input;
    ofstream output;
    string currentInput;
    vector<string> input_name;
    list<Symbol> symbolTable;
    vector<file_> red_files;
    vector<char> for_write;
    int red;

    void red_current();
    void obrada();

public:
    linker(vector<string>, string);
    ~linker();
    int start_reading();
    void exit_protocol();
};

#endif