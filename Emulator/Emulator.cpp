#include "Emulator.h"
#include "Asembler\OpCodeErrors.h"
#include <sstream>
#include <math.h>

using namespace std;

// max value in registers
const int max_val = 65535;
const int sp_init = 65278;

// fiksirani registri
const int pc = 7;
const int sp = 6;

// psw flegovi
const int n = 0;
const int c = 1;
const int o = 2;
const int z = 3;

// kodovi operacija instrukcija
const char *halt = "00";
const char *_int = "10";
const char *iret = "20";
const char *call_ = "30";
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
    this->pushPSW = false;
    this->popPSW = false;
    this->call = false;
    this->num = 0;
    this->input.open(this->input_name);
    for (int i = 0; i < 8; i++)
        this->registers[i] = 0;
    this->temp = 0;
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
        else if (strcmp(call_, code.c_str()) == 0)
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
        this->num++;
        if (this->stopProcess) // ovo ostviti
            return -1;
    }

    return 0;
}

void Emulator::exit_protocol()
{
    // TODO: kreiranje izlaznog fajla

    // /cout << "num = " << this->num << endl;
    cout << "--------------------------------------------" << endl
         << "Emulated processor executed halt instruction" << endl
         << "Emulated processor state:" << endl
         << "psw=0b001000000000" << this->psw[3] << this->psw[2] << this->psw[1] << this->psw[0] << endl
         << "r0=0x" << hex << this->registers[0] << endl
         << "r1=0x" << hex << this->registers[1] << endl
         << "r2=0x" << hex << this->registers[2] << endl
         << "r3=0x" << hex << this->registers[3] << endl
         << "r4=0x" << hex << this->registers[4] << endl
         << "r5=0x" << hex << this->registers[5] << endl
         << "r6=0x" << hex << this->registers[6] << endl
         << "r7=0x" << hex << this->registers[7] << endl;
    cout << "--------------------------------------------" << endl;
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
    // cout << "int_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_reg = "";
    arg_reg.push_back(arg[0]);
    int reg;
    stringstream geek(arg_reg);
    geek >> reg;
    if (reg > 7 || reg < 0)
    {
        printError(REGISTER_OUT_OF_BOUNDS, 0);
        this->stopProcess = true;
        return;
    }
    this->pushPSW = true;
    this->push(0);
    this->push(pc);
    int adr = (this->registers[reg] % 8) * 2;
    string num = "";
    num.append(this->memory[adr]);
    num.append(this->memory[adr + 1]);
    this->registers[pc] = this->lit_end_hex_to_int(num);
}

void Emulator::iret_instruction()
{
    // cout << "iret_instruction" << endl;
    this->pop(pc);
    this->popPSW = true;
    this->pop(0);
}

void Emulator::call_instruction()
{
    // cout << "call_instruction" << endl;
    this->call = true;
    this->jmp_instruction();
}

void Emulator::ret_instruction()
{
    // cout << "ret_instruction" << endl;
    this->pop(pc);
}

void Emulator::jmp_instruction()
{
    // cout << "jmp_instruction" << endl;
    string arg = this->memory[registers[pc]++];
    string adr = this->memory[registers[pc]++];

    if (adr[1] == immed)
    {
        // neposredno adresiranje
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        if (this->call)
        {
            this->call = false;
            this->push(pc);
        }
        this->registers[pc] = oper_val;
    }
    else if (adr[1] == regdir)
    {
        // registarsko direktno
        string arg_regS = "";
        arg_regS.push_back(arg[1]);
        int regS;
        stringstream geek(arg_regS);
        geek >> regS;
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        if (this->call)
        {
            this->call = false;
            this->push(pc);
        }
        this->registers[pc] = this->registers[regS];
    }
    else if (adr[1] == regdirmov)
    {
        // registarsko direktno sa pomerajem
        string arg_regS = "";
        arg_regS.push_back(arg[1]);
        int regS;
        stringstream geek(arg_regS);
        geek >> regS;
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        if (this->call)
        {
            this->call = false;
            this->push(pc);
        }
        if (regS == pc)
        {
            this->registers[pc] = oper_val;
        }
        else
            this->registers[pc] = this->registers[regS] + oper_val;
    }
    else if (adr[1] == regind)
    {
        // registarsko indirektno
        string arg_regS = "";
        arg_regS.push_back(arg[1]);
        int regS;
        stringstream geek(arg_regS);
        geek >> regS;
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        string num = "";
        num.append(this->memory[this->registers[regS]]);
        num.append(this->memory[this->registers[regS] + 1]);
        int val = this->lit_end_hex_to_int(num);
        if (this->call)
        {
            this->call = false;
            this->push(pc);
        }
        this->registers[pc] = val;
    }
    else if (adr[1] == regindmov)
    {
        // registarsko indirektno sa pomerajem
        string arg_regS = "";
        arg_regS.push_back(arg[1]);
        int regS;
        stringstream geek(arg_regS);
        geek >> regS;
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        int val = this->registers[regS];
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        string num = "";
        num.append(this->memory[oper_val + val]);
        num.append(this->memory[oper_val + val + 1]);
        int val_ = this->lit_end_hex_to_int(num);
        if (this->call)
        {
            this->call = false;
            this->push(pc);
        }
        this->registers[pc] = val_;
    }
    else if (adr[1] == mem)
    {
        // memorijsko
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        string num = "";
        num.append(this->memory[oper_val]);
        num.append(this->memory[oper_val + 1]);
        int val = this->lit_end_hex_to_int(num);
        if (this->call)
        {
            this->call = false;
            this->push(pc);
        }
        this->registers[pc] = val;
    }
    else
    {
        printError(UNDEFONED_ADRESSING, 0);
        this->stopProcess = true;
        return;
    }
}

void Emulator::jeq_instruction()
{
    // cout << "jeq_instruction" << endl;
    if (this->psw[z] == '0')
        this->jmp_instruction();
}

void Emulator::jne_instruction()
{
    // cout << "jne_instruction" << endl;
    if (this->psw[z] != '0')
        this->jmp_instruction();
}

void Emulator::jgt_instruction()
{
    // cout << "jgt_instruction" << endl;
    if (this->psw[z] != '0' && this->psw[n] != '0')
        this->jmp_instruction();
}

void Emulator::xchg_instruction()
{
    // cout << "xchg_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->temp = this->registers[regD];
    this->registers[regD] = this->registers[regS];
    this->registers[regS] = this->temp;
}

void Emulator::add_instruction()
{
    // cout << "add_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->registers[regD] += this->registers[regS];
}

void Emulator::sub_instruction()
{
    // cout << "sub_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->registers[regD] -= this->registers[regS];
}

void Emulator::mul_instruction()
{
    // cout << "mul_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->registers[regD] *= this->registers[regS];
}

void Emulator::div_instruction()
{
    // cout << "div_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    if (this->registers[regS] == 0)
    {
        printError(ZERO_DIV, 0);
        this->stopProcess = true;
        return;
    }
    this->registers[regD] /= this->registers[regS];
}

void Emulator::cmp_instruction()
{
    // cout << "cmp_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;

    this->temp = this->registers[regD] - this->registers[regS];

    if (temp == 0)
        this->psw[z] = '1';
    if (temp < 0)
        this->psw[n] = '1';
    if (this->temp < -max_val || this->temp > max_val)
    {
        this->psw[c] = '1';
        this->psw[o] = '1';
    }
}

void Emulator::not_instruction()
{
    // cout << "not_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->registers[regD] == 0 ? this->registers[regD] = 1 : this->registers[regD] = 0;
}

void Emulator::and_instruction()
{
    // cout << "and_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->registers[regD] &= this->registers[regS];
}

void Emulator::or_instruction()
{
    // cout << "or_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->registers[regD] |= this->registers[regS];
}

void Emulator::xor_instruction()
{
    // cout << "xor_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->registers[regD] ^= this->registers[regS];
}

void Emulator::test_instruction()
{
    // cout << "test_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    this->temp = this->registers[regD] & this->registers[regS];
    if (temp == 0)
        this->psw[z] = '1';
    if (temp < 0)
        this->psw[n] = '1';
}

void Emulator::shl_instruction()
{
    // cout << "shl_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;

    int rez = -1;
    if (this->registers[regS] > 0)
    {
        int flag = pow(2, this->registers[regS]) - 1;
        flag <<= 16 - this->registers[regS];
        rez = this->registers[regD] & flag;
    }

    this->registers[regD] <<= this->registers[regS];

    // updt psw
    if (this->registers[regD] == 0)
        this->psw[z] = '1';
    if (this->registers[regD] < 0)
        this->psw[n] = '1';
    if (rez > 0 && rez != -1)
        this->psw[c] = '1';
}

void Emulator::shr_instruction()
{
    // cout << "shr_instruction" << endl;
    string arg = this->memory[this->registers[pc]++];
    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;

    int rez = -1;
    if (this->registers[regS] > 0)
    {
        int flag = pow(2, this->registers[regS]) - 1;
        rez = this->registers[regD] & flag;
    }

    this->registers[regD] >>= this->registers[regS];

    // updt psw
    if (this->registers[regD] == 0)
        this->psw[z] = '1';
    if (this->registers[regD] < 0)
        this->psw[n] = '1';
    if (rez > 0 && rez != -1)
        this->psw[c] = '1';
}

void Emulator::ldr_instruction()
{
    // cout << "ldr_instruction" << endl;
    string arg = this->memory[registers[pc]++];
    string adr = this->memory[registers[pc]++];

    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;

    if (adr[1] == immed)
    {
        // neposredno adresiranje
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        this->registers[regD] = oper_val;
    }
    else if (adr[1] == regdir)
    {
        // registarsko direktno
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        this->registers[regD] = this->registers[regS];
    }
    else if (adr[1] == regdirmov)
    {
        // registarsko direktno sa pomerajem
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        this->registers[regD] = this->registers[regS] + oper_val;
    }
    else if (adr[1] == regind)
    {
        // registarsko indirektno
        if (adr[0] == postinc)
        {
            // pop funckija
            string num = "";
            num.append(this->memory[++this->registers[sp]]);
            num.append(this->memory[++this->registers[sp]]);
            int val = this->lit_end_hex_to_int(num);
            this->registers[regD] = val;
        }
        else
        {
            if (regS > 7 || regS < 0)
            {
                printError(REGISTER_OUT_OF_BOUNDS, 0);
                this->stopProcess = true;
                return;
            }
            string num = "";
            num.append(this->memory[this->registers[regS]]);
            num.append(this->memory[this->registers[regS] + 1]);
            int val = this->lit_end_hex_to_int(num);
            this->registers[regD] = val;
        }
    }
    else if (adr[1] == regindmov)
    {
        // registarsko indirektno sa pomerajem
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        // if (this->num == 74 || this->num == 75)
        //     oper_val++;

        string stack = "";
        for (int i = this->registers[sp]; i <= sp_init; i++)
        {
            stack += this->memory[i];
            // stack += " ";
        }

        int st = this->registers[regS];
        string fir = this->memory[this->registers[regS]];
        string fir_ = this->memory[this->registers[regS] + oper_val + 1];
        string sec_ = this->memory[this->registers[regS] + oper_val];

        string num = "";
        num.append(this->memory[this->registers[regS] + oper_val + 1]); // ovde je ispravka
        num.append(this->memory[this->registers[regS] + oper_val + 2]);
        int val = this->lit_end_hex_to_int(num);
        this->registers[regD] = val;
    }
    else if (adr[1] == mem)
    {
        // memorijsko
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);
        string num = "";
        num.append(this->memory[oper_val]);
        num.append(this->memory[oper_val + 1]);
        int val = this->lit_end_hex_to_int(num);
        this->registers[regD] = val;
    }
    else
    {
        printError(UNDEFONED_ADRESSING, 0);
        this->stopProcess = true;
        return;
    }
}

void Emulator::str_instruction()
{
    // cout << "str_instruction" << endl;
    string arg = this->memory[registers[pc]++];
    string adr = this->memory[registers[pc]++];

    string arg_regD = "", arg_regS = "";
    arg_regD.push_back(arg[0]);
    arg_regS.push_back(arg[1]);
    int regD, regS;
    stringstream geek(arg_regS);
    geek >> regS;
    stringstream geek1(arg_regD);
    geek1 >> regD;
    if (regD > 7 || regD < 0)
    {
        printError(REGISTER_OUT_OF_BOUNDS, 0);
        this->stopProcess = true;
        return;
    }

    if (adr[1] == regdir)
    {
        // registarsko direktno
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        this->registers[regS] = this->registers[regD];
    }
    else if (adr[1] == regind)
    {
        // registarsko indirektno
        if (adr[0] == predec)
        {
            // push funkcija
            string mem = "";

            mem = this->int_to_hex_lit_end(this->registers[regD]);
            if (mem.size() < 4)
                for (int i = mem.size() + 1; i < 5; i++)
                    mem.insert(mem.begin(), '0');

            string low = "";
            low.push_back(mem[0]);
            low.push_back(mem[1]);
            string heigh = "";
            heigh.push_back(mem[2]);
            heigh.push_back(mem[3]);

            this->memory[this->registers[sp]--] = heigh;
            this->memory[this->registers[sp]--] = low;

            string dodato = "";
            dodato += this->memory[this->registers[sp] + 1];
            dodato += this->memory[this->registers[sp] + 2];
        }
        else
        {
            if (regS > 7 || regS < 0)
            {
                printError(REGISTER_OUT_OF_BOUNDS, 0);
                this->stopProcess = true;
                return;
            }
            int val = this->registers[regD];
            string hex_val = this->int_to_hex_lit_end(val);
            string low = "";
            low.push_back(hex_val[0]);
            low.push_back(hex_val[1]);
            string heigh = "";
            heigh.push_back(hex_val[2]);
            heigh.push_back(hex_val[3]);

            this->memory[this->registers[regS]] = low;
            this->memory[this->registers[regS] + 1] = heigh;
        }
    }
    else if (adr[1] == regindmov)
    {
        // registarsko indirektno sa pomerajem
        if (regS > 7 || regS < 0)
        {
            printError(REGISTER_OUT_OF_BOUNDS, 0);
            this->stopProcess = true;
            return;
        }
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);

        int val = this->registers[regD];
        string hex_val = this->int_to_hex_lit_end(val);
        string low = "";
        low.push_back(hex_val[0]);
        low.push_back(hex_val[1]);
        string heigh = "";
        heigh.push_back(hex_val[2]);
        heigh.push_back(hex_val[3]);

        this->memory[this->registers[regS] + oper_val] = low;
        this->memory[this->registers[regS] + oper_val + 1] = heigh;
    }
    else if (adr[1] == mem)
    {
        // memorijsko
        string oper = this->get_number();
        int oper_val = this->lit_end_hex_to_int(oper);

        int val = this->registers[regD];
        string hex_val = this->int_to_hex_lit_end(val);
        string low = "";
        low.push_back(hex_val[0]);
        low.push_back(hex_val[1]);
        string heigh = "";
        heigh.push_back(hex_val[2]);
        heigh.push_back(hex_val[3]);

        this->memory[oper_val] = low;
        this->memory[oper_val + 1] = heigh;
    }
    else
    {
        printError(UNDEFONED_ADRESSING, 0);
        this->stopProcess = true;
        return;
    }
}

void Emulator::push(int reg)
{
    string mem = "";
    if (this->pushPSW)
    {
        this->pushPSW = false;
        mem = psw;
    }
    else
    {
        mem = this->int_to_hex_lit_end(this->registers[reg]);
        if (mem.size() < 4)
            for (int i = mem.size() + 1; i < 5; i++)
                mem.insert(mem.begin(), '0');
    }

    string low = "";
    low.push_back(mem[0]);
    low.push_back(mem[1]);
    string heigh = "";
    heigh.push_back(mem[2]);
    heigh.push_back(mem[3]);

    this->memory[this->registers[sp]--] = heigh;
    this->memory[this->registers[sp]--] = low;
}

void Emulator::pop(int reg)
{
    string num = "";
    num.append(this->memory[++this->registers[sp]]);
    num.append(this->memory[++this->registers[sp]]);

    if (this->popPSW)
    {
        this->popPSW = false;
        this->psw = num;
    }
    else
    {
        int val = this->lit_end_hex_to_int(num);
        this->registers[reg] = val;
    }
}

string Emulator::get_number()
{
    string num = "";
    num.append(this->memory[this->registers[pc]++]);
    num.append(this->memory[this->registers[pc]++]);
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
    ss << hex << num;
    string result = ss.str();

    if (result.size() > 4)
        return "FFFF";

    if (result.size() < 4)
        for (int i = result.size() + 1; i < 5; i++)
            result.insert(result.begin(), '0');

    char c0 = result[0];
    char c1 = result[1];
    result[0] = result[2];
    result[1] = result[3];
    result[2] = c0;
    result[3] = c1;

    return result;
}