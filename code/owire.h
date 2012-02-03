#ifndef _owire_h__
#define _owire_h__

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
 * @file owire.h
 * @brief Web Socket wire protocol (out-bound) for C.
 *
 * @see http://tools.ietf.org/html/rfc6455
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @internal
 * @brief Function prototype of writer handlers.
 * @param wire Current parser object state.
 * @param data Array of bytes to write.  Accessing past @a size bytes in this
 *  array results in undefined behavior.
 * @param size Number of bytes in @a data to forward.
 * @return The number of bytes consumed by the handler.
 *
 * Handlers can process up to @a size bytes starting at @a data as part of
 * accomplishing the task they are assigned.
 *
 * @see ws_owire::state
 */
typedef uint64(*ws_owire_state)
    (struct ws_owire * wire, const uint8 * data, uint64 size);

/*!
 * @brief Writer error codes.
 */
enum ws_owire_error
{
    /*!
     * @brief No error, the parser is in good state.
     */
    ws_owire_eok,

    /*! @brief Attempted to send a frame's payload before the header was ready.
     */
    ws_owire_enotready,
};

/*!
 * @brief Incremental writer for WebSocket wire protocol (out-bound).
 *
 * @see ws_iwire
 * @see ws_owire_state
 * @see http://tools.ietf.org/html/rfc6455
 */
struct ws_owire
{
    /*!
     * @public
     * @brief Called to signal that some data should be transferred.
     * @param wire The current writer state.
     * @param data Array of bytes to be transferred to the peer. Accessing past
     *  @a size bytes in this array results in undefined behavior.
     * @param size Number of bytes in @a data the application must send.
     *
     * @see baton
     */
    void(*accept_content)
        (struct ws_owire * wire, const void * data, uint64 size);

    /*!
     * @public
     * @brief Called to signal that a mask must be generated.
     * @param wire The current writer state.
     * @param mask 4-byte array to fill in with random values.
     *
     * This method is invoked each time a masked frame is sent.  This can
     * either happend when using @c auto_mask, or when @c ws_owire_mask() is
     * called to manually add a mask.
     *
     * By default, this callback invokes the standard library @c rand() to
     * generated masks.  You should set this to a cryptographically secure
     * random number generator.  Also, care should be taken for thread safety
     * in threaded applications since the standard library @c rand()
     * implementation is not guaranteed to be thread-safe.
     *
     * @see auto_mask
     * @see ws_owire_mask()
     */
    void(*rand)(struct ws_owire * wire, uint8 mask[4]);

    /*!
     * @public
     * @brief External state reserved for use by application callbacks.
     *
     * @see accept_content()
     */
    void * baton;

    /*!
     * @public
     * @brief The writer object's current error code.
     *
     * This field should be considered as read-only, and must not be changed by
     * applications.  If paranoid, it should be checked after the call to each
     * method.  However, errors are only signaled when the object is misused
     * and cannot be triggered by use of the higher-level writer API functions.
     * Thus, once an application has been debugged, checking the error code
     * after each call is unnecessary.
     */
    enum ws_owire_error error;

    /*!
     * @public
     * @brief Set to a nonzero value to enable automatic fragmentation.
     *
     * Automatic fragmentation of messages is normally only used during
     * testing.  If set to any nonzero value, the writer will ensure that no
     * data or text messages will contain fragments large than this size
     * (control messages cannot be fragmented).  To achieve this, the writer
     * breaks up large messages into chuncks of at most @a auto_fragment bytes
     * in size.
     */
    uint64 auto_fragment;

    /*!
     * @public
     * @brief Set to 1 to enable automatic masking of outgoing frames.
     *
     * Clients are required to mask all outgoing frames and should enable
     * automatic masking for the higher-level writer API functions.  Servers
     * must never enable automatic masking.
     */
    int auto_mask;

    /*!
     * @internal
     * @private
     * @brief Fixed-size buffer used to generate frame headers.
     */
    uint8 data[14];

    /*!
     * @internal
     * @private
     * @brief Amount of processed bytes in the current fragment's payload.
     *
     * This field is mostly used during masking to track which masking byte
     * should be applied next.
     */
    uint64 used;

    /*!
     * @internal
     * @private
     * @brief Amount of unprocessed bytes in the current frame's payload.
     *
     * This field is used to protect the application from submitting too much
     * data in the fragment payload.  This ensures the stream is not corrupted
     * by malformed applications.
     */
    uint64 pass;

    /*!
     * @internal
     * @private
     * @brief Handles payload output to the application, masking if necessary.
     */
    ws_owire_state state;
};

/*!
 * @brief Initialize a writer.
 * @param stream Uninitialized writer object.
 *
 * Invoking this function clears @e all state, including application callbacks.
 */
void ws_owire_init ( struct ws_owire * stream );

/*!
 * @brief Start a new message.
 * @param stream Current writer state.
 *
 * @see ws_owire_new_frame()
 */
void ws_owire_new_message ( struct ws_owire * stream );

/*!
 * @brief End a message, clearing the state for the next message.
 * @param stream Current writer state.
 *
 * @see ws_owire_new_message()
 */
void ws_owire_end_message ( struct ws_owire * stream );

/*!
 * @brief Start a frame with a @a size byte payload.
 * @param stream Current writer state.
 * @param size Size of the frame payload, in bytes.
 *
 * @see ws_owire_last()
 * @see ws_owire_eval()
 * @see ws_owire_code()
 * @see ws_owire_mask()
 * @see ws_owire_feed()
 */
void ws_owire_new_frame ( struct ws_owire * stream, uint64 size );

/*!
 * @brief End a frame, clearing the state for the next frame.
 * @param stream Current writer state.
 *
 * @see ws_owire_new_frame()
 * @see ws_owire_end_message()
 */
void ws_owire_end_frame ( struct ws_owire * stream );

/*!
 * @brief Mark the current frame as the last fragment in a message.
 * @param stream The current writer state.
 *
 * When sending non-fragmented messages, this should always mark the only
 * fragment sent as part of the message.
 */
void ws_owire_last ( struct ws_owire * stream );

/*!
 * @brief Set the WebSocket extension code.
 * @param stream The current writer state.
 * @param eval Extension value, must be in [0,8).
 */
void ws_owire_eval ( struct ws_owire * stream, uint8 eval );

/*!
 * @brief Set the WebSocket message opcode.
 * @param stream The curernt writer state.
 * @param code The message opcode.
 *
 * This method should not be used unless the WebSocket protocol specification
 * is updated add new control message codes before the library is updated to
 * implement the extended specification.  Use the explicit named functions
 * for application clarity.
 *
 * @see ws_owire_text()
 * @see ws_owire_data()
 * @see ws_owire_ping()
 * @see ws_owire_pong()
 */
void ws_owire_code ( struct ws_owire * stream, uint8 code );

/*!
 * @brief Mark the fragment as masked.
 * @param stream The current writer state.
 *
 * This will indirectly invoke the @c ws_owire::rand callback to generate a
 * random mask.
 *
 * @see ws_owire::rand
 */
void ws_owire_mask ( struct ws_owire * stream );

/*!
 * @brief Add data to the application payload.
 * @param stream The current writer state.
 * @param data Array of bytes to write.  Accessing past @a size bytes in this
 *  array results in undefined behavior.
 * @param size Number of bytes in @a data to forward.
 *
 * You may invoke this function multiple times to reach the total fragment
 * payload size given to @c ws_owire_new_frame().
 *
 * @see ws_owire_new_frame()
 * @see ws_owire_end_frame()
 */
uint64 ws_owire_feed
    ( struct ws_owire * stream, const void * data, uint64 size );

/*!
 * @brief Mark the current frame as a ping message.
 * @param stream The current frame state.
 *
 * Control messages cannot be fragmented.  Always mark ping messages with
 * @c ws_owire_last().
 *
 * @see ws_owire_code()
 * @see ws_owire_last()
 * @see ws_owire_feed()
 */
void ws_owire_ping ( struct ws_owire * stream );

/*!
 * @brief Mark the current frame as a pong message.
 * @param stream The current frame state.
 *
 * Control messages cannot be fragmented.  Always mark pong messages with
 * @c ws_owire_last().
 *
 * @see ws_owire_code()
 * @see ws_owire_last()
 * @see ws_owire_feed()
 */
void ws_owire_pong ( struct ws_owire * stream );

/*!
 * @brief Mark the current frame as a text message.
 * @param stream The current writer state.
 *
 * @warning Remember that text messages' payload must be UTF-8 encoded data.
 *  This library implements no automatic encoding or decoding of text and it is
 *  the application's responsibility to ensure that the payload contains valid
 *  UTF-8 data.  If the peer strictly enforces the WebSocket specification, it
 *  may kill the connection upon receiving invalid UTF-8 data.
 *
 * @see ws_owire_code()
 * @see ws_owire_feed()
 */
void ws_owire_text ( struct ws_owire * stream );

/*!
 * @brief Mark the current frame as a binary data message.
 * @param stream The current writer state.
 *
 * @see ws_owire_code()
 * @see ws_owire_feed()
 */
void ws_owire_data ( struct ws_owire * stream );

/*!
 * @brief Mark the current frame as an end-of-stream control message.
 * @param stream The current frame state.
 *
 * Control messages cannot be fragmented.  Always mark end-of-stream messages
 * with @c ws_owire_last().
 *
 * @see ws_owire_code()
 * @see ws_owire_last()
 * @see ws_owire_feed()
 */
void ws_owire_kill ( struct ws_owire * stream );

/*!
 * @brief Send a full text message in a single call.
 * @param stream The current writer state.
 * @param data Payload data.
 * @param size Payload size, in bytes.
 *
 * @see ws_owire::auto_fragment
 */
void ws_owire_put_text
    ( struct ws_owire * stream, const void * data, uint64 size );

/*!
 * @brief Send a full binary data message in a single call.
 * @param stream The current writer state.
 * @param data Payload data.
 * @param size Payload size, in bytes.
 *
 * @see ws_owire::auto_fragment
 */
void ws_owire_put_data
    ( struct ws_owire * stream, const void * data, uint64 size );

/*!
 * @brief Send a full ping message in a single call.
 * @param stream The current writer state.
 * @param data Payload data.
 * @param size Payload size, in bytes.
 *
 * @note Control messages are never fragmented, regardless of the
 *  @c ws_owire::auto_fragment setting.
 */
void ws_owire_put_ping
    ( struct ws_owire * stream, const void * data, uint64 size );

/*!
 * @brief Send a full pong message in a single call.
 * @param stream The current writer state.
 * @param data Payload data.
 * @param size Payload size, in bytes.
 *
 * @note Control messages are never fragmented, regardless of the
 *  @c ws_owire::auto_fragment setting.
 */
void ws_owire_put_pong
    ( struct ws_owire * stream, const void * data, uint64 size );

/*!
 * @brief Send a full end-of-stream message in a single call.
 * @param stream The current writer state.
 * @param data Payload data.
 * @param size Payload size, in bytes.
 *
 * The payload must be UTF-8 encoded text.
 *
 * @note Control messages are never fragmented, regardless of the
 *  @c ws_owire::auto_fragment setting.
 *
 * @bug The end-of-stream message must set a 2-byte status code before it
 *  writes the fragment's payload.
 */
void ws_owire_put_kill
    ( struct ws_owire * stream, const void * data, uint64 size );

#ifdef __cplusplus
}
#endif

#endif /* _owire_h__ */

