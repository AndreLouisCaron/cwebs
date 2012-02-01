#ifndef _iwire_h__
#define _iwire_h__

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
 * @file iwire.h
 * @brief Web Socket wire protocol (in-bound) for C.
 *
 * @see http://tools.ietf.org/html/rfc6455
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ws_iwire;

typedef uint64(*ws_iwire_state)
    (struct ws_iwire*,const uint8*,uint64);

  /*!
   * @brief Streaming parser for Web Socket wire protocol (in-bound).
   */
struct ws_iwire
{
    // event callbacks.
    void(*new_message)(struct ws_iwire*);
    void(*end_message)(struct ws_iwire*);
    void(*new_fragment)(struct ws_iwire*,uint64);
    void(*end_fragment)(struct ws_iwire*);
    void(*accept_content)(struct ws_iwire*,const void*,uint64);

    // for use by callbacks.
    void * baton;

    // read-only fields.
    int code;
    int eval;
    int last;
    int usem;
    int good;

    // internal state.
    ws_iwire_state state;
    uint8 data[8];
    uint64 size;
    uint8 mask[4];
    uint64 used;
    uint64 pass;
};

void ws_iwire_init ( struct ws_iwire * stream );

uint64 ws_iwire_feed
    ( struct ws_iwire * stream, const void * data, uint64 size );

int ws_iwire_ping ( const struct ws_iwire * stream );
int ws_iwire_pong ( const struct ws_iwire * stream );

int ws_iwire_text ( const struct ws_iwire * stream );
int ws_iwire_data ( const struct ws_iwire * stream );

int ws_iwire_dead ( const struct ws_iwire * stream );

#ifdef __cplusplus
}
#endif

#endif /* _iwire_h__ */

