/**  
 * @file    pimlcsim.cpp
 * @brief   simulation main
 * @author  Chenyu Tang
 * @version 1.0
 * @date    2023-02-09
 * @note    
 */#include <iostream>
#include <bitset>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include "simulator.h"
using namespace std;

#define INPUTNUM 256
#define OUTPUTNUM 128

int main(int argc, char *argv[])
{
    ifstream infile;
    char inputline[1023]="";
    infile.open(argv[2]);
    infile.getline(inputline, 1023);
    int linecnt=atoi(inputline);
    string *s = new string[linecnt];
    for (int i = 0; i < linecnt; ++i) {
        infile.getline(inputline, 1023);
        s[i] = inputline;
    }
    Simulator sim(NORMALMODE);
    sim.openCode(argv[1]);
    sim.importInput(s, linecnt);
    sim.execCommand();
    sim.printOutput(linecnt);
    sim.closeCode();
    delete[] s;
    return 0;
}
