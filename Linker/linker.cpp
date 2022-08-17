#include "linker.h"
#include "Asembler\OpCodeErrors.h"
#include <regex>

linker::linker(vector<string> input_n, string out)
{
    this->input_name = input_n;
    this->red = 0;
    this->stopProcess = false;
    this->output.open(out, ios_base::out | ios_base::binary);
    if (!this->output.is_open())
    {
        printError(FILE_ERROR, 0);
        cout << out << endl;
    }
}

linker::~linker()
{
    this->output.close();
}

int linker::start_reading()
{
    for (auto i : this->input_name)
    {
        input.open(i);
        if (!input.is_open() || !input)
        {
            printError(FILE_ERROR, 0);
            cout << i << endl;
            return -1;
        }
        this->currentInput = i;
        file_ novi(this->currentInput);
        this->red_files.push_back(novi);
        red_current();
        if (this->stopProcess)
            return -1;

        input.close();
    }
    this->obrada();
    // for (auto a : this->red_files)
    // {
    //     cout << a.ime << endl;
    //     for (auto b : a.relTable)
    //         cout << b.locationInCode << " ";
    //     cout << endl;
    //     if (a.sections.size() > 0)
    //         cout << "procitane sekcije" << endl;
    //     for (auto b : a.sections)
    //     {
    //         cout << b.ime << endl;
    //         for (auto c : b.code)
    //             cout << c;
    //         cout << endl;
    //     }
    // }

    for (auto a : this->red_files)
    {
        cout << a.ime << endl;
        cout << "--------" << endl;
        for (auto b : a.symTable)
            cout << b.name << " " << b.value << endl;
        cout << "------------------------" << endl;
    }
    return 0;
}

void linker::exit_protocol()
{
    this->output.write(this->for_write.data(), this->for_write.size());
    this->for_write.clear();
}

void linker::red_current()
{
    string name = "asembly_file_";
    for (auto a : this->currentInput)
        if (a != '.')
            name += a;
        else
            break;
    name += ".txt";
    ifstream curr;
    curr.open(name);
    if (!curr || !curr.is_open())
    {
        printError(FILE_ERROR, 0);
        cout << name << endl;
        this->stopProcess = true;
        return;
    }
    string red;
    string ime_dodatog;
    int br_dodatog = -1;
    int pomeraj = -1;

    // citanje dela fajla sa sekcijama
    while (getline(curr, red))
    {
        if (red == "-----=====-----")
            break;
        // citanje jedne sekcije
        string code;
        getline(curr, code);
        section nova(red);
        // provera da li postoji sekcija sa istim imenom
        bool same = false;
        for (auto i = this->red_files.begin(); i != this->red_files.end(); i++)
        {
            bool ret = false;
            for (auto a = i->sections.begin(); a != i->sections.end(); a++)
                if (a->ime == nova.ime)
                {
                    // obrada takvog slucaja
                    same = true;
                    pomeraj = a->code.size();
                    nova.pomeraj = pomeraj;
                    for (auto b : code)
                        a->code.push_back(b);
                    ime_dodatog = nova.ime;
                    nova.code.clear();
                    this->red_files.back().sections.push_back(nova);
                    ret = true;
                    break;
                }
            if (ret)
                break;
        }

        if (!same)
        {
            for (auto i : code)
                nova.code.push_back(i);
            this->red_files.back().sections.push_back(nova);
        }
    }

    // citanje dela fajla sa tabelama simbola
    while (getline(curr, red))
    {
        if (red == "-----=====-----")
            break;
        char nam[20];
        int glob, num, sec, size, val, ext;
        sscanf(red.c_str(), "%s\t%d\t%d\t%d\t%d\t%d\t%d", nam, &glob, &num, &sec, &size, &val, &ext);
        Symbol cur(nam, false, false, sec, val);
        cur.size = size;
        ext == 1 ? cur.isExtern = true : cur.isExtern = false;
        cur.number = num;
        cur.value = val;
        cur.seciton == cur.number ? cur.isSection = true : cur.isSection = false;
        glob == 1 ? cur.isGlobal = true : cur.isGlobal = false;
        this->red_files.back().symTable.push_back(cur);
        // obrada slucaja kada je simbol globalan ( ide u globalnu tabelu simbola linkera )
        if (cur.isGlobal && !cur.isExtern)
        {
            for (auto i : this->symbolTable)
                if (i.name == cur.name)
                {
                    printError(ALREDY_DEFINED, 0);
                    this->stopProcess = true;
                    return;
                }
            this->symbolTable.push_back(cur);
        }
        // pamcenje broja sekcije koja se nastavlja na vec postojecu ako se poklopi sa trenutnom obradom u tabeli
        if (cur.name == ime_dodatog)
            br_dodatog = cur.number;
    }

    // obrada relokacionih simbola trenutnog fajla
    while (getline(curr, red))
    {
        char nam[20];
        int locC, locM, ty, num;
        sscanf(red.c_str(), "%s\t%d\t%d\t%d\t%d", nam, &locC, &locM, &ty, &num);
        Info cur(nam, locC, locM, ty, num);
        // obrada slucaja ako se radi o rel zapisu nastavljene sekcije na neku drugu
        int br = -1;
        if (pomeraj != -1)
            for (auto a : this->red_files.back().symTable) // Potraga broja sekcije promenljive ciji rel zapis obradjujemo
                if (a.name == cur.name)
                {
                    br = a.seciton;
                    break;
                }
        if (pomeraj != -1 && br_dodatog != -1 && (br == br_dodatog || br == 0)) // premalo puta se ovde ulazi
            cur.locationInCode += pomeraj;

        this->red_files.back().relTable.push_back(cur);
    }
}

void linker::obrada()
{
    // obrada na kraju
    // prepravljanje prema relokacionim zapisima
    // prepravljanje globalnih vrednosti

    int trenutni_pomeraj = 0;
    for (auto i = this->red_files.begin(); i != this->red_files.end(); i++)
    {
        for (auto j = i->sections.begin(); j != i->sections.end(); j++)
        {
            if (j->code.empty())
            {
                for (auto m = this->red_files.begin(); m != this->red_files.end(); m++)
                    for (auto n = m->sections.begin(); n != m->sections.end(); n++)
                        if (n->ime == j->ime && !n->code.empty())
                        {
                            int br = -1;
                            for (auto a : i->symTable)
                                if (a.name == j->ime)
                                {
                                    br = a.number;
                                    break;
                                }

                            for (auto b = i->symTable.begin(); b != i->symTable.end(); b++)
                            {
                                int val = -1, broj;
                                for (auto c : this->symbolTable)
                                    if (b->name == c.name)
                                        val = c.value;
                                if (!b->isExtern)
                                    broj = b->value / 2;
                                else
                                    broj = val;
                                if (!b->isExtern)
                                {
                                    broj += n->pomeraj / 2;
                                    broj += j->pomeraj / 2;
                                }
                                b->value = broj;
                                for (auto c = this->symbolTable.begin(); c != this->symbolTable.end(); c++)
                                    if (c->name == b->name)
                                    {
                                        c->value = broj;
                                        break;
                                    }
                            }
                            break;
                        }
            }
            else
            {
                int br = -1;
                for (auto a : i->symTable)
                    if (a.name == j->ime)
                    {
                        br = a.number;
                        break;
                    }

                for (auto b = i->symTable.begin(); b != i->symTable.end(); b++)
                {
                    if (b->seciton == br)
                    {
                        string ime = b->name;
                        int val = -1, broj;
                        for (auto c : this->symbolTable)
                            if (b->name == c.name)
                                val = c.value;
                        if (!b->isExtern)
                            broj = b->value / 2;
                        else
                            broj = val;
                        if (!b->isExtern)
                        {
                            // if (ime == "destinations" || ime == "value6")
                            //     cout << "menjamo" << endl;
                            broj += trenutni_pomeraj / 2;
                        }
                        b->value = broj;
                        for (auto c = this->symbolTable.begin(); c != this->symbolTable.end(); c++)
                            if (c->name == b->name)
                            {
                                c->value = broj;
                                break;
                            }
                    }
                }
                if (j->pomeraj == -1)
                    j->pomeraj = trenutni_pomeraj;
                trenutni_pomeraj += j->code.size();
            }
        }
    }

    for (auto i = this->red_files.begin(); i != this->red_files.end(); i++)
    {
        for (auto j = i->sections.begin(); j != i->sections.end(); j++)
        {
            if (j->code.empty())
            {
                for (auto m = this->red_files.begin(); m != this->red_files.end(); m++)
                    for (auto n = m->sections.begin(); n != m->sections.end(); n++)
                        if (n->ime == j->ime && !n->code.empty())
                        {
                            int br = -1;
                            for (auto a : i->symTable)
                                if (a.name == j->ime)
                                {
                                    br = a.number;
                                    break;
                                }

                            for (auto a : i->relTable)
                                for (auto b = i->symTable.begin(); b != i->symTable.end(); b++)
                                {
                                    if (a.name == b->name && a.numberInSybolTable == br && a.typeOfDefinition == 1)
                                    {
                                        int val = -1, broj;
                                        for (auto c : this->symbolTable)
                                            if (b->name == c.name)
                                                val = c.value;
                                        if (!b->isExtern)
                                            broj = b->value;
                                        else
                                            broj = val;
                                        b->value = broj;
                                        // Ako nije def a extern je greska!!!
                                        std::ostringstream ss;
                                        ss << std::hex << broj;
                                        string result = ss.str();
                                        if (result.size() == 1)
                                        {
                                            n->code[a.locationInCode + 2] = '0';
                                            n->code[a.locationInCode + 3] = '0';
                                            n->code[a.locationInCode] = '0';
                                            n->code[a.locationInCode + 1] = result[0];
                                        }
                                        else if (result.size() == 2)
                                        {
                                            n->code[a.locationInCode + 2] = '0';
                                            n->code[a.locationInCode + 3] = '0';
                                            n->code[a.locationInCode] = result[0];
                                            n->code[a.locationInCode + 1] = result[1];
                                        }
                                        else if (result.size() == 3)
                                        {
                                            n->code[a.locationInCode + 2] = '0';
                                            n->code[a.locationInCode + 3] = result[0];
                                            n->code[a.locationInCode] = result[1];
                                            n->code[a.locationInCode + 1] = result[2];
                                        }
                                        else if (result.size() == 4)
                                        {
                                            n->code[a.locationInCode + 2] = result[0];
                                            n->code[a.locationInCode + 3] = result[1];
                                            n->code[a.locationInCode] = result[2];
                                            n->code[a.locationInCode + 1] = result[3];
                                        }
                                    }
                                }
                            break;
                        }
            }
            else
            {
                int br = -1;
                for (auto a : i->symTable)
                    if (a.name == j->ime)
                    {
                        br = a.number;
                        break;
                    }

                for (auto a : i->relTable)
                    for (auto b = i->symTable.begin(); b != i->symTable.end(); b++)
                    {
                        if (a.name == b->name && a.numberInSybolTable == br && a.typeOfDefinition == 1)
                        {
                            int val = -1, broj;
                            for (auto c : this->symbolTable)
                                if (b->name == c.name)
                                    val = c.value;
                            if (!b->isExtern)
                                broj = b->value;
                            else
                                broj = val;
                            b->value = broj;
                            // Ako nije def a extern je greska!!!
                            std::ostringstream ss;
                            ss << std::hex << broj;
                            string result = ss.str();
                            if (result.size() == 1)
                            {
                                j->code[a.locationInCode + 2] = '0';
                                j->code[a.locationInCode + 3] = '0';
                                j->code[a.locationInCode] = '0';
                                j->code[a.locationInCode + 1] = result[0];
                            }
                            else if (result.size() == 2)
                            {
                                j->code[a.locationInCode + 2] = '0';
                                j->code[a.locationInCode + 3] = '0';
                                j->code[a.locationInCode] = result[0];
                                j->code[a.locationInCode + 1] = result[1];
                            }
                            else if (result.size() == 3)
                            {
                                j->code[a.locationInCode + 2] = '0';
                                j->code[a.locationInCode + 3] = result[0];
                                j->code[a.locationInCode] = result[1];
                                j->code[a.locationInCode + 1] = result[2];
                            }
                            else if (result.size() == 4)
                            {
                                j->code[a.locationInCode + 2] = result[0];
                                j->code[a.locationInCode + 3] = result[1];
                                j->code[a.locationInCode] = result[2];
                                j->code[a.locationInCode + 1] = result[3];
                            }
                        }
                    }
            }
        }
    }

    int i = 0;
    int cur_adr = 0;
    this->for_write.push_back('0');
    this->for_write.push_back('0');
    this->for_write.push_back('0');
    this->for_write.push_back('0');
    this->for_write.push_back(':');
    this->for_write.push_back(' ');
    for (auto a : this->red_files)
        for (auto b : a.sections)
            for (auto c : b.code)
            {
                if (i % 16 == 0 && i != 0)
                {
                    this->for_write.push_back('\n');
                    cur_adr += 8;
                    std::ostringstream ss;
                    ss << std::hex << cur_adr;
                    string result = ss.str();
                    if (result.size() == 1)
                    {
                        this->for_write.push_back('0');
                        this->for_write.push_back('0');
                        this->for_write.push_back('0');
                        this->for_write.push_back(result[0]);
                    }
                    else if (result.size() == 2)
                    {
                        this->for_write.push_back('0');
                        this->for_write.push_back('0');
                        this->for_write.push_back(result[0]);
                        this->for_write.push_back(result[1]);
                    }
                    else if (result.size() == 3)
                    {
                        this->for_write.push_back('0');
                        this->for_write.push_back(result[0]);
                        this->for_write.push_back(result[1]);
                        this->for_write.push_back(result[2]);
                    }
                    else if (result.size() == 4)
                    {
                        this->for_write.push_back(result[0]);
                        this->for_write.push_back(result[1]);
                        this->for_write.push_back(result[2]);
                        this->for_write.push_back(result[3]);
                    }
                    this->for_write.push_back(':');
                    this->for_write.push_back(' ');
                }
                else if (i % 2 == 0 && i != 0)
                    this->for_write.push_back(' ');
                this->for_write.push_back(c);
                i++;
            }
}