#ifndef _ASEMBLER_H_
#define _ASEMBLER_H_

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const string file_name = "main.s";

class Asembler
{
public:
    static int next_operation();
};

#endif