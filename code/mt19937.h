#ifndef _mt19937_h__
#define _mt19937_h__

// Copyright (c) 2011-2012, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*!
 * @file code/mt19937.h
 * @brief Mersenne twister pseudo-random number generator.
 *
 * @see http://en.wikipedia.org/wiki/Mersenne_twister
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
