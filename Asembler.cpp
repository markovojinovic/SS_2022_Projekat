#include "asembler.h"

Asembler::Asembler(string in_name, string out_name)
{
    this->input_name = in_name;
    this->output_name = out_name;

    line = 0;

    fajl.open(input_name);
    if (!fajl || !fajl.is_open())
    {
        op_code = -2;
        cout << "Operation code: " << op_code << " - Greska pri otvaranju fajla!!!" << endl;
    }
}

Asembler::~Asembler()
{
    this->fajl.close();
}

int Asembler::next_operation()
{
    string red;
    getline(this->fajl, red);
    if (red != "")
        this->line++;

    // get_code_of_operation();
    // switch za pozivanje funkcija ( u zavisnosti koja je asemblerska naredba )

    return 0; // treba da se vrati kod operacije od 1 - koliko ima naredbi
              // Ako je nedefinisana naredba, op_code = -3, i stampa se u kom je redu grska
}