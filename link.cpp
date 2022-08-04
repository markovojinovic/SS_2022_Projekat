#include <iostream>
#include "linker.h"
#include <regex>

using namespace std;

int main(int argc, char *argv[])
{
    // char *argv[] = {"main.exe",
    //                 "-o",
    //                 "main.o",
    //                 "ivt.o",
    //                 "isr_reset.o"};
    // argc = 5;
    if (strcmp(argv[1], "-o") == 0)
    {
        vector<string> input;
        for (int i = 2; i < argc; i++)
            input.push_back(argv[i]);
        linker lk(input);
        lk.start_reading(); // Potencijalno dodati uslov za ispis ako se metoda izvrsila do kraja
        lk.exit_protocol();
    }
    else
    {
        cout << "Nedefinisana ulazna komanda" << endl;
        return -1;
    }
    return 0;
}

// 31:50