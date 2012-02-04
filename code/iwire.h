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

/*!
 * @brief Checks that a supplied message type is valid.
 * @return 1 if the message type is valid, else 0.
 */
int ws_known_message_type ( int type );

typedef enum ws_iwire_status
{
    /*!
     * @brief No errors were detected.
     */
    ws_iwire_ok,

    /*!
     * @brief An invalid extension field was detected.
     */
    ws_iwire_invalid_extension,

    /*!
     * @brief A frame containing an unkonwn message type was detected.
     */
    ws_iwire_unknown_message_type,

} ws_iwire_status;

struct ws_iwire;

/*!
 * @internal
 * @brief Function prototype of parser state handlers.
 * @param wire Current parser object state.
 * @param data Array of bytes available to the parser state handler.  Accessing
 *  past @a size bytes in this array results in undefined behavior.
 * @param size Number of bytes in @a data the state can process.  
 * @return The number of bytes consumed by the state.
 *
 * Handlers can process up to @a size bytes starting at @a data as part of
 * accomplishing the task they are assigned.  They may change any internal
 * state in @a wire.  When they finished execution, they express state
 * transition by assigning a new state to @c wire->state.
 *
 * @see ws_iwire::state
 */
typedef uint64(*ws_iwire_state)
    (struct ws_iwire * wire, const uint8 * data, uint64 size);

/*!
 * @brief Incremental parser for WebSocket wire protocol (in-bound).
 *
 * @see ws_owire
 * @see ws_iwire_state
 * @see http://tools.ietf.org/html/rfc6455
 *
 * @bug The current parser implementation cannot handle control messages
 *  injected between message fragments (see RFC6455, section 5.4).
 */
struct ws_iwire
{
    /*!
     * @public
     * @brief Called to signal that a new message has started.
     * @param wire The current parser state.
     *
     * @warning The current parser state (other than @c baton) is unreliable:
     *  this callback is invoked @e before the frame header is parsed.  This
     *  callback should mostly be used to clear any leftover state from the
     *  previous message.
     *
     * @see baton
     */
    void(*new_message)(struct ws_iwire * wire);

    /*!
     * @public
     * @brief Called to signal that all message fragments were parsed.
     * @param wire The current parser state.
     *
     * This callback should be used to commit any buffered message contents
     * and reset any external state before processing the next message.
     *
     * If the message was a closing control frame, the application should stop
     * processing input data and possibly shutdown input on the TCP connection.
     *
     * @see baton
     * @see ws_iwire_dead
     */
    void(*end_message)(struct ws_iwire * wire);

    /*!
     * @public
     * @brief Called to signal that a new message fragment has arrived.
     * @param wire The current parser state.
     * @param size The size of the message fragment (see note below).
     *
     * This callback should be used to prepare to accept the message payload.
     * At this point, most of the frame's header has been parsed and the
     * different state checks (e.g. the message type) can be performed.
     *
     * This call will be followed by zero or more calls to @c accept_content(),
     * which will then be followed by a call to @c end_fragment().
     *
     * @note If the message is fragmented (@c ws_iwire_last(wire)==0), then the
     *  total size of the message is unavailable.  The WebSocket specification
     *  thus allows messages of unlimited size.
     *
     * @see baton
     * @see ws_iwire_last
     * @see ws_iwire_mask
     * @see ws_iwire_ping
     * @see ws_iwire_pong
     * @see ws_iwire_text
     * @see ws_iwire_data
     * @see ws_iwire_dead
     */
    void(*new_fragment)(struct ws_iwire * wire, uint64 size);

    /*!
     * @public
     * @brief Called to signal that the current fragment is complete.
     * @param wire The current parser state.
     *
     * This callback should be used to commit any buffered message contents.
     *
     * @see baton
     */
    void(*end_fragment)(struct ws_iwire * wire);

    /*!
     * @public
     * @brief Called to signal that some of the message payload is available.
     * @param wire The current parser state.
     * @param data Array of bytes available to the parser state handler.
     *  Accessing past @a size bytes in this array results in undefined
     *  behavior.
     * @param size Number of bytes in @a data the application can process.
     *
     * The application should not assume that the entire fragment payload is
     * received at once.  The parser forwards all data as it becomes available
     * and may break up message fragments to avoid buffering data internally.
     * If the application wishes to buffer incoming fragments to process entire
     * fragments at once, it may do so by accumulating the data for successive
     * calls until @c end_fragment() is called.
     *
     * @see baton
     */
    void(*accept_content)
        (struct ws_iwire * wire, const void * data, uint64 size);

    /*!
     * @public
     * @brief The current parser status.
     *
     * This value should be considered as read-only and should never be
     * modified by applications.
     */
    ws_iwire_status status;

    /*!
     * @public
     * @brief Defines the which extension fields may be enabled.
     *
     * This is a 3-bit mask.  Each incoming frame's extension field will be
     * checked against this mask.  If a frame's extension field has any enabled
     * bits not in this mask, an error will be reported.
     *
     * This value is 0 by default, meaning all extensions are rejected.  Set to
     * a non-zero value to (partially) enable WebSocket wire protocol
     * extensions.
     */
    uint8 extension_mask;

    /*!
     * @public
     * @brief External state reserved for use by application callbacks.
     *
     * @see new_message()
     * @see end_message()
     * @see new_fragment()
     * @see end_fragment()
     * @see accept_content()
     */
    void * baton;

    /*!
     * @internal
     * @private
     * @brief Current message's type.
     */
    int message_type;

    /*!
     * @internal
     * @private
     * @brief Current frame's extension code.
     */
    int extension_code;

    /*!
     * @internal
     * @private
     * @brief 1 if the current frame ends the message, else 0.
     */
    int last;

    /*!
     * @internal
     * @private
     * @brief 1 if the current frame's payload is masked, else 0.
     *
     * @see mask
     */
    int usem;

    /*!
     * @internal
     * @private
     * @brief 1 if the parser state is safe, 0 if an error occured.
     */
    int good;

    /*!
     * @internal
     * @private
     * @brief Current parser state handler.
     *
     * This function reference is changed at state handlers to make the
     * transition to another state.
     */
    ws_iwire_state state;

    /*!
     * @internal
     * @private
     * @brief Fixed-size buffer used to parse frame headers.
     *
     * @see size
     */
    uint8 data[8];

    /*!
     * @internal
     * @private
     * @brief Number of bytes buffered in @a data.
     *
     * @see size
     */
    uint64 size;

    /*!
     * @internal
     * @private
     * @brief Current frame's mask, if any.
     *
     * The contents of the mask are undefined if @c usem is 0.
     *
     * @see usem
     */
    uint8 mask[4];

    /*!
     * @internal
     * @private
     * @brief Amount of processed bytes in the current fragment's payload.
     *
     * This field is always updated after calling @c accept_content().  It
     * reflects the amount of data already processed by the application.
     *
     * @see pass
     */
    uint64 used;

    /*!
     * @internal
     * @private
     * @brief Amount of unprocessed bytes in the current frame's payload.
     *
     * This field is always updated after calling @c accept_content().  It
     * reflects the amount of data left to process by the application.
     *
     * @see used
     */
    uint64 pass;
};

/*!
 * @brief Initialize a parser.
 * @param stream Uninitialized parser object.
 *
 * Invoking this function clears @e all state, including application callbacks.
 */
void ws_iwire_init ( struct ws_iwire * stream );

/*!
 * @brief Consume available data and trigger appropriate application callbacks.
 * @param stream The current parser state.
 * @param data Array of bytes available to the parser.  Accessing past @a size
 *  bytes in this array results in undefined behavior.
 * @param size Number of bytes in @a data the state can process.  
 * @return The number of bytes consumed by the state.  This is normally always
 *  equal to @a size.  If they are different, an error most likely occured in
 *  parsing.  However, the returned value being equal to @a size does not
 *  guarantee the absence of an error.
 *
 * This function always consumes as much data as it can.  Therefore, a single
 * invocation of this function may happend to parse many messages if lots of
 * data has accumulated in the application's buffers.  It is recommended that
 * the application call this method often to ensure prompt responses and low
 * latency.
 */
uint64 ws_iwire_feed
    ( struct ws_iwire * stream, const void * data, uint64 size );

/*!
 * @brief Check if the current frame is masked.
 * @param stream The current parser state.
 * @return 1 if the fragment is masked, else 0.
 *
 * @warning This cannot be used inside the @c ws_iwire::new_message callback
 *  because that callback is triggered before the message frame is parsed.
 *
 * @see ws_iwire::new_fragment
 * @see ws_iwire::reqm
 */
int ws_iwire_mask ( const struct ws_iwire * stream );

/*!
 * @brief Check if the current frame is the message's last fragment.
 * @param stream The current parser state.
 * @return 1 if the fragment ends a message, else 0.
 *
 * @warning This cannot be used inside the @c ws_iwire::new_message callback
 *  because that callback is triggered before the message frame is parsed.
 *
 * @see ws_iwire::new_fragment
 */
int ws_iwire_last ( const struct ws_iwire * stream );

/*!
 * @brief Check if the current frame is a ping message.
 * @param stream The current parser state.
 * @return 1 if the fragment consists of a ping message, else 0.
 *
 * The application should respond with a pong message containing identical
 * data.  Ping pong messages help reset timeouts, ensuring the connection is
 * not killed until either peer decides to close it.
 *
 * @note Ping and pong messages implement "heartbeats".  This helps reset
 *  timeouts, ensuring the connection is not killed until either peer decides
 *  to close it.
 *
 * @warning This cannot be used inside the @c ws_iwire::new_message callback
 *  because that callback is triggered before the message frame is parsed.
 *
 * @see ws_iwire::new_fragment
 * @see ws_iwire::accept_content
 */
int ws_iwire_ping ( const struct ws_iwire * stream );

/*!
 * @brief Check if the current frame is a pong message.
 * @param stream The current parser state.
 * @return 1 if the fragment consists of a pong message, else 0.
 *
 * The application should validate that the pong message contains data
 * identical to that sent in a ping message.  Note that the WebSocket
 * specification allows unsolicited pong messages for implementation of
 * unidirectional heartbeats.  This may be useful if the application wants to
 * test the socket for writability, which is the only reliable way to determine
 * if a TCP connection has been broken).
 *
 * @note Ping and pong messages implement "heartbeats".  This helps reset
 *  timeouts, ensuring the connection is not killed until either peer decides
 *  to close it.
 *
 * @warning This cannot be used inside the @c ws_iwire::new_message callback
 *  because that callback is triggered before the message frame is parsed.
 *
 * @see ws_iwire::new_fragment
 * @see ws_iwire::accept_content
 */
int ws_iwire_pong ( const struct ws_iwire * stream );

/*!
 * @brief Check if the current fragment is a UTF-8 text message.
 * @param stream The current parser state.
 * @return 1 if the fragment contains a UTF-8 payload, else 0.
 *
 * @warning This cannot be used inside the @c ws_iwire::new_message callback
 *  because that callback is triggered before the message frame is parsed.
 *
 * @see ws_iwire::new_fragment
 * @see ws_iwire::accept_content
 */
int ws_iwire_text ( const struct ws_iwire * stream );

/*!
 * @brief Check if the current fragment is a binary message.
 * @param stream The current parser state.
 * @return 1 if the fragment contains a binary payload, else 0.
 *
 * @warning This cannot be used inside the @c ws_iwire::new_message callback
 *  because that callback is triggered before the message frame is parsed.
 *
 * @see ws_iwire::new_fragment
 * @see ws_iwire::accept_content
 */
int ws_iwire_data ( const struct ws_iwire * stream );

/*!
 * @brief Check if the current fragment is a close frame.
 * @param stream The current parser state.
 * @return 1 if the fragment is the last message, else 0.
 *
 * Closing frames may supply a 16-bit unsigned integer and a UTF-8 payload to
 * describe the reason for closing the connection.  Interpretation of the
 * status code and error text are up to the application.  The WebSocket
 * specification does not require the UTF-8 error text to be human readable.
 *
 * @warning This cannot be used inside the @c ws_iwire::new_message callback
 *  because that callback is triggered before the message frame is parsed.
 *
 * @see ws_iwire::new_fragment
 * @see ws_iwire::accept_content
 * @see ws_iwire::end_message
 */
int ws_iwire_dead ( const struct ws_iwire * stream );

#ifdef __cplusplus
}
#endif

#endif /* _iwire_h__ */

