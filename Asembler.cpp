#include "asembler.h"
#include "Regexes.h"
#include "OpCodeErrors.h"
#include <algorithm>

Asembler::Asembler(string in_name, string out_name)
{
    this->input_name = in_name;
    this->output_name = out_name;
    this->currentSection = "";

    this->line = 0;
    this->locationCounter = 0;

    file.open(input_name);
    if (!file || !file.is_open())
    {
        op_code = -2;
        printError(op_code, line);
    }

    this->output = fopen(out_name.c_str(), "wb");
    if (!output)
    {
        op_code = -2;
        printError(op_code, line);
    }
}

Asembler::~Asembler()
{
    this->file.close();
    fclose(this->output);
}

int Asembler::next_instruction()
{
    string red;
    if (!file.eof())
        getline(this->file, red);
    else
        return 0; // Znaci da nije greska samo smo dosli do kraja fajla
    this->line++;

    int rez = 0;
    if (red != "")
    {
        red = regex_replace(red, filter_comment, "");
        if (red != "")
            rez = get_code_of_instriction(red);
        else
            return 1;
    }

    switch (rez) // TODO: zavrsiti
    {
    case 2:
        global_function(red);
        break;
    case 3:
        extern_function(red);
        break;
    case 4:
        section_function(red);
        break;
    case 5:
        word_function(red);
        break;
    }

    if (rez >= 0)
        return rez;
    else
    {
        op_code = -3;
        printError(op_code, line);
        return op_code;
    }
}

int Asembler::get_code_of_instriction(string red)
{
    if (regex_match(red, comments))
        return 1;
    if (regex_match(red, direktives))
    {
        if (regex_match(red, global_directive))
            return 2;
        if (regex_match(red, extern_directive))
            return 3;
        if (regex_match(red, section_directive))
            return 4;
        if (regex_match(red, word_directive))
            return 5;
        if (regex_match(red, skip_directive))
            return 6;
        if (regex_match(red, end_directive))
            return 7;
    }

    return -3;
}

void Asembler::extern_function(string red)
{
    string novi = regex_replace(red, extern_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    while (regex_search(novi, m, filter_from_direktives))
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");
        bool greska = false;

        for (string a : this->global)
        {
            if (a == new_symbol)
            {
                op_code = -4;
                printError(op_code, this->line);
                return;
            }
        }

        for (Symbol tr : this->symbolTable)
        {
            if (tr.name != new_symbol)
                continue;
            else
            {
                this->op_code = -6;
                greska = true;
                printError(op_code, line);
                break;
            }
        }

        if (greska) // TODO: potencijalno napraviti da se prekine ovde rad prevodjenja
        {
            novi = m.suffix().str();
            continue;
        }

        bool contains = false;
        for (string a : this->extern_)
        {
            if (a == new_symbol)
                contains = true;
        }
        if (!contains)
        {
            this->add_to_symbol_table(Symbol(new_symbol, false, false), false);
            this->extern_.push_back(new_symbol);
        }
        novi = m.suffix().str();
    }
}

void Asembler::global_function(string red)
{
    string novi = regex_replace(red, global_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    while (regex_search(novi, m, filter_from_direktives))
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");
        for (string a : this->extern_)
        {
            if (a == new_symbol)
            {
                op_code = -4;
                printError(-4, this->line);
                return;
            }
        }
        bool contains = false;
        for (string a : this->global)
        {
            if (a == new_symbol)
            {
                contains = true;
                this->add_to_symbol_table(Symbol(new_symbol, true, false), contains);
            }
        }
        if (!contains)
        {
            this->global.push_back(new_symbol);
            this->add_to_symbol_table(Symbol(new_symbol, true, false), false);
        }
        novi = m.suffix().str();
    }
}

void Asembler::section_function(string red)
{
    if (this->currentSection != "")
    {
        for (Symbol s : this->symbolTable)
        {
            if (s.name == this->currentSection)
            {
                s.size = this->locationCounter;
            }
        }
    }

    string novi = regex_replace(red, section_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    if (regex_search(novi, m, filter_from_direktives))
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");

        add_to_symbol_table(Symbol(new_symbol, false, true), false);
    }
    else
    {
        op_code = -3;
        printError(op_code, line);
        return;
    }
}

void Asembler::word_function(string red) // TODO: popraviti upis u bin fajl
{
    string novi = regex_replace(red, word_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    while (regex_search(novi, m, filter_from_word))
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");

        if (new_symbol != "")
        {
            if (regex_match(new_symbol, decimal_num))
            {
                cout << "Decimalni broj je " << endl;
                fwrite(new_symbol.c_str(), 2, this->locationCounter, this->output);
                this->locationCounter += 2;
            }
            else if (regex_match(new_symbol, hexa_num))
            {
                cout << "Hexadecimalni broj je" << endl;
                int x = std::stoul(new_symbol, nullptr, 16);
                string upis = to_string(x);
                fwrite(upis.c_str(), 2, this->locationCounter, this->output);
                this->locationCounter += 2;
            }
            else if (regex_match(new_symbol, symbol))
            {
                cout << "Simbol je" << endl;
                // Dodati u neku listu pa posale, lokaciju zapamtiti kao locCnt pa kad dobijemo njegovu vrednost, dodati
                fwrite(this->nonce, 2, this->locationCounter, this->output);
                this->backPatching.push_back(this->locationCounter);
                this->locationCounter += 2;
            }
        }

        novi = m.suffix().str();
    }
}

void Asembler::print_symbol_table() // TODO: zavrsiti ostatak
{
    for (Symbol a : this->symbolTable)
        cout << a.name << " " << a.isGlobal << " " << a.number << " " << a.seciton << " " << a.size << endl;
}

void Asembler::add_to_symbol_table(Symbol s, bool redefied) // TODO: zavrsiti ostatak
{
    bool dodaj = true;

    for (Symbol tr : this->symbolTable)
    {
        if (tr.name != s.name)
            continue;
        else
        {
            if (redefied && s.isGlobal)
                tr.isGlobal = true;
            else
            {
                this->op_code = -6;
                printError(op_code, line);
                dodaj = false;
                break;
            }
        }
    }

    if (dodaj)
    {
        s.number = number++;
        if (s.isSection)
        {
            s.seciton = s.number;
            this->locationCounter = 0;
            this->currentSection = s.name;
        }
        this->symbolTable.push_back(s);
    }
}

// string nova_rec(string *red)
// {
//     string ret = "";
//     for (int i = 0; i < red->length(); i++)
//     {
//         if ((*red)[i] != ',')
//         {
//             ret[i] = (*red)[i];
//             (*red)[i] = "";
//         }
//     }

//     return ret;
// }