/**  
 * @file    subarray.h
 * @brief   Simulate the basic sub-array
 * @author  Chenyu Tang
 * @version 0.1
 * @date    2022-11-28
 * @note    
 */
#ifndef _SUBARRAY_
#define _SUBARRAY_
#include <bitset>
#include "common.h"

template<size_t r, size_t c>
struct Subarray {
    std::bitset<c> rows[r];
};

#endif