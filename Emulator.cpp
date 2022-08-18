#include "Emulator.h"
#include "Asembler\OpCodeErrors.h"
#include <sstream>

using namespace std;

Emulator::Emulator(string input)
{
    this->input_name = input;
    this->input.open(this->input_name);
    for (int i = 0; i < 8; i++)
        this->registers[i] = 0;
    if (!this->input.is_open() || !this->input)
        printError(FILE_ERROR, 0);
}

Emulator::~Emulator()
{
}

int Emulator::start_reading()
{
    this->load_memory();
    // citanje iz memorije
    // obradjivanje slucaja
    // prekidanje ako je halt
    // pozivanje odgovarajuce funkcije
    return 0;
}

void Emulator::exit_protocol()
{
}

void Emulator::load_memory()
{
    string red;
    int adresa = 0;
    while (getline(this->input, red))
    {
        for (int i = 6; i < red.length(); i += 3)
        {
            string dat = "";
            dat.push_back(red[i]);
            dat.push_back(red[i + 1]);
            this->memory[adresa++] = dat;
        }
    }

    string num = "";
    num.append(this->memory[0]);
    num.append(this->memory[1]);
    this->registers[pc] = this->lit_end_hex_to_int(num);

    this->input.close();
}

int Emulator::lit_end_hex_to_int(string num)
{
    int ret = 0;
    string first = "";
    string second = "";
    first.push_back(num[2]);
    first.push_back(num[3]);
    second.push_back(num[0]);
    second.push_back(num[1]);
    num = "";
    num.append(first);
    num.append(second);

    istringstream iss(num);
    iss >> hex >> ret;

    return ret;
}