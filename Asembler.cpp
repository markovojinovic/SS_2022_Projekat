#include "asembler.h"
#include "Regexes.h"
#include "OpCodeErrors.h"

Asembler::Asembler(string in_name, string out_name)
{
    this->input_name = in_name;
    this->output_name = out_name;

    line = 0;

    file.open(input_name);
    if (!file || !file.is_open())
    {
        op_code = -2;
        printError(op_code, line);
    }
}

Asembler::~Asembler()
{
    this->file.close();
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
        rez = get_code_of_instriction(red);
    if (rez == 0)
        return rez;
    // TODO: switch za pozivanje funkcija ( u zavisnosti koja je asemblerska naredba )

    if (rez > 0)
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
        this->extern_.push_back(m.str(0));
        novi = m.suffix().str();
    }

    // TODO: zavrsiti ostatak za direktivu
}

void Asembler::global_function(string red)
{
    string novi = regex_replace(red, global_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    while (regex_search(novi, m, filter_from_direktives))
    {
        this->global.push_back(m.str(0));
        novi = m.suffix().str();
    }

    // TODO: zavrsiti ostatak za direktivu
}