#include "asembler.h"
#include <iostream>
#include <regex>
using namespace std;
int main()
{
    Asembler as("main.s", "neki_izlaz");
    as.next_instruction();
    as.next_instruction();
    int rez = as.next_instruction();
    if (rez == 3)
        cout << "Jeste" << endl;
    else
        cout << "Nije" << endl;

    return 0;
}
