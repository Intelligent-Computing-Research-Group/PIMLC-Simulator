/**  
 * @file    simulator.cpp
 * @brief   PIMLC Simulator
 * @author  Chenyu Tang
 * @version 1.0
 * @date    2023-02-09
 * @note    
 */
#include "simulator.h"
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <climits>

#define SKIPSPACES(p)  while (*(p)==' ' || *(p)=='\t' || *(p) == '\n' || *(p) == '\r') ++(p)

int strheadstr(const char *s, const char *sub)
{
    int len = 0;
    if (!s || !sub) {
        return -1;
    }
    while (*s && *sub && (*s==*sub)) {
        ++s;
        ++sub;
        ++len;
    }
    if (*sub) {
        return -1;
    }
    return len;
}


void getaddr(const char *&p, int &addr, bool &flag)
{
    SKIPSPACES(p);
    if (*p == '~') {
        ++p;
        flag = true;
    }
    else {
        flag = false;
    }
    addr = 0;
    while (*p >= '0' && *p <= '9') {
        addr = addr * 10 + *p - '0';
        ++p;
    }
}

Simulator::Simulator(int m)
{
    curline = 0;
    mode = m;
    *codeline = '\0';
    *cmdbuf = '\0';
    if (mode==NORMALMODE) {
        cmd = SimulatorCommand::RUN;
    }
    else {
        cmd = SimulatorCommand::NOP;
    }
    cmdarg = -1;
    meshrows = 0u;
    meshcols = 0u;
    memoffset = 0u;
    data = 0u;
    datasize = 0u;
    input = 0u;
    output = 0u;
    curtask=0u;
}

Simulator::~Simulator()
{
    if (codefile.is_open()) {
        codefile.close();
    }
}

/**
 * @brief import input data
 * 
 * @param s   inputs - binary format, strlen=input
 * @param len input data size
 * @return int 
 * @details input indexes - 0 ~ input-1
 *          output indexes - input - input+output-1
 */
int Simulator::importInput(std::string *s, int len)
{
    if (len > data) {
        return -1;
    }
    int group = (len + MEMLEN - 1) / MEMLEN;
    int groupsize;
    std::bitset<MEMLEN> bs;
    for (int x = 0; x < input; ++x) {
        for (int g = 0; g < group; ++g) {
            groupsize = (g==(group-1) ? len-g*MEMLEN : MEMLEN);
            bs = 0;
            for (int i = 0; i < groupsize; ++i) {
                mem[x*datasize+g][i] = (bool)(s[g*MEMLEN+i][x]-'0');
            }
        }
    }
    return 1;
}

int Simulator::printOutput(int len)
{
    if (len > data) {
        return -1;
    }
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < output; ++j) {
            // printf("mem[%d][%d]: %d\n",(input+j)*datasize+i/MEMLEN,i%MEMLEN,(int)(mem[(input+j)*datasize+i/MEMLEN][i%MEMLEN]));
            printf("%d",(int)(mem[(input+j)*datasize+i/MEMLEN][i%MEMLEN]));
        }
        printf("\n");
    }
    return 1;
}

/**
 * @brief wait for command in debug mode
 * @return int 
 * @details command explaination
 *  next(n)     next stage
 *  step(s)     step to next instruction
 *  run(r)      run
 *  print(p)    print instruction information
 *  array(a)    print sub-array information
 *  memory(m)   print main memory information 
 *  exec(e) xx  execute the following instrution(xx)
 */
int Simulator::waitCommand()
{
    do printf("> "); while (fgets(cmdbuf, BUFSIZE, stdin) == NULL || *cmdbuf == '\n' || *cmdbuf == '\r');

    if (strlen(cmdbuf) == BUFSIZE-1 && cmdbuf[BUFSIZE-2] != '\n') {
        return -1;  // input too long
    }

    char *p = cmdbuf;
    const char *cmdname;
    SKIPSPACES(p);
    switch (*p) {
    case 'n':
        cmd = SimulatorCommand::NEXT;
        break;
    case 's':
        cmd = SimulatorCommand::STEP;
        break;
    case 'r':
        cmd = SimulatorCommand::RUN;
        break;
    case 'p':
        cmd = SimulatorCommand::PRINT;
        break;
    case 'a':
        cmd = SimulatorCommand::ARRAY;
        break;
    case 'm':
        cmd = SimulatorCommand::MEMORY;
        break;
    case 'e':
        cmd = SimulatorCommand::EXEC;
        break;
    case 'q':
        return 0;
        break;
    default:
        cmd = SimulatorCommand::NOP;
        break;
    }
    ++p;
    if (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '\0') {
        return -1;
    }
    if (cmd == SimulatorCommand::EXEC) {
        return getInst(p);
    }
    else {
        SKIPSPACES(p);
        if (*p > '9' || *p < '0') {
            cmdarg = -1;    // default value
        }
        else {
            cmdarg = atoi(p);
        }
    }
    return 1;
}

/**
 * @brief execute command in debug mode
 * @return int 
 * @details \\see Simulator::waitCommand()
 */
int Simulator::execCommand()
{
    switch (cmd)
    {
    case SimulatorCommand::NEXT: {

        break;
    }
    case SimulatorCommand::STEP: {
        step();
        break;
    }
    case SimulatorCommand::RUN: {
        run();
        break;
    }
    case SimulatorCommand::PRINT: {
        
        break;
    }
    case SimulatorCommand::ARRAY: {
        
        break;
    }
    case SimulatorCommand::MEMORY: {
        
        break;
    }
    case SimulatorCommand::EXEC: {
        
        break;
    }
    default:
        break;
    }
    return 1;
}


int Simulator::step(int n)
{
    int t = cmdarg;
    if (n != 0) {
        t = n;
    }
    if (t == -1) {
        t = 1;
    }
    
    while (t--) {
        
        if (codefile.eof()) {
            return 0;
        }
        *codeline = '\0';
        codefile.getline(codeline, BUFSIZE);
        ++curline;

        int ret = getInst(codeline);
        if (ret > 0) {
            if (mode == 0) {
                printf("line %d: %s\n", curline-1, codeline);
            }
            if (ret == 1) {
                exec();
            }
        }
    }
    return 1;
}


int Simulator::run(int n)
{
    if (n > 0) {
        cmdarg = n;
    }

    if (cmdarg == -1) {
        cmdarg = INT_MAX;
    }
    *codeline = '\0';
    while (curline < cmdarg) {
        if (codefile.eof()) {
            if (mode == DEBUGMODE) {
                printf("Last line %d: %s\n", curline-1, codeline);
            }
            return 0;
        }
        *codeline = '\0';
        codefile.getline(codeline, BUFSIZE);
        ++curline;

        if (getInst(codeline) == 1) {
            exec();
        }
    }
    if (mode == DEBUGMODE) {
        printf("Last line %d: %s\n", curline-1, codeline);
    }
    return 1;
}



int Simulator::exec()
{
    int src0arr, src1arr, src2arr, destarr;
    int src0row, src1row, src2row, destrow;
    src0arr = inst.src[0] / SUBARRROWS;
    src1arr = inst.src[1] / SUBARRROWS;
    src2arr = inst.src[2] / SUBARRROWS;
    destarr = inst.dest / SUBARRROWS;
    src0row = inst.src[0] % SUBARRROWS;
    src1row = inst.src[1] % SUBARRROWS;
    src2row = inst.src[2] % SUBARRROWS;
    destrow = inst.dest % SUBARRROWS;
    switch (inst.op) {
    case COPY: {
        for (int offset = 0; offset < meshcols; ++offset) {
            arrs[destarr+offset*meshrows].rows[destrow] = arrs[src0arr+offset*meshrows].rows[src0row];
        }
        break;
    }
    case STORE: {
        for (int offset = 0; offset < meshcols; ++offset) {
            // std::cout << "Store " << mem[(inst.dest-meshrows*SUBARRCOLS-1)*datasize+offset+memoffset] << " to " << (inst.dest-meshrows*SUBARRCOLS-1) << "[" << offset+memoffset << "]" << std::endl;
            mem[(inst.dest-meshrows*SUBARRCOLS-1)*datasize+offset+memoffset] = arrs[src0arr+offset*meshrows].rows[src0row];
        }
        break;
    }
    case LOAD: {
        for (int offset = 0; offset < meshcols; ++offset) {
            Memory::iterator it = mem.find((inst.src[0]-meshrows*SUBARRCOLS-1)*datasize+offset+memoffset);
            if (it != mem.end()) {
                arrs[destarr+offset*meshrows].rows[destrow] = it->second;
            }
            else {
                arrs[destarr+offset*meshrows].rows[destrow] = 0;
            }
        }
        break;
    }
    case SYNC: {

        break;
    }
    case INV: {
        for (int offset = 0; offset < meshcols; ++offset) {
            arrs[destarr+offset*meshrows].rows[destrow] = ~arrs[src0arr+offset*meshrows].rows[src0row];
        }
        break;
    }
    case AND: {
        for (int offset = 0; offset < meshcols; ++offset) {
            arrs[destarr+offset*meshrows].rows[destrow] = arrs[src0arr+offset*meshrows].rows[src0row] & arrs[src1arr+offset*meshrows].rows[src1row];
        }
        break;
    }
    case OR: {
        for (int offset = 0; offset < meshcols; ++offset) {
            arrs[destarr+offset*meshrows].rows[destrow] = arrs[src0arr+offset*meshrows].rows[src0row] | arrs[src1arr+offset*meshrows].rows[src1row];
        }
        break;
    }
    case XOR: {
        for (int offset = 0; offset < meshcols; ++offset) {
            arrs[destarr+offset*meshrows].rows[destrow] = arrs[src0arr+offset*meshrows].rows[src0row] ^ arrs[src1arr+offset*meshrows].rows[src1row];
        }
        break;
    }
    case XOR3: {
        for (int offset = 0; offset < meshcols; ++offset) {
            arrs[destarr+offset*meshrows].rows[destrow] = arrs[src0arr+offset*meshrows].rows[src0row] ^ arrs[src1arr+offset*meshrows].rows[src1row] ^ arrs[src2arr+offset*meshrows].rows[src2row];
        }
        break;
    }
    case MAJ: {
        for (int offset = 0; offset < meshcols; ++offset) {
            arrs[destarr+offset*meshrows].rows[destrow] = 
                (arrs[src0arr+offset*meshrows].rows[src0row] & arrs[src1arr+offset*meshrows].rows[src1row]) | 
                (arrs[src0arr+offset*meshrows].rows[src0row] & arrs[src2arr+offset*meshrows].rows[src2row]) | 
                (arrs[src1arr+offset*meshrows].rows[src1row] & arrs[src2arr+offset*meshrows].rows[src2row]);
        }
        break;
    }
    default:
        break;
    }
    if (curtask && !memoffset) {
        // printf("n%d,%d\n", curtask, (int)(arrs[destarr].rows[destrow][0]));
        curtask = 0u;
    }
    return 1;
}


int Simulator::printMem(int *indexes, int len, int offset)
{
    for (int i = 0; i < len; ++i) {
        printf("%d",int(mem[indexes[i]*datasize+offset/SUBARRCOLS][offset%SUBARRCOLS]));
    }
    printf("\n");
    return 0;
}



int Simulator::traverseMem()
{
    Memory::iterator it;
    for (it = mem.begin(); it != mem.end(); ++it) {
        std::cout << it->first << " : " << it->second << std::endl;
    }
    return 1;
}



int Simulator::openCode(const char *filename)
{
    if (codefile.is_open()) {
        codefile.close();
    }
    codefile.open(filename);

    if (codefile.is_open()) {
        curline = 1;
        while (codefile.peek() == '#') {
            step(1);
        }
        return 1;
    }
    return 0;
}

int Simulator::closeCode()
{
    if (codefile.is_open()) {
        codefile.close();
    }
    if (codefile.is_open()) {
        return 0;
    }
    curline = 0;
    return 1;
}

int Simulator::setMode(int m)
{
    mode = m;
    return m;
}


int Simulator::setMem(int addr, std::bitset<MEMLEN> row)
{
    mem[addr] = row;
    return 1;
}

int Simulator::setMem(int *indexes, int len, int offset, bool *value)
{
    Memory::iterator it;
    for (int i = 0; i < len; ++i) {
        it = mem.find(indexes[i]*datasize+offset/SUBARRCOLS);
        if (it == mem.end()) {
            mem[indexes[i]*datasize+offset/SUBARRCOLS] = 0;
        }
        mem[indexes[i]*datasize+offset/SUBARRCOLS][offset%SUBARRCOLS] = value[i];
    }
    return 1;
}


int Simulator::getInst(const char *str)
{
    const char *p = str;
    int len;
    SKIPSPACES(p);

    // non-inst line
    if (*p == '\0') {
        return 0;
    }
    else if (*p == '#' || *p == '%') {
        getInfo(++p);
        return 2;
    }

    int i;
    for (i = 0; i < sizeof(instname)/sizeof(const char* const); ++i) {
        len = strheadstr(p, instname[i]);
        if (len > 0 && (*(p+len)==' ' || *(p+len)=='\t')) {
            inst.op = (InstructionType)(i);
            break;
        }
    }
    if (i >= sizeof(instname)/sizeof(const char* const)) {
        return -1;
    }
    p += len;
    getaddr(p, inst.src[0], inst.invflag[0]);
    getaddr(p, inst.src[1], inst.invflag[1]);
    getaddr(p, inst.src[2], inst.invflag[2]);
    bool tmp;
    getaddr(p, inst.dest, tmp);
    const char *task = strstr(p, "Task.");
    curtask = 0u;
    if (task) {
        task += 5;
        while (*task<='9'&&*task>='0') {
            curtask = curtask * 10 + *task - '0';
            ++task;
        }
    }
    return 1;
}


int Simulator::getInfo(const char *str)
{
    const char *p = str;
    int len;
    SKIPSPACES(p);

    if ((len = strheadstr(p, "meshrows")) > 0 && (*(p + len) == ' ')) {
        p += len;
        SKIPSPACES(p);
        meshrows = 0;
        while (*p >= '0' && *p <= '9') {
            meshrows = meshrows * 10 + *p - '0';
            ++p;
        }
    }
    else if ((len = strheadstr(p, "meshcols")) > 0 && (*(p + len) == ' ')) {
        p += len;
        SKIPSPACES(p);
        meshcols = 0;
        while (*p >= '0' && *p <= '9') {
            meshcols = meshcols * 10 + *p - '0';
            ++p;
        }
    }
    else if ((len = strheadstr(p, "memoffset")) > 0 && (*(p + len) == ' ')) {
        p += len;
        SKIPSPACES(p);
        memoffset = 0;
        while (*p >= '0' && *p <= '9') {
            memoffset = memoffset * 10 + *p - '0';
            ++p;
        }
    }
    else if ((len = strheadstr(p, "data")) > 0 && (*(p + len) == ' ')) {
        p += len;
        SKIPSPACES(p);
        data = 0;
        while (*p >= '0' && *p <= '9') {
            data = data * 10 + *p - '0';
            ++p;
        }
        datasize = (data+SUBARRCOLS-1)/SUBARRCOLS;
    }
    else if ((len = strheadstr(p, "input")) > 0 && (*(p + len) == ' ')) {
        p += len;
        SKIPSPACES(p);
        input = 0;
        while (*p >= '0' && *p <= '9') {
            input = input * 10 + *p - '0';
            ++p;
        }
    }
    else if ((len = strheadstr(p, "output")) > 0 && (*(p + len) == ' ')) {
        p += len;
        SKIPSPACES(p);
        output = 0;
        while (*p >= '0' && *p <= '9') {
            output = output * 10 + *p - '0';
            ++p;
        }
    }
    return 1;
}