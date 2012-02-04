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
 * @file owire.c
 * @brief Web Socket wire protocol (out-bound) for C.
 *
 * @see http://tools.ietf.org/html/rfc6455
 */

#include "owire.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

/*!
 * @internal
 * @brief Simple (and bad) random number generator.
 * @param stream Current writer state.
 * @param mask 4-byte array to fill in with random values.
 *
 * This generator is used for creating masks when no better random number
 * generator is provided.
 */
static void _ws_unsafe_random_mask ( struct ws_owire * stream, uint8 mask[4] )
{
    mask[0] = rand() & 0xff;
    mask[1] = rand() & 0xff;
    mask[2] = rand() & 0xff;
    mask[3] = rand() & 0xff;
}

/*!
 * @internal
 * @brief Generated frame mask using custom generator, if required.
 * @param stream Current writer state.
 */
static void _ws_mask ( struct ws_owire * stream )
{
    if ( (stream->data[1]&0x80) != 0 ) {
        stream->rand(stream, stream->data+10);
    }
}

/*!
 * @internal
 * @brief Pack frame header and forward it to the application for transfer.
 * @param stream Current writer state.
 */
static void _ws_head ( struct ws_owire * stream )
{
    const uint8 size = (stream->data[1] & 0x7f);
    const uint8 usem = (stream->data[1] & 0x80);
    // compute prefix size.
    uint8 used = 0;
    if ( size < 126 ) {
        used = 2;
    }
    if ( size == 126 ) {
        used = 4;
    }
    if ( size == 127 ) {
        used = 10;
    }
    // pack header by moving mask.
    if ( usem && (size < 127) ) {
        memcpy(stream->data+used, stream->data+10, 4), used += 4;
    }
    // send packed header.
    stream->accept_content(stream, stream->data, used);
}

/*!
 * @internal
 * @brief Default writer state.  Emits a writer error if called.
 * @param stream Current writer state.
 * @param data Ignored, used to comply with writer handler signature.
 * @param size Ignored, used to comply with writer handler signature.
 *
 * The writer uses this to protected against ill-formed applications that start
 * trying to send data before the frame header is prepared.  As soon as the
 * frame header is sent, this handler is changed for a real writer handler.
 *
 * @see _ws_body_1
 * @see _ws_body_2
 */
static uint64 _ws_fail
    ( struct ws_owire * stream, const uint8 * data, uint64 size )
{
    stream->error = ws_owire_enotready;
    return (0);
}

/*!
 * @internal
 * @brief Forward unmasked frame payload.
 * @param stream Current writer state.
 * @param data Data to be written.
 * @param size Number of bytes to write.
 *
 * @see _ws_fail
 * @see _ws_body_2
 */
static uint64 _ws_body_1
    ( struct ws_owire * stream, const uint8 * data, uint64 size )
{
    // don't smear across frames.
    size = MIN(size, stream->pass);
    // pass all possible data.
    stream->accept_content(stream, data, size);
    // update cursors.
    stream->used += size;
    stream->pass -= size;
    // prepare for next frame.
    if ( stream->pass == 0 )
    {
        stream->used = 0;
    }
    return (size);
}

/*!
 * @internal
 * @brief Forward masked frame payload.
 * @param stream Current writer state.
 * @param data Data to be written.
 * @param size Number of bytes to write.
 *
 * @see _ws_fail
 * @see _ws_body_1
 */
static uint64 _ws_body_2
    ( struct ws_owire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    uint8 bufdata[256];
    size_t bufsize = 0;
    // don't smear across frame boundaries.
    size = MIN(stream->pass, size);
    // start parsing data.
    while ( used < size )
    {
        // copy butes to buffer and un-mask.
        const uint8 *const mask = stream->data+10;
        for ( bufsize = 0; ((used < size) && (bufsize < 256)); ++bufsize ) {
            bufdata[bufsize] = data[used++] ^ mask[stream->used++%4];
        }
        // pass data to stream owner.
        stream->accept_content(stream, bufdata, bufsize);
    }
    // adjust cursors.
    stream->pass -= used;
    // prepare for next frame.
    if ( stream->pass == 0 )
    {
        stream->used = 0;
    }
    return (used);
}

/*!
 * @internal
 * @brief Process and forward frame payload, masking if necessary.
 * @param stream Current writer state.
 * @param data Data to be written.
 * @param size Number of bytes to write.
 *
 * @see _ws_body_1
 * @see _ws_body_2
 */
static uint64 _ws_body
    ( struct ws_owire * stream, const uint8 * data, uint64 size )
{
    return (((stream->data[1] & 0x80)==0)?
        _ws_body_1(stream, data, size) :
        _ws_body_2(stream, data, size));
}

/*!
 * @internal
 * @brief Emit a complete message, fragmenting it if necessary.
 * @param stream Current writer state.
 * @param data Data to be written.
 * @param size Number of bytes to write.
 * @param code The message type (text, data, ping, etc.).
 *
 * @bug The message type is not considered when auto-fragmenting.  However,
 *  control frames (ping, pong and close) must not be fragmented (see RFC6455,
 *  section 5.4).
 */
static void ws_owire_put_full
    ( struct ws_owire * stream, const uint8 * data, uint64 size, uint8 code )
{
    uint64 used = 0;
    uint64 part = 0;
    ws_owire_new_message(stream);
    if ((stream->auto_fragment == 0) || (size == 0))
    {
        ws_owire_new_frame(stream, size);
          ws_owire_last(stream);
          ws_owire_eval(stream, 0);
          ws_owire_code(stream, code);
          if ( stream->auto_mask ) {
              ws_owire_mask(stream);
          }
          _ws_mask(stream);
          _ws_head(stream);
          _ws_body(stream, data, size);
        ws_owire_end_frame(stream);
    }
    else
    {
        while ( used < size )
        {
            part = MIN(size-used, stream->auto_fragment);
            ws_owire_new_frame(stream, part);
            {
                if ( (used+part) >= size ) {
                    ws_owire_last(stream);
                }
                ws_owire_eval(stream, 0);
                if ( used == 0 ) {
                    ws_owire_code(stream, code);
                } else {
                    ws_owire_code(stream, 0);
                }
                if ( stream->auto_mask ) {
                    ws_owire_mask(stream);
                }
                _ws_mask(stream);
                _ws_head(stream);
                used += _ws_body(stream, data+used, part);
            }
            ws_owire_end_frame(stream);
        }
    }
    ws_owire_end_message(stream);
}

void ws_owire_init ( struct ws_owire * stream )
{
    stream->accept_content = 0;
    stream->rand = &_ws_unsafe_random_mask;
    stream->baton = 0;
    stream->auto_fragment = 0;
    stream->auto_mask = 0;
    stream->state = &_ws_fail;
    stream->pass = 0;
}

void ws_owire_new_message ( struct ws_owire * stream )
{
    memset(stream->data, 0, 14);
    stream->state = &_ws_fail;
    stream->pass = 0;
    stream->used = 0;
}

void ws_owire_end_message ( struct ws_owire * stream )
{
    memset(stream->data, 0, 14);
    stream->state = &_ws_fail;
    stream->pass = 0;
    stream->used = 0;
}

void ws_owire_new_frame ( struct ws_owire * stream, uint64 size )
{
    memset(stream->data, 0, 14);
    stream->state = &_ws_fail;
    stream->pass = size;
    if ( size < 126 )
    {
        stream->data[1] &= 0x7f;
        stream->data[1] |= size;
    }
    else if ( size < 65537 )
    {
        stream->data[1] &= 0x7f;
        stream->data[1] |= 126;
        stream->data[2] = ((size >> 8) & 0xff);
        stream->data[3] = ((size >> 0) & 0xff);
    }
    else
    {
        stream->data[1] &= 0x7f;
        stream->data[1] |= 127;
        stream->data[2] = ((size >> 56) & 0xff);
        stream->data[3] = ((size >> 48) & 0xff);
        stream->data[4] = ((size >> 40) & 0xff);
        stream->data[5] = ((size >> 32) & 0xff);
        stream->data[6] = ((size >> 24) & 0xff);
        stream->data[7] = ((size >> 16) & 0xff);
        stream->data[8] = ((size >>  8) & 0xff);
        stream->data[9] = ((size >>  0) & 0xff);
    }
}

void ws_owire_end_frame ( struct ws_owire * stream )
{
    memset(stream->data, 0, 14);
    stream->state = &_ws_fail;
    stream->pass = 0;
}

void ws_owire_last ( struct ws_owire * stream )
{
    // set bit 8.
    stream->data[0] |= 0x80;
}

void ws_owire_eval ( struct ws_owire * stream, uint8 eval )
{
    // clear, then set bits 5-7.
    stream->data[0] &= 0xf1;
    stream->data[0] |= ((eval & 0x07) << 4);
}

void ws_owire_code ( struct ws_owire * stream, uint8 code )
{
    // clear, then set bits 5-7.
    stream->data[0] &= 0xf0;
    stream->data[0] |= (code & 0x0f);
}

void ws_owire_mask ( struct ws_owire * stream )
{
    // set bit 8.
    stream->data[1] |= 0x80;
}

uint64 ws_owire_feed
    ( struct ws_owire * stream, const void * data, uint64 size )
{
    return (stream->state(stream, static_cast<const uint8*>(data), size));
}

void ws_owire_put_text
    ( struct ws_owire * stream, const void * data, uint64 size )
{
    ws_owire_put_full(stream, static_cast<const uint8*>(data), size, 0x01);
}

void ws_owire_put_data
    ( struct ws_owire * stream, const void * data, uint64 size )
{
    ws_owire_put_full(stream, static_cast<const uint8*>(data), size, 0x02);
}

void ws_owire_put_kill
    ( struct ws_owire * stream, const void * data, uint64 size )
{
    ws_owire_put_full(stream, static_cast<const uint8*>(data), size, 0x08);
}

void ws_owire_put_ping
    ( struct ws_owire * stream, const void * data, uint64 size )
{
    ws_owire_put_full(stream, static_cast<const uint8*>(data), size, 0x09);
}

void ws_owire_put_pong
    ( struct ws_owire * stream, const void * data, uint64 size )
{
    ws_owire_put_full(stream, static_cast<const uint8*>(data), size, 0x0a);
}

