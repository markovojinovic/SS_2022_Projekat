#include "Emulator.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        Emulator em(argv[1]);
        if (em.start_reading() == 0)
        {
            em.exit_protocol();
        }
        else
        {
            cout << "Prevodjenje prekinuto zbog greske!!!" << endl;
        }
    }
    else
    {
        cout << "Nedefinisana ulazna komanda" << endl;
        return -1;
    }

    // Emulator em("program.hex");
    // if (em.start_reading() == 0)
    // {
    //     em.exit_protocol();
    // }
    // else
    // {
    //     cout << "Prevodjenje prekinuto zbog greske!!!" << endl;
    // }

    return 0;
}