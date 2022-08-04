#include "linker.h"
#include "Asembler\OpCodeErrors.h"

linker::linker(vector<string> input_n)
{
    this->input_name = input_n;
    input.open(this->input_name.front());
    if (!input.is_open() || !input)
        printError(FILE_ERROR, 0);
    this->currentInput = this->input_name.front();
    this->red = 0;
}

linker::~linker()
{
    this->currentInput.clear();
    this->global.clear();
    this->extern_.clear();
    this->input_name.clear();
    this->symbolTable.clear();
    this->currentTable.clear();
    this->currentRelocatible.clear();
    this->for_write.clear();
}

int linker::start_reading()
{
    for (auto i : this->input_name)
    {
        this->currentInput = i;
        red_current_sym_table();
        red_current_relocatible();
        // obrada trenutnog fajla
        // prebacivanje input na sledeci fajl, i zatvaranje prethodnog
    }
}

void linker::exit_protocol()
{
}

void linker::red_current_sym_table()
{
    string name = "asembly_symbol_table_";
    for (auto a : this->currentInput)
        if (a != '.')
            name += a;
        else
            break;
    name += ".txt";
    ifstream curr;
    curr.open(name);
    if (!curr || !curr.is_open())
        printError(FILE_ERROR, 0);
    string red;
    while (getline(curr, red))
    {
        char nam[20];
        int glob, num, sec, size, val;
        sscanf(red.c_str(), "%s\t%d\t%d\t%d\t%d\t%d", nam, &glob, &num, &sec, &size, &val);
        Symbol cur(nam, false, false, sec, val);
        cur.size = size;
        cur.seciton == cur.number ? cur.isSection = true : cur.seciton = false;
        glob == 1 ? cur.isGlobal = true : cur.isGlobal = false;
        this->currentTable.push_back(cur);
    }
}

void linker::red_current_relocatible()
{
    string name = "asembly_relocation_write_";
    for (auto a : this->currentInput)
        if (a != '.')
            name += a;
        else
            break;
    name += ".txt";
    ifstream curr;
    curr.open(name);
    if (!curr || !curr.is_open())
        printError(FILE_ERROR, 0);
    string red;
    while (getline(curr, red))
    {
        char nam[20];
        int locC, locM, ty, num;
        sscanf(red.c_str(), "%s\t%d\t%d\t%d\t%d", nam, &locC, &locM, &ty, &num);
        Info cur(nam, locC, locM, ty, num);
        this->currentRelocatible.push_back(cur);
    }
}