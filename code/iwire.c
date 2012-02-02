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
 * @file iwire.c
 * @brief Web Socket wire protocol (in-bound) for C.
 *
 * @see http://tools.ietf.org/html/rfc6455
 */

#include "iwire.h"

  //
  // List all parser states in advance.
  //
#define WS_DECLARE_STATE(name) \
    static uint64 name \
        ( struct ws_iwire * stream, const uint8 * data, uint64 size );
WS_DECLARE_STATE(_ws_idle);
WS_DECLARE_STATE(_ws_wait);
WS_DECLARE_STATE(_ws_parse_size_1);
WS_DECLARE_STATE(_ws_parse_size_2);
WS_DECLARE_STATE(_ws_parse_size_3);
WS_DECLARE_STATE(_ws_parse_mask);
WS_DECLARE_STATE(_ws_parse_data);
WS_DECLARE_STATE(_ws_parse_data_1);
WS_DECLARE_STATE(_ws_parse_data_2);

  //
  // Initial state, waiting for a message to start.
  //
static uint64 _ws_idle
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    // signal start of message.
    stream->code = 0;
    if ( stream->new_message ) {
        stream->new_message(stream);
    }
    stream->state = &_ws_wait;
    return (0);
}

static void _ws_done ( struct ws_iwire * stream )
{
    if ( stream->end_fragment ) {
        stream->end_fragment(stream);
    }
    stream->state = &_ws_wait;
    if ( stream->last )
    {
        if ( stream->end_message ) {
	    stream->end_message(stream);
	}
        stream->code = 0;
        stream->state = &_ws_idle;
    }
}

  //
  // Parsing a fragmented message, waiting for next frame.
  //
static uint64 _ws_wait
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    uint8 code = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // parse fields.
        stream->last = ((byte & 0x80) != 0);
        stream->eval = ((byte & 0x70) >> 4);
        code = ((byte & 0x0f) >> 0);
        // for fragmented messags, the opcode is set on the first frame
        // only and is required to be 0 on subsequent frames.
        if ((stream->code != 0) && (code != 0))
        {
            // don't be fussy unless peer sends a different opcode.
            if ( stream->code != code )
            {
                // ...
            }
        }
        if ( stream->code == 0 ) {
            stream->code = code;
        }
        // done.  look at fragment size.
        stream->state = &_ws_parse_size_1; break;
    }
    return (used);
}

  //
  // Parsing a frame, waiting for 7-bit frame size to arrive.
  //
static uint64 _ws_parse_size_1
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // parse fields.
        stream->usem    = ((byte & 0x80) != 0);
        stream->data[0] = ((byte & 0x7f) >> 0);
        stream->size = 1;
        // parse extended size, if necessary.
        if ( stream->data[0] == 126 ) {
            stream->size = 0;
            stream->state = &_ws_parse_size_2; break;
        }
        if ( stream->data[0] == 127 ) {
            stream->size = 0;
            stream->state = &_ws_parse_size_3; break;
        }
        // commit size.	
	stream->pass = stream->data[0];
	if ( stream->new_fragment ) {
            stream->new_fragment(stream, stream->pass);
        }
        stream->size = 0;
        // start parsing mask.
        stream->state = &_ws_parse_mask; break;
    }
    return (used);
}

  //
  // Parsing a frame, waiting for 16-bit frame size to arrive.
  //
static uint64 _ws_parse_size_2
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // store it in the staging area.
        stream->data[stream->size++] = byte;
        // stop parsing when done.
        if ( stream->size == 2 )
        {
            const uint16 size =
                (((uint16)stream->data[0] << 8)
                |((uint16)stream->data[1] << 0));
            stream->pass = size;
            if ( stream->new_fragment ) {
                stream->new_fragment(stream, stream->pass);
            }
            stream->size = 0;
            // start parsing mask.
            stream->state = &_ws_parse_mask; break;
        }
    }
    return (used);
}

  //
  // Parsing a frame, waiting for 64-bit frame size to arrive.
  //
static uint64 _ws_parse_size_3
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // store it in the staging area.
        stream->data[stream->size++] = byte;
        // stop parsing when done.
        if ( stream->size == 8 )
        {
            // assemble fragment size.
            const uint64 size =
                (((uint64)stream->data[0] << 56)
                |((uint64)stream->data[1] << 48)
                |((uint64)stream->data[2] << 40)
                |((uint64)stream->data[3] << 32)
                |((uint64)stream->data[4] << 24)
                |((uint64)stream->data[5] << 16)
                |((uint64)stream->data[6] <<  8)
                |((uint64)stream->data[7] <<  0));
            // notify start of fragment.
	    stream->pass = size;
	    if ( stream->new_fragment ) {
	        stream->new_fragment(stream, size);
	    }
            stream->size = 0;
            // start parsing mask.
            stream->state = &_ws_parse_mask; break;
        }
    }
    return (used);
}

  //
  // Parsing frame, waiting for mask to arrive.  If the frame payload is not
  // masked, then simply forwards to the payload state.
  //
static uint64 _ws_parse_mask
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    // there's no easy way to check for complete submissal of frame
    // payload for empty frames.  the do-while loop in the '_feed()'
    // method guarantees this function will be called at least once.
    // complete the frame right away if it is empty, this avoids
    // complicating the simple '_feed()' method.
    uint64 used = 0;
    if ( !stream->usem )
    {
        stream->used = 0;
        stream->state = &_ws_parse_data;
        // fast-track to next message (see comment above).
        if ( stream->pass == 0 ) {
            return (_ws_parse_data(stream, data+used, size-used));
        }
        return (used);
    }
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // store it in the staging area.
        stream->data[stream->size++] = byte;
        // stop parsing when done.
        if ( stream->size == 4 )
        {
            // store mask.
            stream->mask[0] = stream->data[0];
            stream->mask[1] = stream->data[1];
            stream->mask[2] = stream->data[2];
            stream->mask[3] = stream->data[3];
            stream->size = 0;
            // start parsing data.
            stream->used = 0;
            stream->state = &_ws_parse_data; break;
        }
    }
    // fast-track to next message (see comment above).
    if ( stream->pass == 0 ) {
        return (used + _ws_parse_data(stream, data+used, size-used));
    }
    return (used);
}

  //
  // Parsing message payload, forward data to owner.
  //
static uint64 _ws_parse_data_1
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    // don't smear across frames.
    const uint64 used = MIN(stream->pass, size);
    // pass all possible data.
    stream->accept_content(stream, data, used);
    // update cursors.
    stream->pass -= used;
    stream->used += used;
    // prepare for next frame.
    if ( stream->pass == 0 ) {
        _ws_done(stream);
    }
    return (used);
}

  //
  // Parsing message payload, unmask data and forward to owner.
  //
static uint64 _ws_parse_data_2
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    uint8 bufdata[256];
    uint8 bufsize = 0;
    // don't smear across frame boundaries.
    size = MIN(stream->pass, size);
    // start parsing data.
    while ( used < size )
    {
        // copy butes to buffer and un-mask.
        for ( bufsize = 0; (bufsize < size); ++bufsize ) {
            bufdata[bufsize] = data[used++] ^ stream->mask[stream->used++%4];
        }
        // pass data to stream owner.
        stream->accept_content(stream, bufdata, bufsize);
    }
    // adjust cursors.
    stream->pass -= used;
    // prepare for next frame.
    if ( stream->pass == 0 ) {
        _ws_done(stream);
    }
    return (used);
}

  //
  // Parsing frame payload, forward to mask or unmasked data handler.
  //
static uint64 _ws_parse_data
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    return (stream->usem?
        _ws_parse_data_2(stream, data, size) :
        _ws_parse_data_1(stream, data, size));
}

void ws_iwire_init ( struct ws_iwire * stream )
{
    stream->new_message = 0;
    stream->end_message = 0;
    stream->new_fragment = 0;
    stream->end_fragment = 0;
    stream->accept_content = 0;
    stream->usem = 0;
    stream->size = 0;
    stream->code = 0;
    stream->good = 1;
    stream->state = &_ws_idle;
}

uint64 _ws_iwire_feed
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    do {
        // Note: invoking the state handler might move the parser to a new
        //   state.  In the end, all data will be consumed by one state or
        //   the other.
        used += (*stream->state)(stream, data+used, size-used);
    }
    while ( used < size );
    return (used);
}

uint64 ws_iwire_feed
    ( struct ws_iwire * stream, const void * data, uint64 size )
{
    return (_ws_iwire_feed(stream, (const uint8*)data, size));
}

int ws_iwire_mask ( const struct ws_iwire * stream )
{
    return (stream->usem);
}

int ws_iwire_last ( const struct ws_iwire * stream )
{
    return (stream->last);
}

int ws_iwire_ping ( const struct ws_iwire * stream )
{
    return (stream->code == 0x09);
}

int ws_iwire_pong ( const struct ws_iwire * stream )
{
    return (stream->code == 0x0a);
}

int ws_iwire_text ( const struct ws_iwire * stream )
{
    return (stream->code == 0x01);
}

int ws_iwire_data ( const struct ws_iwire * stream )
{
    return (stream->code == 0x02);
}

int ws_iwire_dead ( const struct ws_iwire * stream )
{
    return (stream->code == 0x08);
}

