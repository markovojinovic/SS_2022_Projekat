#ifndef _OPCODEERRORS_H_
#define _OPCODEERRORS_H_

#include <string>

void printError(int code, int line)
{
    switch (code)
    {
    case -2:
        cout << "Operation code: " << code << " Greska pri otvaranju fajla!!!" << endl;
        break;
    case -3:
        cout << "Operation code: " << code << " Nedefinisana asemblerka naredba!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case -4:
        cout << "Operation code: " << code << " Isti simbol i izvezen i uvezen!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case -5:
        cout << "Operation code: " << code << " Simbol je vec definisan lokalno!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case -6:
        cout << "Operation code: " << code << " Simbol je vec definisan!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case -7:
        cout << "Operation code: " << code << " Naredba ne moze biti definisana van '.section' dela!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    default:
        break;
    }
}

// Dodatno: -1 preskoceno kao univerzalni kod greske

#endif