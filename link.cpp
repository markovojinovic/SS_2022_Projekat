#include <iostream>
#include "linker.h"
#include <regex>

using namespace std;
// int argc, char *argv[]
int main(int argc, char *argv[])
{
    // char *argv[] = {
    //     "main.exe",
    //     "-o",
    //     "program.hex",
    //     "ivt.o",
    //     "math.o",
    //     "main.o",
    //     "isr_reset.o",
    //     "isr_terminal.o",
    //     "isr_timer.o",
    //     "isr_user0.o",
    // };
    // int argc = 10;
    if (strcmp(argv[1], "-o") == 0)
    {
        vector<string> input;
        for (int i = 3; i < argc; i++)
            input.push_back(argv[i]);
        linker lk(input, argv[2]);
        if (lk.start_reading() == 0)
        {
            lk.exit_protocol();
        }
        else
        {
            cout << "Prevodjenje prekinuto zbog greske!!!" << endl;
        }
    }
    else
    {
        cout << "Nedefinisana ulazna komanda" << endl;
        return -1;
    }
    return 0;
}
// destinations iz main fajla vidno ispada iz opsega, a jednom se uvecava
// TODO: obraditi relativne pomeraje ( ona druga vrsta rel zapisa )

// 31:50