#include "asembler.h"
#include <iostream>
#include <fstream>

int Asembler::next_operation()
{
    ifstream fajl;
    fajl.open(file_name);
    if (!fajl || !fajl.is_open())
    {
        cout << "Nije se otvorio";
        return -1;
    }
    string red;
    getline(fajl, red);
    cout << red << endl;
    fajl.close();
    return 0;
}