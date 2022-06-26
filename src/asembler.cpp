#include "asembler.h"

int Asembler::next_operation()
{
    ifstream MyReadFile(file_name);
    if (!MyReadFile.is_open())
    {
        cout << "Nije otvoren fajl" << endl;
        return -1;
    }
}