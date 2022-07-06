#include "asembler.h"
#include <iostream>
#include <regex>
using namespace std;
int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "-o") == 0)
    {
        Asembler as(argv[3], argv[2]);
        if (as.start_reading() == 1)
        {
            as.print_symbol_table();
        }
        else
            cout << "Zbog greske prevodjenje prekinuto!!!" << endl;
    }
    else
    {
        cout << "Nedefinisana ulazna komanda" << endl;
    }

    return 0;
}

// 52:12
