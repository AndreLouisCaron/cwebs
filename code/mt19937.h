#ifndef _mt19937_h__
#define _mt19937_h__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file code/mt19937.h
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#ifdef _WIN32
    typedef unsigned char uint8_t;
    typedef unsigned __int32 uint32_t;
#else
#   include <stdint.h>
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Mersene twister pseudo-random number generator.
 */
typedef struct mt19937_prng_t
{
    uint32_t data[624];
    uint32_t base;

} mt19937_prng;

/*!
 * @brief Initialize a Mersene twister pseudo-random number generator.
 */
void mt19937_prng_init ( mt19937_prng * generator, uint32_t seed );

/*!
 * @brief Generate a random 32-bit unsigned integer.
 */
uint32_t mt19937_prng_next ( mt19937_prng * generator );

/*!
 * @brief Generate a sequence of random bytes.
 */
void mt19937_prng_grab ( mt19937_prng * generator, void * data, size_t size );

#ifdef __cplusplus
}
#endif

#endif /* _mt19937_h__ */
