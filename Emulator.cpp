#include "Emulator.h"
#include "Asembler\OpCodeErrors.h"
#include <sstream>

using namespace std;

// max value in registers
const int max_val = 65535;

// fiksirani registri
const int pc = 7;
const int sp = 6;

// psw flegovi
const int n = 3;
const int c = 2;
const int o = 1;
const int z = 0;
const int i = 15;

// kodovi operacija instrukcija
const char *halt = "00";
const char *_int = "10";
const char *iret = "20";
const char *call = "30";
const char *ret = "40";
const char *jmp = "50";
const char *jeq = "51";
const char *jne = "52";
const char *jgt = "53";
const char *xchg = "60";
const char *add = "70";
const char *sub = "71";
const char *mul = "72";
const char *_div = "73";
const char *cmp = "74";
const char *_not = "80";
const char *_and = "81";
const char *_or = "82";
const char *_xor = "83";
const char *test = "84";
const char *shl = "90";
const char *shr = "91";
const char *ldr = "A0";
const char *str = "B0";

// adresiranja
const char immed = '0';
const char regdir = '1';
const char regdirmov = '5';
const char regind = '2';
const char regindmov = '3';
const char mem = '4';

// azuriranja
const char no_upd = '0';
const char predec = '1';
const char preinc = '2';
const char postdec = '3';
const char postinc = '4';

Emulator::Emulator(string input)
{
    this->input_name = input;
    this->stopProcess = false;
    this->input.open(this->input_name);
    for (int i = 0; i < 8; i++)
        this->registers[i] = 0;
    if (!this->input.is_open() || !this->input)
        printError(FILE_ERROR, 0);
}

Emulator::~Emulator()
{
}

int Emulator::start_reading()
{
    this->load_memory();

    bool halt_cond = false;
    while (!halt_cond)
    {
        string code = this->memory[this->registers[pc]];
        this->registers[pc]++;

        if (strcmp(halt, code.c_str()) == 0)
            halt_cond = true;
        else if (strcmp(_int, code.c_str()) == 0)
            this->int_instruction();
        else if (strcmp(iret, code.c_str()) == 0)
            this->iret_instruction();
        else if (strcmp(call, code.c_str()) == 0)
            this->call_instruction();
        else if (strcmp(ret, code.c_str()) == 0)
            this->ret_instruction();
        else if (strcmp(jmp, code.c_str()) == 0)
            this->jmp_instruction();
        else if (strcmp(jeq, code.c_str()) == 0)
            this->jeq_instruction();
        else if (strcmp(jne, code.c_str()) == 0)
            this->jne_instruction();
        else if (strcmp(jgt, code.c_str()) == 0)
            this->jgt_instruction();
        else if (strcmp(xchg, code.c_str()) == 0)
            this->xchg_instruction();
        else if (strcmp(add, code.c_str()) == 0)
            this->add_instruction();
        else if (strcmp(sub, code.c_str()) == 0)
            this->sub_instruction();
        else if (strcmp(mul, code.c_str()) == 0)
            this->mul_instruction();
        else if (strcmp(_div, code.c_str()) == 0)
            this->div_instruction();
        else if (strcmp(cmp, code.c_str()) == 0)
            this->cmp_instruction();
        else if (strcmp(_not, code.c_str()) == 0)
            this->not_instruction();
        else if (strcmp(_and, code.c_str()) == 0)
            this->and_instruction();
        else if (strcmp(_or, code.c_str()) == 0)
            this->or_instruction();
        else if (strcmp(_xor, code.c_str()) == 0)
            this->xor_instruction();
        else if (strcmp(test, code.c_str()) == 0)
            this->test_instruction();
        else if (strcmp(shl, code.c_str()) == 0)
            this->shl_instruction();
        else if (strcmp(shr, code.c_str()) == 0)
            this->shr_instruction();
        else if (strcmp(ldr, code.c_str()) == 0)
            this->ldr_instruction();
        else if (strcmp(str, code.c_str()) == 0)
            this->str_instruction();
        else
        {
            printError(UNDEFINED_INSTRUCTION, 0);
            return -1;
        }

        return 0;
    }
}

void Emulator::exit_protocol()
{
    // TODO: kreiranje izlaznog fajla
}

void Emulator::load_memory()
{
    string red;
    int adresa = 0;
    while (getline(this->input, red))
    {
        for (int i = 6; i < red.length(); i += 3)
        {
            string dat = "";
            dat.push_back(red[i]);
            dat.push_back(red[i + 1]);
            this->memory[adresa++] = dat;
        }
    }

    this->registers[pc] = this->lit_end_hex_to_int(this->get_number());

    this->input.close();
}

void Emulator::int_instruction()
{
}

void Emulator::iret_instruction()
{
}

void Emulator::call_instruction()
{
}

void Emulator::ret_instruction()
{
}

void Emulator::jmp_instruction()
{
}

void Emulator::jeq_instruction()
{
}

void Emulator::jne_instruction()
{
}

void Emulator::jgt_instruction()
{
}

void Emulator::xchg_instruction()
{
}

void Emulator::add_instruction()
{
}

void Emulator::sub_instruction()
{
}

void Emulator::mul_instruction()
{
}

void Emulator::div_instruction()
{
}

void Emulator::cmp_instruction()
{
}

void Emulator::not_instruction()
{
}

void Emulator::and_instruction()
{
}

void Emulator::or_instruction()
{
}

void Emulator::xor_instruction()
{
}

void Emulator::test_instruction()
{
}

void Emulator::shl_instruction()
{
}

void Emulator::shr_instruction()
{
}

void Emulator::ldr_instruction()
{
}

void Emulator::str_instruction()
{
}

void Emulator::push(int reg) // TODO: provera
{
    string mem = this->int_to_hex_lit_end(this->registers[reg]);

    string low = "";
    low.push_back(mem[0]);
    low.push_back(mem[1]);
    string heigh = "";
    heigh.push_back(mem[2]);
    heigh.push_back(mem[3]);

    this->registers[sp]--;
    this->memory[this->registers[sp]--] = heigh;
    this->memory[this->registers[sp]] = low;
}

void Emulator::pop(int reg) // TODO: provera
{
    string num = "";
    num.append(this->memory[this->registers[sp]++]);
    num.append(this->memory[this->registers[sp]++]);

    int val = this->lit_end_hex_to_int(num);
    this->registers[reg] = val;
}

string Emulator::get_number()
{
    string num = "";
    num.append(this->memory[this->registers[pc]]);
    num.append(this->memory[this->registers[pc] + 1]);
    return num;
}

int Emulator::lit_end_hex_to_int(string num)
{
    int ret = 0;
    string first = "";
    string second = "";
    first.push_back(num[2]);
    first.push_back(num[3]);
    second.push_back(num[0]);
    second.push_back(num[1]);
    num = "";
    num.append(first);
    num.append(second);

    istringstream iss(num);
    iss >> hex >> ret;

    return ret;
}

string Emulator::int_to_hex_lit_end(int num)
{
    ostringstream ss;
    ss << hex << i;
    string result = ss.str();

    if (result.size() > 4)
        return "FFFF";

    string first = "";
    string second = "";
    first.push_back(result[2]);
    first.push_back(result[3]);
    second.push_back(result[0]);
    second.push_back(result[1]);
    result = "";
    result.append(first);
    result.append(second);

    return result;
}