#include "asembler.h"
#include "Regexes.h"
#include "OpCodeErrors.h"
#include <algorithm>

Asembler::Asembler(string in_name, string out_name)
{
    this->input_name = in_name;
    this->output_name = out_name;
    this->currentSection = "";
    this->currentSectionNumber = -1;

    this->line = 0;
    this->locationCounter = 0;
    this->stopProcess = false;

    file.open(input_name);
    if (!file || !file.is_open())
    {
        op_code = -2;
        printError(op_code, line);
        this->stopProcess = true;
    }

    output.open(this->output_name, ios_base::out | ios_base::binary);
    if (!file.is_open())
    {
        op_code = -2;
        printError(op_code, line);
        this->stopProcess = true;
    }
}

Asembler::~Asembler()
{
    this->file.close();
    this->output.close();
}

int Asembler::next_instruction()
{
    if (this->stopProcess)
    {
        return -8;
    }
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

    if (rez == 7)
    {
        return rez;
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
    case 6:
        skip_function(red);
        break;
    case 8:
        halt_instruction();
        break;
    case 9:
        reg_instruction(7, 0, red);
        break;
    case 10:
        reg_instruction(7, 1, red);
        break;
    case 11:
        reg_instruction(7, 2, red);
        break;
    case 12:
        reg_instruction(7, 3, red);
        break;
    case 13:
        reg_instruction(7, 4, red);
        break;
    case 14:
        reg_instruction(8, 1, red);
        break;
    case 15:
        reg_instruction(8, 2, red);
        break;
    case 16:
        reg_instruction(8, 3, red);
        break;
    case 17:
        reg_instruction(8, 4, red);
        break;
    case 18:
        reg_instruction(9, 0, red);
        break;
    case 19:
        reg_instruction(9, 1, red);
        break;
    }

    if (rez >= 0)
        return rez;
    else
    {
        op_code = rez;
        printError(op_code, line);
        this->stopProcess = true;
        return op_code;
    }
}

int Asembler::get_code_of_instriction(string red)
{
    if (regex_match(red, comments))
        return 1;
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
    if (regex_match(red, halt_instr))
        return 8;
    if (regex_match(red, add_instr))
        return 9;
    if (regex_match(red, sub_instr))
        return 10;
    if (regex_match(red, mul_instr))
        return 11;
    if (regex_match(red, div_instr))
        return 12;
    if (regex_match(red, cmp_instr))
        return 13;
    if (regex_match(red, and_instr))
        return 14;
    if (regex_match(red, or_instr))
        return 15;
    if (regex_match(red, xor_instr))
        return 16;
    if (regex_match(red, test_instr))
        return 17;
    if (regex_match(red, shl_instr))
        return 18;
    if (regex_match(red, shr_instr))
        return 19;

    return -3;
}

void Asembler::extern_function(string red)
{
    string novi = regex_replace(red, extern_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    while (regex_search(novi, m, filter_from_direktives) && !this->stopProcess)
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
                this->stopProcess = true;
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
                this->stopProcess = true;
                printError(op_code, line);
                break;
            }
        }

        if (greska)
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
            this->add_to_symbol_table(Symbol(new_symbol, false, false, 0), false);
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
    while (regex_search(novi, m, filter_from_direktives) && !this->stopProcess)
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");
        for (string a : this->extern_)
        {
            if (a == new_symbol)
            {
                op_code = -4;
                printError(-4, this->line);
                this->stopProcess = true;
                return;
            }
        }
        bool contains = false;
        for (auto a : this->symbolTable)
        {
            if (a.name == new_symbol)
            {
                contains = true;
                this->add_to_symbol_table(Symbol(new_symbol, true, false, 0), contains);
            }
        }
        if (!contains)
        {
            this->global.push_back(new_symbol);
            this->add_to_symbol_table(Symbol(new_symbol, true, false, 0), false);
        }
        novi = m.suffix().str();
    }
}

void Asembler::section_function(string red)
{
    if (this->currentSection != "")
    {
        for (auto i = this->symbolTable.rbegin(); i != this->symbolTable.rend(); i++)
        {
            if (i->name == this->currentSection)
            {
                i->size = this->locationCounter;
            }
        }

        this->print_vector();
    }

    string novi = regex_replace(red, section_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    if (regex_search(novi, m, filter_from_direktives))
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");

        add_to_symbol_table(Symbol(new_symbol, false, true, this->currentSectionNumber), false);
    }
    else
    {
        op_code = -3;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }
}

void Asembler::word_function(string red)
{
    if (this->currentSection == "")
    {
        this->op_code = -7;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }

    string novi = regex_replace(red, word_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;
    while (regex_search(novi, m, filter_from_word) && !this->stopProcess)
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");

        if (new_symbol != "")
        {
            if (regex_match(new_symbol, decimal_num))
            {
                __int16 broj = stoi(new_symbol);
                __int8 prvi = broj / 10;
                __int8 drugi = broj % 10;
                this->for_write.push_back(*to_string(drugi).c_str());
                this->for_write.push_back(*to_string(prvi).c_str());
                this->locationCounter += 2;
            }
            else if (regex_match(new_symbol, hexa_num))
            {
                __int16 x = std::stoul(new_symbol, nullptr, 16);
                __int8 prvi = x / 10;
                __int8 drugi = x % 10;
                this->for_write.push_back(*to_string(drugi).c_str());
                this->for_write.push_back(*to_string(prvi).c_str());
                this->locationCounter += 2;
            }
            else if (regex_match(new_symbol, symbol))
            {
                __int16 val = -1;
                int num = -1;

                for (auto tr : this->symbolTable)
                    if (tr.name == new_symbol)
                    {
                        if (tr.isGlobal)
                        {
                            num = tr.number;
                            val = 0;
                        }
                        else
                        {
                            num = this->currentSectionNumber;
                            val = tr.value;
                        }
                    }

                if (val != -1)
                {
                    this->for_write.push_back(*to_string(val & 0xff).c_str());
                    this->for_write.push_back(*to_string((val >> 8) & 0xff).c_str());
                    this->backPatching[new_symbol] = Info(this->locationCounter, 1, num);
                }
                else
                {
                    this->for_write.push_back(this->nonce);
                    this->for_write.push_back(this->nonce);
                    int num = this->add_to_symbol_table(Symbol(new_symbol, false, false, this->currentSectionNumber), false);
                    this->backPatching[new_symbol] = Info(this->locationCounter, 1, num);
                }
                this->locationCounter += 2;
            }
        }

        novi = m.suffix().str();
    }
}

void Asembler::skip_function(string red)
{
    if (this->currentSection == "")
    {
        this->op_code = -7;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }

    string novi = regex_replace(red, skip_directive_replace, "");
    novi = regex_replace(novi, regex(" "), "");
    smatch m;

    if (regex_match(novi, hexa_num))
    {
        int broj = stoi(novi);
        for (int i = 0; i < broj; i++)
        {
            this->for_write.push_back('0');
            this->locationCounter++;
        }
    }
    else if (regex_match(novi, decimal_num))
    {
        int broj = std::stoul(novi, nullptr, 16);
        for (int i = 0; i < broj; i++)
        {
            this->for_write.push_back('0');
            this->locationCounter++;
        }
    }
    else
    {
        this->op_code = -3;
        printError(op_code, line);
        this->stopProcess = true;
    }
}

void Asembler::halt_instruction() // TODO: Videti da li jos treba nesto da se doda
{
    this->for_write.push_back('0');
    this->for_write.push_back('0');
}

void Asembler::parse_reg_instruction(string red, int &destination, int &source)
{
    int dest, src;
    smatch m;

    if (regex_search(red, m, filter_from_instruction))
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");
        new_symbol = regex_replace(new_symbol, filter_from_add, "");

        red = m.suffix().str();

        dest = stoi(new_symbol);
        if (dest > 7)
        {
            op_code = -11;
            printError(op_code, line);
            this->stopProcess = true;
            return;
        }

        if (regex_search(red, m, filter_from_instruction))
        {
            new_symbol = m.str(0);
            new_symbol = regex_replace(new_symbol, regex(" "), "");
            new_symbol = regex_replace(new_symbol, filter_from_add, "");

            src = stoi(new_symbol);
            if (src > 7)
            {
                op_code = -11;
                printError(op_code, line);
                this->stopProcess = true;
                return;
            }

            destination = dest;
            source = src;
        }
        else
        {
            op_code = -10;
            printError(op_code, line);
            this->stopProcess = true;
            return;
        }
    }
    else
    {
        op_code = -10;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }
}

void Asembler::reg_instruction(int fa, int sa, string red)
{
    regex filter;
    if (fa == 7)
    {
        switch (sa)
        {
        case 0:
            filter = add_instr_filter;
            break;
        case 1:
            filter = sub_instr_filter;
            break;
        case 2:
            filter = mul_instr_filter;
            break;
        case 3:
            filter = div_instr_filter;
            break;
        case 4:
            filter = cmp_instr_filter;
            break;
        default:
            break;
        }
    }
    else if (fa == 8)
    {
        switch (sa)
        {
        // case 0:
        //     filter = add_instr_filter;
        //     break;
        case 1:
            filter = and_instr_filter;
            break;
        case 2:
            filter = or_instr_filter;
            break;
        case 3:
            filter = xor_instr_filter;
            break;
        case 4:
            filter = test_instr_filter;
            break;
        default:
            break;
        }
    }
    else if (fa == 9)
    {
        switch (sa)
        {
        case 0:
            filter = shl_instr_filter;
            break;
        case 1:
            filter = shr_instr_filter;
            break;
        default:
            break;
        }
    }

    string novi = regex_replace(red, filter, "");
    novi = regex_replace(novi, regex(" "), "");

    int dest = 0, src = 0;
    this->parse_reg_instruction(novi, dest, src);

    if (this->stopProcess)
    {
        return;
    }

    this->for_write.push_back(*to_string(sa).c_str());
    this->for_write.push_back(*to_string(fa).c_str());
    this->for_write.push_back(*to_string(src).c_str());
    this->for_write.push_back(*to_string(dest).c_str());
}

void Asembler::print_symbol_table()
{
    cout << endl
         << endl;
    cout << "=========================================Symbol Table==============================================" << endl;
    cout << "---------------------------------------------------------------------------------------------------" << endl;
    cout << "Name\t\tIs global\tNumber\t\tSection\t\tSize\t\tValue\t\t" << endl;
    cout << "---------------------------------------------------------------------------------------------------" << endl;
    for (Symbol a : this->symbolTable)
    {
        if (a.name.length() < 8)
            cout << a.name << "\t\t" << a.isGlobal << "\t\t" << a.number << "\t\t" << a.seciton
                 << "\t\t" << a.size << "\t\t" << a.value << endl;
        else
            cout << a.name << "\t" << a.isGlobal << "\t\t" << a.number << "\t\t" << a.seciton
                 << "\t\t" << a.size << "\t\t" << a.value << endl;
    }
    cout << "---------------------------------------------------------------------------------------------------" << endl;
    cout << endl
         << endl;
}

int Asembler::add_to_symbol_table(Symbol s, bool redefied)
{
    bool dodaj = true;

    for (auto tr = this->symbolTable.rbegin(); tr != this->symbolTable.rend(); tr++)
    {
        if (tr->name != s.name)
            continue;
        else
        {
            if (redefied && s.isGlobal)
            {
                tr->isGlobal = true;
                dodaj = false;
                break;
            }
            else
            {
                this->op_code = -6;
                printError(op_code, line);
                this->stopProcess = true;
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
            this->currentSectionNumber = s.number;
        }
        this->symbolTable.push_back(s);
    }

    return s.number;
}

void Asembler::print_vector()
{
    this->output.write(this->for_write.data(), this->for_write.size());
    this->for_write.clear();
    this->locationCounter = 0;
}

int Asembler::start_reading()
{
    while (true)
    {
        int rez = this->next_instruction();
        if (rez < 0)
            break;
        if (rez == 7)
        {
            for (auto i = this->symbolTable.rbegin(); i != this->symbolTable.rend(); i++)
            {
                if (i->name == this->currentSection)
                {
                    i->size = this->locationCounter;
                }
            }

            this->print_vector();
            break;
        }
    }
    if (this->stopProcess)
        return -1;
    else
        return 1;
}