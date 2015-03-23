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
#include <stddef.h>

/*!
 * @internal
 * @defgroup parser-states Incremental parser finite machine states.
 */

/*!
 * @internal
 * @def WS_DECLARE_STATE
 * @brief Simple shorthand to declare websocket parser state handlers.
 */
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

/*!
 * @internal
 * @ingroup parser-states
 * @brief Idle state, expecting the start of the next message.
 */
static uint64 _ws_idle
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    // signal start of message.
    stream->message_type = 0;
    if ( stream->new_message ) {
        stream->new_message(stream);
    }
    stream->handler = &_ws_wait;
    return (0);
}

/*!
 * @internal
 * @brief Invokes end-of-frame and end-of-message callbacks and resets state.
 */
static void _ws_done ( struct ws_iwire * stream )
{
    if ( stream->end_fragment ) {
        stream->end_fragment(stream);
    }
    stream->handler = &_ws_wait;
    if ( stream->last_fragment )
    {
        if ( stream->end_message ) {
	    stream->end_message(stream);
	}
        stream->message_type = 0;
        stream->handler = &_ws_idle;
    }
}

/*!
 * @internal
 * @ingroup parser-states
 * @brief Idle state, expecting a fragment that continues a fragmented message.
 */
static uint64 _ws_wait
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    uint8 message_type = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // parse fields.
        stream->last_fragment = ((byte & 0x80) != 0);
        stream->extension_code = ((byte & 0x70) >> 4);
        message_type = ((byte & 0x0f) >> 0);
        // check for invalid extension fields.
        if ((stream->extension_code & ~stream->extension_mask) != 0)
        {
            stream->status = ws_iwire_invalid_extension;
            return (used);
        }
        // for fragmented messages, the opcode is set on the first
        // frame only and is required to be 0 on subsequent frames.
        if ((stream->message_type != 0) && (message_type != 0))
        {
            stream->status = ws_iwire_message_type_changed;
            return (used);
        }
        // if this is the first fragment, store the message type.
        if ( stream->message_type == 0 )
        {
            // make sure the message type is supported.
            if (!ws_known_message_type(message_type))
            {
                stream->status = ws_iwire_unknown_message_type;
                return (used);
            }
            stream->message_type = message_type;
        }
        // done.  look at fragment size.
        stream->handler = &_ws_parse_size_1; break;
    }
    return (used);
}

/*!
 * @internal
 * @ingroup parser-states
 * @brief Check first byte of message size.
 *
 * If the first byte is one of the special values 126 or 127, then the fragment
 * size is parsed as 2 or 8 bytes, respectively.  In that case, the state is
 * forwarded to the appropriate handlers.
 *
 * @see _ws_parse_size_2
 * @see _ws_parse_size_3
 */
static uint64 _ws_parse_size_1
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // parse fields.
        stream->unmask_payload = ((byte & 0x80) != 0);
        stream->buffer[0] = ((byte & 0x7f) >> 0);
        stream->stored = 1;
        // reject unmasked frames if masking is required by the host.
        if (stream->masking_required && !stream->unmask_payload)
        {
            stream->status = ws_iwire_masking_required;
            return (used);
        }
        // parse extended size, if necessary.
        if ( stream->buffer[0] == 126 ) {
            stream->stored = 0;
            stream->handler = &_ws_parse_size_2; break;
        }
        if ( stream->buffer[0] == 127 ) {
            stream->stored = 0;
            stream->handler = &_ws_parse_size_3; break;
        }
        // commit size.	
	stream->pass = stream->buffer[0];
	if ( stream->new_fragment ) {
            stream->new_fragment(stream, stream->pass);
        }
        stream->stored = 0;
        // start parsing mask.
        stream->handler = &_ws_parse_mask; break;
    }
    return (used);
}

/*!
 * @internal
 * @ingroup parser-states
 * @brief Accumulate and parse 2 byte fragment size.
 *
 * @see _ws_parse_size_1
 */
static uint64 _ws_parse_size_2
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // store it in the staging area.
        stream->buffer[stream->stored++] = byte;
        // stop parsing when done.
        if ( stream->stored == 2 )
        {
            const uint16 size =
                (((uint16)stream->buffer[0] << 8)
                |((uint16)stream->buffer[1] << 0));
            stream->pass = size;
            if ( stream->new_fragment ) {
                stream->new_fragment(stream, stream->pass);
            }
            stream->stored = 0;
            // start parsing mask.
            stream->handler = &_ws_parse_mask; break;
        }
    }
    return (used);
}

/*!
 * @internal
 * @ingroup parser-states
 * @brief Accumulate and parse 8 byte fragment size.
 *
 * @see _ws_parse_size_1
 */
static uint64 _ws_parse_size_3
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    while ( used < size )
    {
        // fetch next byte.
        const uint8 byte = data[used++];
        // store it in the staging area.
        stream->buffer[stream->stored++] = byte;
        // stop parsing when done.
        if ( stream->stored == 8 )
        {
            // assemble fragment size.
            const uint64 size =
                (((uint64)stream->buffer[0] << 56)
                |((uint64)stream->buffer[1] << 48)
                |((uint64)stream->buffer[2] << 40)
                |((uint64)stream->buffer[3] << 32)
                |((uint64)stream->buffer[4] << 24)
                |((uint64)stream->buffer[5] << 16)
                |((uint64)stream->buffer[6] <<  8)
                |((uint64)stream->buffer[7] <<  0));
            // notify start of fragment.
	    stream->pass = size;
	    if ( stream->new_fragment ) {
	        stream->new_fragment(stream, size);
	    }
            stream->stored = 0;
            // start parsing mask.
            stream->handler = &_ws_parse_mask; break;
        }
    }
    return (used);
}

/*!
 * @internal
 * @ingroup parser-states
 * @brief Accumulate and parse 4 byte mask.
 *
 * If the frame is not masked, forward directly to the next state.
 */
static uint64 _ws_parse_mask
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    // there's no easy way to check for complete submissal of frame
    // payload for empty frames.  the do-while loop in the '_feed()'
    // method guarantees this function will be called at least once.
    // complete the frame right away if it is empty, this avoids
    // complicating the simple '_feed()' method.
    uint64 used = 0;
    if ( !stream->unmask_payload )
    {
        stream->used = 0;
        stream->handler = &_ws_parse_data;
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
        stream->buffer[stream->stored++] = byte;
        // stop parsing when done.
        if ( stream->stored == 4 )
        {
            // store mask.
            stream->mask[0] = stream->buffer[0];
            stream->mask[1] = stream->buffer[1];
            stream->mask[2] = stream->buffer[2];
            stream->mask[3] = stream->buffer[3];
            stream->stored = 0;
            // start parsing data.
            stream->used = 0;
            stream->handler = &_ws_parse_data; break;
        }
    }
    // fast-track to next message (see comment above).
    if ( stream->pass == 0 ) {
        return (used + _ws_parse_data(stream, data+used, size-used));
    }
    return (used);
}

/*!
 * @internal
 * @brief Process and forward unmasked frame payload.
 *
 * @see _ws_parse_data
 */
static uint64 _ws_parse_data_1
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    // don't smear across frames.
    const uint64 used = MIN(stream->pass, size);
    // pass all possible data.
    if ( stream->accept_content ) {
        stream->accept_content(stream, data, used);
    }
    // update cursors.
    stream->pass -= used;
    stream->used += used;
    // prepare for next frame.
    if ( stream->pass == 0 ) {
        _ws_done(stream);
    }
    return (used);
}

/*!
 * @internal
 * @brief Process and forward masked frame payload.
 *
 * @see _ws_parse_data
 */
static uint64 _ws_parse_data_2
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
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
        for ( bufsize = 0; ((used < size) && (bufsize < 256)); ++bufsize ) {
            bufdata[bufsize] = data[used++] ^ stream->mask[stream->used++%4];
        }
        // pass data to stream owner.
        if ( stream->accept_content ) {
            stream->accept_content(stream, bufdata, bufsize);
        }
    }
    // adjust cursors.
    stream->pass -= used;
    // prepare for next frame.
    if ( stream->pass == 0 ) {
        _ws_done(stream);
    }
    return (used);
}

/*!
 * @internal
 * @ingroup parser-states
 * @brief Process and forward frame payload.
 *
 * @see _ws_parse_data_1
 * @see _ws_parse_data_2
 */
static uint64 _ws_parse_data
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    return (stream->unmask_payload?
        _ws_parse_data_2(stream, data, size) :
        _ws_parse_data_1(stream, data, size));
}

/*!
 * @internal
 * @brief Consume available data and trigger appropriate application callbacks.
 *
 * @see ws_iwire_feed
 */
static uint64 _ws_iwire_feed
    ( struct ws_iwire * stream, const uint8 * data, uint64 size )
{
    uint64 used = 0;
    do {
        // Note: invoking the state handler might move the parser to a new
        //   state.  In the end, all data will be consumed by one state or
        //   the other.
        used += (*stream->handler)(stream, data+used, size-used);
    }
    while ((used < size) && (stream->status == ws_iwire_ok));
    return (used);
}

int ws_known_message_type ( int type )
{
    switch (type)
    {
    case 0x1: // text.
    case 0x2: // data.
    case 0x8: // kill.
    case 0x9: // ping.
    case 0xa: // pong.
        return 1;
    default:
        return 0;
    }
}

void ws_iwire_init ( struct ws_iwire * stream )
{
    stream->new_message = 0;
    stream->end_message = 0;
    stream->new_fragment = 0;
    stream->end_fragment = 0;
    stream->accept_content = 0;
    stream->masking_required = 0;
    stream->extension_mask = 0;
    stream->extension_code = 0;
    stream->unmask_payload = 0;
    stream->stored = 0;
    stream->message_type = 0;
    stream->handler = &_ws_idle;
    stream->status = ws_iwire_ok;
}

uint64 ws_iwire_feed
    ( struct ws_iwire * stream, const void * data, uint64 size )
{
    return (_ws_iwire_feed(stream, (const uint8*)data, size));
}

int ws_iwire_masked ( const struct ws_iwire * stream )
{
    return (stream->unmask_payload);
}

int ws_iwire_last_fragment ( const struct ws_iwire * stream )
{
    return (stream->last_fragment);
}

int ws_iwire_ping ( const struct ws_iwire * stream )
{
    return (stream->message_type == 0x09);
}

int ws_iwire_pong ( const struct ws_iwire * stream )
{
    return (stream->message_type == 0x0a);
}

int ws_iwire_text ( const struct ws_iwire * stream )
{
    return (stream->message_type == 0x01);
}

int ws_iwire_data ( const struct ws_iwire * stream )
{
    return (stream->message_type == 0x02);
}

int ws_iwire_dead ( const struct ws_iwire * stream )
{
    return (stream->message_type == 0x08);
}

