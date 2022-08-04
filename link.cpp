#include <iostream>
#include "linker.h"
#include <regex>

using namespace std;

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "-o") == 0)
    {
        list<string> input;
        for (int i = 2; i < argc; i++)
            input.push_back(argv[i]);
        // Stvaranje objekta i pokretanje procesa
        // exit protocol
    }
    else
    {
        cout << "Nedefinisana ulazna komanda" << endl;
        return -1;
    }
    return 0;
}