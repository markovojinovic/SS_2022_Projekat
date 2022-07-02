#include "asembler.h"
#include <iostream>
#include <regex>
using namespace std;
int main()
{
    Asembler as("main.s", "neki_izlaz");
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

    return 0;
}
