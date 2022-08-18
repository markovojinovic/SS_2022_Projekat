#ifndef EMULATOR_H
#define EMULATOR_H

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

using namespace std;

class Emulator
{
public:
    Emulator(string);
    int start_reading();
    void exit_protocol();
    ~Emulator();

private:
    void load_memory();
    int lit_end_hex_to_int(string);

    string input_name;
    ifstream input;
    unordered_map<int, string> memory;
    int registers[8];
    string psw = "0000000000000000";
};

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
const string halt = "00";
const string _int = "10";
const string iret = "20";
const string call = "30";
const string ret = "40";
const string jmp = "50";
const string jeq = "51";
const string jne = "52";
const string jgt = "53";
const string xchg = "60";
const string add = "70";
const string sub = "71";
const string mul = "72";
const string _div = "73";
const string cmp = "74";
const string _not = "80";
const string _and = "81";
const string _or = "82";
const string _xor = "83";
const string test = "84";
const string shl = "90";
const string shr = "91";
const string ldr = "A0";
const string str = "B0";

// adresiranja
const string immed = "0";
const string regdir = "1";
const string regdirmov = "5";
const string regind = "2";
const string regindmov = "3";
const string mem = "4";

// azuriranja
const string no_upd = "0";
const string predec = "1";
const string preinc = "2";
const string postdec = "3";
const string postinc = "4";

#endif