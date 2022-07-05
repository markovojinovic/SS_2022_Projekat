#include "asembler.h"
#include <iostream>
#include <regex>
using namespace std;
int main()
{
    Asembler as("main.s", "main.o");
    as.next_instruction();
    as.next_instruction();
    as.next_instruction();
    as.next_instruction();
    as.next_instruction();
    as.next_instruction();
    as.next_instruction();
    as.next_instruction();
    as.next_instruction();

    as.print_symbol_table();
    cout << "==================================================";
    as.print_vector();

    return 0;
}

// 43:04
