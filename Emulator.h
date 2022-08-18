#ifndef EMULATOR_H
#define EMULATOR_H

#include <iostream>
#include <string>
#include <string.h>
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
    string int_to_hex_lit_end(int);
    string get_number();

    void int_instruction();
    void iret_instruction();
    void call_instruction();
    void ret_instruction();
    void jmp_instruction();
    void jeq_instruction();
    void jne_instruction();
    void jgt_instruction();
    void xchg_instruction();
    void add_instruction();
    void sub_instruction();
    void mul_instruction();
    void div_instruction();
    void cmp_instruction();
    void not_instruction();
    void and_instruction();
    void or_instruction();
    void xor_instruction();
    void test_instruction();
    void shl_instruction();
    void shr_instruction();
    void ldr_instruction();
    void str_instruction();

    void push(int);
    void pop(int);

    string input_name;
    ifstream input;
    unordered_map<int, string> memory;
    int registers[8];
    string psw = "0000000000000000";
    bool stopProcess;
};

#endif