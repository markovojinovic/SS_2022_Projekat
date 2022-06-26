#include "asembler.h"
#include <iostream>
using namespace std;
int main()
{
    Asembler as("main.s", "neki_izlaz");
    as.next_instruction();

    return 0;
}
