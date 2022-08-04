#ifndef _OPCODEERRORS_H_
#define _OPCODEERRORS_H_

#include <string>

// OPERATION CODES
const int FILE_ERROR = -2;
const int UNDEFINED_INSTRUCTION = -3;
const int EXPORT_IMPORT_ERROR = -4;
const int ALREDY_LOCAL = -5;
const int ALREDY_DEFINED = -6;
const int OUT_OF_SECTION = -7;
const int DOT_START = -9;
const int SINTAX_ERROR = -10;
const int REGISTER_OUT_OF_BOUNDS = -11;
const int UNDEFINED_SYMBOL = -12;

// EROOR SCHEMA

// op_code = CODE_OF_INSTRUCTION;
// printError(op_code, this->line);
// this->stopProcess = true;

void printError(int code, int line)
{
    switch (code)
    {
    case FILE_ERROR:
        cout << "Operation code: " << code << " = Greska pri otvaranju fajla!!!" << endl;
        break;
    case UNDEFINED_INSTRUCTION:
        cout << "Operation code: " << code << " = Nedefinisana asemblerka naredba!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case EXPORT_IMPORT_ERROR:
        cout << "Operation code: " << code << " = Isti simbol i izvezen i uvezen!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case ALREDY_LOCAL:
        cout << "Operation code: " << code << " = Simbol je vec definisan lokalno!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case ALREDY_DEFINED:
        cout << "Operation code: " << code << " = Simbol je vec definisan!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case OUT_OF_SECTION:
        cout << "Operation code: " << code << " = Naredba ne moze biti definisana van '.section' dela!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case DOT_START:
        cout << "Operation code: " << code << " = Naredba ne moze pocinjati tackom, osim ako nije direktiva!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case SINTAX_ERROR:
        cout << "Operation code: " << code << " = Sintaksna greska asemblerske naredbe!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case REGISTER_OUT_OF_BOUNDS:
        cout << "Operation code: " << code << " = Registar nije u opsegu od 0 do 7!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    case UNDEFINED_SYMBOL:
        cout << "Operation code: " << code << " = Simbol nije definisan!!!" << endl;
        cout << "Line: " << line << endl;
        break;
    default:
        break;
    }
}

// Dodatno: -1 preskoceno kao univerzalni kod greske
// Dodatno: -8 Stopiranje procesa prevodjenja

#endif