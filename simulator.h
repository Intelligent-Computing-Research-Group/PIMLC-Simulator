/**  
 * @file    simulator.h
 * @brief   Simulator
 * @author  Chenu Tang
 * @version 0.1
 * @date    2022-11-28
 * @note    
 */
#ifndef _SIMULATOR_
#define _SIMULATOR_
#include <fstream>
#include "common.h"
#include "subarray.h"
#include "mainmem.h"
#include "instruction.h"

#define BUFSIZE 256
#define SUBARRROWS 256
#define SUBARRCOLS 256
#define SUBARRNUM  64

namespace SimulatorCommand {
enum Command {
    NOP, NEXT, STEP, RUN, PRINT, ARRAY, MEMORY, EXEC,
};
}
class Simulator {
private:
    int curline;
    char cmdbuf[BUFSIZE];
    char codeline[BUFSIZE];
    SimulatorCommand::Command cmd;
    int cmdarg;
    Subarray<SUBARRROWS, SUBARRCOLS> arrs[SUBARRNUM];
    std::ifstream codefile;
    Instruction inst;
    Memory mem;
    uint meshrows;
    uint meshcols;
    uint memoffset;
    uint data;
    uint datasize;
    uint input;
    uint output;
    int mode;
public:
    Simulator();
    ~Simulator();
    int Init();
    int clear();

    int importInput(std::string *s, int len);
    int printOutput(int len);

    // Debug
    int waitCommand();
    int execCommand();
    int next();
    int step(int n = 0);
    int run(int n = 0);
    int exec();

    /// @brief print memory bits
    /// @param indexes indexes of bits need to be printed
    /// @param len the length of indexes
    /// @param offset parallelism offset 
    /// @return 
    int printMem(int *indexes, int len, int offset);
    
    int traverseMem();


    // 
    int openCode(const char *filename);
    int closeCode();
    int setMem(int addr, std::bitset<MEMLEN> row);
    int setMem(int *indexes, int len, int offset, bool *value);
    int getInst(const char *str);
    int getInfo(const char *str);
};


#endif