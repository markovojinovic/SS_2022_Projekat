#ifndef _OPCODEERRORS_H_
#define _OPCODEERRORS_H_

#include <string>

string ret_op_code(int code)
{
    switch (code)
    {
    case -2:
        return " - Greska pri otvaranju fajla!!!";
    case -3:
        return " - Nedefinisana asemblerska naredba na liniji!!!";

    default:
        break;
    }
    return "";
}

#endif