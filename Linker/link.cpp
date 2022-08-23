#include <iostream>
#include "linker.h"
#include <regex>

using namespace std;

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "-hex") == 0 && strcmp(argv[2], "-o") == 0)
    {
        vector<string> input;
        for (int i = 4; i < argc; i++)
            input.push_back(argv[i]);
        linker lk(input, argv[3]);
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