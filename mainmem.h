/**  
 * @file    mainmem.h
 * @brief   Simulate the basic main memory
 * @author  Chenyu Tang
 * @version 0.1
 * @date    2022-11-28
 * @note    
 */
#ifndef _MAINMEM_
#define _MAINMEM_

#include <map>
#include <bitset>


#define MEMLEN 256

typedef std::map<int, std::bitset<MEMLEN>> Memory;
typedef std::map<int, std::bitset<MEMLEN>>::iterator MemoryIter;

// class Memory {

// };

#endif