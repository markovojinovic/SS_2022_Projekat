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
        op_code = FILE_ERROR;
        printError(op_code, line);
        this->stopProcess = true;
    }

    output.open(this->output_name, ios_base::out | ios_base::binary);
    if (!file.is_open())
    {
        op_code = FILE_ERROR;
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
        return -8;

    string red;
    if (!file.eof())
        getline(this->file, red);
    else
        return 0;
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
        return rez;

    switch (rez)
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
    case 20:
        ret_instruction();
        break;
    case 21:
        iret_instruction();
        break;
    case 22:
        reg_instruction(8, 0, red);
        break;
    case 23:
        reg_instruction(1, 0, red);
        break;
    case 24:
        reg_instruction(6, 0, red);
        break;
    case 25:
        call_instruction(red);
        break;
    case 26:
        ldr_instruction(red);
        break;
    case 27:
        str_instruction(red);
        break;
    case 28:
        jmp_instruction(0, red);
        break;
    case 29:
        jmp_instruction(1, red);
        break;
    case 30:
        jmp_instruction(2, red);
        break;
    case 31:
        jmp_instruction(3, red);
        break;
    case 32:
        push_pop_instruction(red, 0);
        break;
    case 33:
        push_pop_instruction(red, 1);
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
    if (regex_match(red, ret_instr))
        return 20;
    if (regex_match(red, iret_instr))
        return 21;
    if (regex_match(red, not_instr))
        return 22;
    if (regex_match(red, int_instr))
        return 23;
    if (regex_match(red, xchg_instr))
        return 24;
    if (regex_match(red, call_instr))
        return 25;
    if (regex_match(red, ldr_instr))
        return 26;
    if (regex_match(red, str_instr))
        return 27;
    if (regex_match(red, jmp_instr))
        return 28;
    if (regex_match(red, jeq_instr))
        return 29;
    if (regex_match(red, jne_instr))
        return 30;
    if (regex_match(red, jgt_instr))
        return 31;
    if (regex_match(red, regex("(push)(.*)")))
        return 32;
    if (regex_match(red, regex("(pop)(.*)")))
        return 33;

    return -3;
}

void Asembler::extern_function(string red)
{
    string novi = regex_replace(red, extern_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;

    if (novi == "")
    {
        op_code = UNDEFINED_INSTRUCTION;
        printError(op_code, this->line);
        this->stopProcess = true;
        return;
    }

    while (regex_search(novi, m, filter_from_direktives) && !this->stopProcess)
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");
        bool greska = false;

        for (string a : this->global)
            if (a == new_symbol)
            {
                op_code = EXPORT_IMPORT_ERROR;
                printError(op_code, this->line);
                this->stopProcess = true;
                return;
            }

        for (Symbol tr : this->symbolTable)
            if (tr.name != new_symbol)
                continue;
            else
            {
                this->op_code = ALREDY_DEFINED;
                greska = true;
                this->stopProcess = true;
                printError(op_code, line);
                break;
            }

        if (greska)
        {
            novi = m.suffix().str();
            continue;
        }

        bool contains = false;
        for (string a : this->extern_)
            if (a == new_symbol)
                contains = true;

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

    if (novi == "")
    {
        op_code = UNDEFINED_INSTRUCTION;
        printError(op_code, this->line);
        this->stopProcess = true;
        return;
    }

    while (regex_search(novi, m, filter_from_direktives) && !this->stopProcess)
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");
        for (string a : this->extern_)
            if (a == new_symbol)
            {
                op_code = EXPORT_IMPORT_ERROR;
                printError(-4, this->line);
                this->stopProcess = true;
                return;
            }

        bool contains = false;
        for (auto a : this->symbolTable)
            if (a.name == new_symbol)
            {
                contains = true;
                this->add_to_symbol_table(Symbol(new_symbol, true, false, 0), contains);
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
            if (i->name == this->currentSection)
                i->size = this->locationCounter;

        this->print_vector();
    }

    string novi = regex_replace(red, section_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;

    if (novi == "")
    {
        op_code = UNDEFINED_INSTRUCTION;
        printError(op_code, this->line);
        this->stopProcess = true;
        return;
    }

    if (regex_search(novi, m, filter_from_direktives))
    {
        string new_symbol = m.str(0);
        new_symbol = regex_replace(new_symbol, regex(" "), "");

        add_to_symbol_table(Symbol(new_symbol, false, true, this->currentSectionNumber), false);
    }
    else
    {
        op_code = UNDEFINED_INSTRUCTION;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }
}

void Asembler::word_function(string red)
{
    if (this->currentSection == "")
    {
        this->op_code = OUT_OF_SECTION;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }

    string novi = regex_replace(red, word_directive_replace, "");
    novi = regex_replace(novi, regex("(, )"), " ");
    smatch m;

    if (novi == "" || novi == " ")
    {
        op_code = UNDEFINED_INSTRUCTION;
        printError(op_code, this->line);
        this->stopProcess = true;
        return;
    }

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
        this->op_code = OUT_OF_SECTION;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }

    string novi = regex_replace(red, skip_directive_replace, "");
    novi = regex_replace(novi, regex(" "), "");
    smatch m;

    if (novi == "")
    {
        op_code = UNDEFINED_INSTRUCTION;
        printError(op_code, this->line);
        this->stopProcess = true;
        return;
    }

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
        this->op_code = UNDEFINED_INSTRUCTION;
        printError(op_code, line);
        this->stopProcess = true;
    }
}

void Asembler::halt_instruction()
{
    this->for_write.push_back('0');
    this->for_write.push_back('0');
    this->locationCounter += 2;
}

void Asembler::iret_instruction()
{
    this->for_write.push_back('0');
    this->for_write.push_back('2');
    this->locationCounter += 2;
}

void Asembler::ret_instruction()
{
    this->for_write.push_back('0');
    this->for_write.push_back('4');
    this->locationCounter += 2;
}

void Asembler::call_instruction(string red)
{
    string novi = regex_replace(red, call_instr_filter, "");
    novi = regex_replace(novi, regex(" "), "");
    string first, second;
    char sa, fa, ta, ca;
    bool one;

    this->data_adressing(novi, first, second, one, sa);
    fa = *first.c_str();

    this->for_write.push_back('0');
    this->for_write.push_back('3');
    this->for_write.push_back(fa);
    this->for_write.push_back('F');
    this->for_write.push_back(sa);
    this->for_write.push_back('0');
    this->locationCounter += 6;
    if (!one)
    {
        ta = second[0];
        if (second.size() > 1)
        {
            ca = second[1];
            this->for_write.push_back(ca);
        }
        else
            this->for_write.push_back('0');
        this->for_write.push_back(ta);
        this->locationCounter += 2;
    }
}

void Asembler::data_adressing(string novi, string &first, string &second, bool &one, char &sa)
{
    one = false;
    if (regex_match(novi, dolar))
    {
        one = true;
        novi = regex_replace(novi, dolar_filter, "");
        if (novi == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        sa = '0';
        if (regex_match(novi, clasic_literal))
        {
            first = "F";
            second = novi;
        }
        else if (regex_match(novi, clasic_symbol))
        {
            first = "F";
            string val = "";
            for (auto tr : this->symbolTable)
                if (tr.name == novi)
                    val = to_string(tr.value);
            if (val == "")
            {
                this->op_code = UNDEFINDE_SYMBOL;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
                second = val;
        }
        else
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
    }
    else if (regex_match(novi, clasic_literal))
    {
        one = true;
        sa = '4';
        second = novi;
        first = "F";
    }
    else if (regex_match(novi, percent))
    {
        sa = '3';

        novi = regex_replace(novi, percent_filter, "");
        if (novi == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
        {

            smatch m;
            regex_search(novi, m, clasic_symbol);
            second = m.str(0);
            if (second == "")
            {
                this->op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                first = "7";
                string val = "";
                for (auto tr : this->symbolTable)
                    if (tr.name == second)
                        val = to_string(tr.value);
                if (val == "")
                {
                    this->op_code = UNDEFINDE_SYMBOL;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
                else
                    second = val;
            }
        }
    }
    else if (regex_match(novi, register_adressing))
    {
        one = true;
        sa = '5';
        novi = regex_replace(novi, filter_from_add, "");
        if (novi == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
        {
            first = novi;
            if (stoi(first) > 7 or first.size() > 1)
            {
                this->op_code = REGISTER_OUT_OF_BOUNDS;
                printError(op_code, line);
                this->stopProcess = true;
            }
        }
    }
    else if (regex_match(novi, mem_register_adressing))
    {
        one = true;
        sa = '2';
        novi = regex_replace(novi, filter_from_add, "");
        novi = regex_replace(novi, first_indirekt_filter, "");
        novi = regex_replace(novi, second_indirekt_filter, "");
        if (novi == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
        {
            first = novi;
            if (stoi(first) > 7 or first.size() > 1)
            {
                this->op_code = REGISTER_OUT_OF_BOUNDS;
                printError(op_code, line);
                this->stopProcess = true;
            }
        }
    }
    else if (regex_match(novi, mem_register_adressing_mov_literal))
    {
        sa = '3';

        smatch m;
        regex_search(novi, m, clasic_literal);
        first = m.str(0);
        if (first == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
        {
            if (stoi(first) > 7 or first.size() > 1)
            {
                this->op_code = REGISTER_OUT_OF_BOUNDS;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                novi = m.suffix().str();
                regex_search(novi, m, clasic_literal);
                second = m.str(0);
                if (second == "")
                {
                    this->op_code = SINTAX_ERROR;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
            }
        }
    }
    else if (regex_match(novi, mem_register_adressing_mov_symbol))
    {
        sa = '3';

        smatch m;
        regex_search(novi, m, clasic_literal);
        first = m.str(0);
        if (first == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
        {
            if (stoi(first) > 7 or first.size() > 1)
            {
                this->op_code = REGISTER_OUT_OF_BOUNDS;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                novi = m.suffix().str();
                regex_search(novi, m, clasic_symbol);
                second = m.str(0);
                if (second == "")
                {
                    this->op_code = SINTAX_ERROR;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
                else
                {
                    string val = "";
                    for (auto tr : this->symbolTable)
                        if (tr.name == second)
                            val = to_string(tr.value);
                    if (val == "")
                    {
                        this->op_code = UNDEFINDE_SYMBOL;
                        printError(op_code, line);
                        this->stopProcess = true;
                    }
                    else
                        second = val;
                }
            }
        }
    }
    else if (regex_match(novi, clasic_symbol))
    {
        one = true;
        sa = '4';
        first = "F";

        string val = "";
        for (auto tr : this->symbolTable)
            if (tr.name == novi)
                val = to_string(tr.value);
        if (val == "")
        {
            this->op_code = UNDEFINDE_SYMBOL;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
            second = val;
    }
    else
    {
        this->op_code = SINTAX_ERROR;
        printError(op_code, line);
        this->stopProcess = true;
    }
}

void Asembler::jump_adressing(string novi, string &first, string &second, bool &one, char &sa)
{
    one = false;
    if (regex_match(novi, clasic_literal))
    {
        one = true;
        if (novi == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        sa = '0';
        first = "F";
        second = novi;
    }
    else if (regex_match(novi, clasic_symbol))
    {
        one = true;
        sa = '0';

        string val = "";
        for (auto tr : this->symbolTable)
            if (tr.name == novi)
                val = to_string(tr.value);
        if (val == "")
        {
            this->op_code = UNDEFINDE_SYMBOL;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
            second = val;
    }
    else if (regex_match(novi, percent))
    {
        sa = '5';

        novi = regex_replace(novi, percent_filter, "");
        if (novi == "")
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
        else
        {

            smatch m;
            regex_search(novi, m, clasic_symbol);
            second = m.str(0);
            if (second == "")
            {
                this->op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                first = "7";
                string val = "";
                for (auto tr : this->symbolTable)
                    if (tr.name == second)
                        val = to_string(tr.value);
                if (val == "")
                {
                    this->op_code = UNDEFINDE_SYMBOL;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
                else
                    second = val;
            }
        }
    }
    else if (regex_match(novi, zvezdica))
    {
        novi = regex_replace(novi, zvezdica_filter, "");

        if (regex_match(novi, register_adressing))
        {
            one = true;
            sa = '1';
            novi = regex_replace(novi, filter_from_add, "");
            if (novi == "")
            {
                this->op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                first = novi;
                if (stoi(first) > 7 or first.size() > 1)
                {
                    this->op_code = REGISTER_OUT_OF_BOUNDS;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
            }
        }
        else if (regex_match(novi, mem_register_adressing))
        {
            one = true;
            sa = '2';
            novi = regex_replace(novi, filter_from_add, "");
            novi = regex_replace(novi, first_indirekt_filter, "");
            novi = regex_replace(novi, second_indirekt_filter, "");
            if (novi == "")
            {
                this->op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                first = novi;
                if (stoi(first) > 7 or first.size() > 1)
                {
                    this->op_code = REGISTER_OUT_OF_BOUNDS;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
            }
        }
        else if (regex_match(novi, mem_register_adressing_mov_literal))
        {
            sa = '3';

            smatch m;
            regex_search(novi, m, clasic_literal);
            first = m.str(0);
            if (first == "")
            {
                this->op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                if (stoi(first) > 7 or first.size() > 1)
                {
                    this->op_code = REGISTER_OUT_OF_BOUNDS;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
                else
                {
                    novi = m.suffix().str();
                    regex_search(novi, m, clasic_literal);
                    second = m.str(0);
                    if (second == "")
                    {
                        this->op_code = SINTAX_ERROR;
                        printError(op_code, line);
                        this->stopProcess = true;
                    }
                }
            }
        }
        else if (regex_match(novi, mem_register_adressing_mov_symbol))
        {
            sa = '3';

            smatch m;
            regex_search(novi, m, clasic_literal);
            first = m.str(0);
            if (first == "")
            {
                this->op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
            {
                if (stoi(first) > 7 or first.size() > 1)
                {
                    this->op_code = REGISTER_OUT_OF_BOUNDS;
                    printError(op_code, line);
                    this->stopProcess = true;
                }
                else
                {
                    novi = m.suffix().str();
                    regex_search(novi, m, clasic_symbol);
                    second = m.str(0);
                    if (second == "")
                    {
                        this->op_code = SINTAX_ERROR;
                        printError(op_code, line);
                        this->stopProcess = true;
                    }
                    else
                    {
                        string val = "";
                        for (auto tr : this->symbolTable)
                            if (tr.name == second)
                                val = to_string(tr.value);
                        if (val == "")
                        {
                            this->op_code = UNDEFINDE_SYMBOL;
                            printError(op_code, line);
                            this->stopProcess = true;
                        }
                        else
                            second = val;
                    }
                }
            }
        }
        else if (regex_match(novi, clasic_literal))
        {
            one = true;
            if (novi == "")
            {
                this->op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
            }
            sa = '4';
            second = novi;
            first = "F";
        }
        else if (regex_match(novi, clasic_symbol))
        {
            one = true;
            sa = '4';

            string val = "";
            for (auto tr : this->symbolTable)
                if (tr.name == novi)
                    val = to_string(tr.value);
            if (val == "")
            {
                this->op_code = UNDEFINDE_SYMBOL;
                printError(op_code, line);
                this->stopProcess = true;
            }
            else
                second = val;
        }
        else
        {
            this->op_code = SINTAX_ERROR;
            printError(op_code, line);
            this->stopProcess = true;
        }
    }
    else
    {
        this->op_code = SINTAX_ERROR;
        printError(op_code, line);
        this->stopProcess = true;
    }
}

void Asembler::parse_reg_instruction(string red, int &destination, int &source, bool one_read)
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
            op_code = REGISTER_OUT_OF_BOUNDS;
            printError(op_code, line);
            this->stopProcess = true;
            return;
        }

        if (!one_read)
        {
            if (regex_search(red, m, filter_from_instruction))
            {
                new_symbol = m.str(0);
                new_symbol = regex_replace(new_symbol, regex(" "), "");
                new_symbol = regex_replace(new_symbol, filter_from_add, "");

                src = stoi(new_symbol);
                if (src > 7)
                {
                    op_code = REGISTER_OUT_OF_BOUNDS;
                    printError(op_code, line);
                    this->stopProcess = true;
                    return;
                }

                destination = dest;
                source = src;
            }
            else
            {
                op_code = SINTAX_ERROR;
                printError(op_code, line);
                this->stopProcess = true;
                return;
            }
        }
        else
            destination = dest;
    }
    else
    {
        op_code = SINTAX_ERROR;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }
}

void Asembler::reg_instruction(int fa, int sa, string red)
{
    regex filter;
    if (fa == 6 && sa == 0)
    {
        filter = xchg_instr_filter;
    }
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
        case 0:
            filter = not_instr_filter;
            break;
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
    else if (fa == 1 && sa == 0)
        filter = int_instr_filter;

    string novi = regex_replace(red, filter, "");
    novi = regex_replace(novi, regex(" "), "");

    int dest = 0, src = 0;
    bool flag;
    (fa == 8 || fa == 1) &&sa == 0 ? flag = true : flag = false;
    this->parse_reg_instruction(novi, dest, src, flag);

    if (this->stopProcess)
    {
        return;
    }

    this->for_write.push_back(*to_string(sa).c_str());
    this->for_write.push_back(*to_string(fa).c_str());
    if (!flag)
        this->for_write.push_back(*to_string(src).c_str());
    else if (fa != 1)
        this->for_write.push_back('0');
    else
        this->for_write.push_back('F');
    this->for_write.push_back(*to_string(dest).c_str());
    this->locationCounter += 4;
}

void Asembler::jmp_instruction(int fa, string red)
{
    regex filter;
    switch (fa)
    {
    case 0:
        filter = jmp_instr_filter;
        break;
    case 1:
        filter = jeq_instr_filter;
        break;
    case 2:
        filter = jne_instr_filter;
        break;
    case 3:
        filter = jgt_instr_filter;
        break;
    }

    string novi = regex_replace(red, filter, "");
    novi = regex_replace(novi, regex(" "), "");

    string prvi, drugi;
    char sa, ta, ca;
    bool one;
    this->jump_adressing(novi, prvi, drugi, one, sa);

    this->for_write.push_back(*to_string(fa).c_str());
    this->for_write.push_back('5');
    if (sa == '1' || sa == '2' || sa == '3' || sa == '5')
        this->for_write.push_back(*prvi.c_str());
    else
        this->for_write.push_back('F');
    this->for_write.push_back('F');
    this->for_write.push_back(sa);
    this->for_write.push_back('0');
    this->locationCounter += 6;
    if (!one)
    {
        ta = drugi[0];
        if (drugi.size() > 1)
        {
            ca = drugi[1];
            this->for_write.push_back(ca);
        }
        else
            this->for_write.push_back('0');
        this->for_write.push_back(ta);
        this->locationCounter += 2;
    }
}

void Asembler::ldr_instruction(string red)
{
    string novi = regex_replace(red, ldr_instr_filter, "");
    novi = regex_replace(novi, regex(" "), "");
    string first;

    smatch m;
    regex_search(novi, m, register_adressing);
    first = m.str(0);
    novi = regex_replace(novi, regex("(r|R)[0-9]+\\,"), "");
    novi = regex_replace(novi, regex("( )*\\,"), "");
    first = regex_replace(first, regex("(r|R)"), "");

    if (stoi(first) > 7 || first.size() > 1)
    {
        op_code = REGISTER_OUT_OF_BOUNDS;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }
    else
    {
        string prvi, drugi;
        char sa;
        bool one;
        this->data_adressing(novi, prvi, drugi, one, sa);

        char fa = *prvi.c_str(), ta, ca;
        this->for_write.push_back('0');
        this->for_write.push_back('A');
        if (sa == '5' || sa == '2' || sa == '3')
            this->for_write.push_back(fa);
        else
            this->for_write.push_back('F');
        this->for_write.push_back(*first.c_str());
        this->for_write.push_back(sa);
        this->for_write.push_back('0');
        this->locationCounter += 6;
        if (!(sa == '5' || sa == '2' || sa == '3'))
        {
            ta = drugi[0];
            if (drugi.size() > 1)
            {
                ca = drugi[1];
                this->for_write.push_back(ca);
            }
            else
                this->for_write.push_back('0');
            this->for_write.push_back(ta);
            this->locationCounter += 2;
        }
    }
}

void Asembler::str_instruction(string red)
{
    string novi = regex_replace(red, str_instr_filter, "");
    novi = regex_replace(novi, regex(" "), "");
    string first;

    smatch m;
    regex_search(novi, m, register_adressing);
    first = m.str(0);
    novi = regex_replace(novi, register_adressing, "");
    novi = regex_replace(novi, regex("( )*\\,"), "");
    first = regex_replace(first, regex("(r|R)"), "");

    if (stoi(first) > 7 || first.size() > 1)
    {
        op_code = REGISTER_OUT_OF_BOUNDS;
        printError(op_code, line);
        this->stopProcess = true;
        return;
    }
    else
    {
        string prvi, drugi;
        char sa;
        bool one;
        this->data_adressing(novi, prvi, drugi, one, sa);

        char fa = *prvi.c_str(), ta, ca;
        this->for_write.push_back('0');
        this->for_write.push_back('B');
        if (sa == '5' || sa == '2' || sa == '3')
            this->for_write.push_back(fa);
        else
            this->for_write.push_back('F');
        this->for_write.push_back(*first.c_str());
        this->for_write.push_back(sa);
        this->for_write.push_back('0');
        this->locationCounter += 6;
        if (!(sa == '5' || sa == '2' || sa == '3'))
        {
            ta = drugi[0];
            if (drugi.size() > 1)
            {
                ca = drugi[1];
                this->for_write.push_back(ca);
            }
            else
                this->for_write.push_back('0');
            this->for_write.push_back(ta);
            this->locationCounter += 2;
        }
    }
}

void Asembler::push_pop_instruction(string red, int code)
{
    string novi = regex_replace(red, push_pop_filter, "");
    novi = regex_replace(novi, regex(" "), "");
    if (novi == "")
    {
        op_code = SINTAX_ERROR;
        printError(op_code, this->line);
        this->stopProcess = true;
    }
    novi = regex_replace(novi, regex("(r|R)"), "");
    if (novi == "" || isalpha(novi[0]))
    {
        op_code = SINTAX_ERROR;
        printError(op_code, this->line);
        this->stopProcess = true;
        return;
    }
    else if (stoi(novi) > 7 || novi.size() > 1)
    {
        op_code = REGISTER_OUT_OF_BOUNDS;
        printError(op_code, this->line);
        this->stopProcess = true;
        return;
    }

    char c1, c2 = '0', c3 = *novi.c_str(), c4 = '6', c5, c6 = '2';

    if (code)
    {
        c1 = 'A';
        c5 = '4';
    }
    else
    {
        c1 = 'B';
        c5 = '1';
    }

    this->for_write.push_back(c2);
    this->for_write.push_back(c1);
    this->for_write.push_back(c4);
    this->for_write.push_back(c3);
    this->for_write.push_back(c6);
    this->for_write.push_back(c5);
    this->locationCounter += 6;
}

void Asembler::print_symbol_table()
{
    cout << endl
         << endl;
    cout << "=========================================Symbol Table==============================================" << endl;
    cout << "---------------------------------------------------------------------------------------------------" << endl;
    cout << "Name\t\tIs global\tNumber\t\tSection\t\tSize\t\tValue\t\t" << endl;
    cout << "---------------------------------------------------------------------------------------------------" << endl;
    cout << "UND\t\t0\t\t0\t\t0\t\t0\t\t0\t\t" << endl;
    for (Symbol a : this->symbolTable)
    {
        if (a.name.length() < 8)
            cout << a.name << "\t\t" << to_string(a.isGlobal) << "\t\t" << a.number << "\t\t" << a.seciton
                 << "\t\t" << a.size << "\t\t" << a.value << endl;
        else
            cout << a.name << "\t" << to_string(a.isGlobal) << "\t\t" << a.number << "\t\t" << a.seciton
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
                this->back_patching(s.name);
                break;
            }
            else
            {
                this->op_code = ALREDY_DEFINED;
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
        this->back_patching(s.name);
    }

    return s.number;
}

void Asembler::print_vector()
{
    this->output.write(this->for_write.data(), this->for_write.size());
    this->for_write.clear();
    this->locationCounter = 0;
}

void Asembler::back_patching(string simbol)
{
    int val = -1;
    if (this->backPatching[simbol].typeOfDefinition == 1)
    {
        for (auto i : this->symbolTable)
            if (i.name == simbol)
                val = i.value;
        if (val == -1)
            return;
        else
        {
            char vals[2];
            itoa(val, vals, 16);
            char c1 = vals[0], c2 = vals[1];
            this->for_write[this->backPatching[simbol].locationInCode] = c2;
            this->for_write[this->backPatching[simbol].locationInCode + 1] = c1;
        }
    }
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
                if (i->name == this->currentSection)
                    i->size = this->locationCounter;

            this->print_vector();
            break;
        }
    }
    if (this->stopProcess)
        return -1;
    else
        return 1;
}