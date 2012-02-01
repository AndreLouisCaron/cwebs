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
 * @file code/mt19937.c
 * @brief Mersenne twister pseudo-random number generator.
 *
 * @see http://en.wikipedia.org/wiki/Mersenne_twister
 */

#include "mt19937.h"

static void _init ( uint32_t MT[] )
{
    size_t i;
    for ( i = 1; (i < 624); ++i ) {
        MT[i] = (uint32_t)(0x6c078965 * (MT[i-1]^(MT[i-1]>>30)) + i);
    }
}

static uint32_t _next ( const uint32_t MT[], size_t i )
{
    uint32_t y = MT[i];
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= (y >> 18);
    return (y);
}

static void _mash ( uint32_t MT[] )
{
    size_t i;
    size_t y;
    for ( i = 0; (i < 624); ++i )
    {
        y = (MT[i] >> 31) |
            (MT[(i+1)%624]&(1<<31));
        MT[i] = MT[(i+397)%624] ^ (y >> 1);
        if ((y % 2) != 0) {
            MT[i] = MT[i] ^ 0x9908b0df;
        }
    }
}

static void _grab ( mt19937_prng * generator, uint8_t * data, size_t size )
{
    uint32_t next;
    while ( size > 0 )
    {
        next = mt19937_prng_next(generator);
        if ( size > 0 ) {
            --size, *data++ = next&0xff, next >>= 8;
        }
        if ( size > 0 ) {
            --size, *data++ = next&0xff, next >>= 8;
        }
        if ( size > 0 ) {
            --size, *data++ = next&0xff, next >>= 8;
        }
        if ( size > 0 ) {
            --size, *data++ = next&0xff, next >>= 8;
        }
    }
}

void mt19937_prng_init ( mt19937_prng * generator, uint32_t seed )
{
    generator->base = 0;
    generator->data[0] = seed;
    _init(generator->data);
}

uint32_t mt19937_prng_next ( mt19937_prng * generator )
{
    uint32_t next;
    if (generator->base == 0) {
        _mash(generator->data);
    }
    next = _next(generator->data, generator->base);
    ++generator->base, generator->base %= 624;
    return (next);
}

void mt19937_prng_grab ( mt19937_prng * generator, void * data, size_t size )
{
    _grab(generator, (uint8_t*)data, size);
}
