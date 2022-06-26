#include "asembler.h"
#include "Regeksi.h"
#include "OpCodeErrors.h"

Asembler::Asembler(string in_name, string out_name)
{
    this->input_name = in_name;
    this->output_name = out_name;

    line = 0;

    file.open(input_name);
    if (!file || !file.is_open())
    {
        op_code = -2;
        cout << "Operation code: " << op_code << ret_op_code(op_code) << endl;
    }
}

Asembler::~Asembler()
{
    this->file.close();
}

int Asembler::next_instruction()
{
    string red;
    getline(this->file, red);
    if (red != "")
        this->line++;

    int rez = get_code_of_instriction(red);
    // switch za pozivanje funkcija ( u zavisnosti koja je asemblerska naredba )

    if (rez > 0)
        return rez;
    else
    {
        op_code = -3;
        return op_code;
    }
}

int Asembler::get_code_of_instriction(string red)
{
    if (regex_match(red, rx_comments))
        return 1;
    return -3;
}