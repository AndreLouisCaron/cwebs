#ifndef _webs_h__
#define _webs_h__

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
 * @file webs.h
 * @brief Web Socket wire protocol implementation in C.
 *
 * @see http://tools.ietf.org/html/rfc6455
 */

/*!
 * @mainpage cwebs: WebSocket wire protocol, in C.
 *
 * @section abstract Abstract
 *
 * This library provides facilities for processing WebSocket wire protocol
 * frames. The parser is implemented as a finite state machine (FSM) for use in
 * streaming applications (i.e. data arrives at an unpredictable rate and the
 * parser must be interruptible). As such, the parser itself does not buffer any
 * received data. It just forwards it to registered callbacks. It requires
 * little overhead and is well suited for being used in an object-oriented
 * wrapper.
 *
 * @see http://tools.ietf.org/html/rfc6455
 *
 * @section introduction Introduction
 *
 * It defines a pair of structures @c ws_iwire and @c ws_owire that respectively
 * parse and emit WebSocket frames.  The parsers implement only the low-level
 * bit-twidling facilities and leave the HTTP parsing and networking code up to
 * your application.  This means you have to do a bit more work, but allows the
 * library to be used in virtually any type of application, with any networking
 * library, etc.  When you receive data, feed it to the parser.  When you want
 * to reply, feed it to the emitter.  Register a few callbacks to process
 * decoded data and totrnasfer encoded data.  Then, focus on your application.
 *
 * To test the code, as well as to get you started on how to use the library,
 * there are a few demo programs worth looking at.  These demo programs are
 * based on different frameworks, including native Win32 and UNIX APIs, as well
 * as Qt.
 *
 * @section how-to How to use this library (overly simplified example)
 *
 * This mini how-to sample shows the key operations in a simple console chat
 * application that reads text from standard input and sends the message over
 * the websocket connection.  Text messages received from the websocket are
 * written to the standard output.  This code uses a hypothetical I/O interface
 * with descriptive function names to help focus on the calls involved with
 * this library rather than a particular I/O library.
 *
 * @code
 *
 *  void write_text ( ::ws_iwire * wire, const void * data, uint64 size )
 *  {
 *      // note that this callback would also be invoked for messages
 *      // containing binary data.  we'll assume the exact same code
 *      // is running on the other end.
 *
 *      // 1: use 'wire->baton' to recuperate standard output.
 *      // 2: write 'size' bytes of UTF-8 text starting at 'data'.
 *  }
 *
 *  void flush_line ( ::ws_iwire * wire )
 *  {
 *      // 1: use 'wire->baton' to recuperate standard output.
 *      // 2: write newline character and flush output buffer.
 *  }
 *
 *  void send_to_peer ( ::ws_owire * wire, const void * data, uint64 size )
 *  {
 *      // 1: use 'wire->baton' to recuperate socket object.
 *      // 2: send 'size' bytes starting at 'data'.
 *  }
 *
 *  int main ( int, char ** )
 *  {
 *      // connect socket, open streams, etc.
 *      standard_input = ...;
 *      socket_object = ...;
 *
 *      // use external HTTP parser, base64, SA-11 and whatenot libraries
 *      // to implement the WebSocket handshake (not provided by this
 *      // library).  look at the complete demo programs for concrete
 *      // implementation using only light-weight open source libraries.
 *      // ...
 *
 *      ws_iwire_init(&iwire);
 *      iwire.baton          = &standard_output;
 *      iwire.accept_content = &write_text;
 *      iwire.end_message    = &flush_line;
 *
 *      ws_owire_init(&owire);
 *      owire.baton          = &socket_object;
 *      owire.accept_content = &send_to_peer;
 *
 *      // simple buffer.
 *      char data[1024];
 *
 *      while (!exhausted(standard_input) ||
 *             !exhausted(socket_object))
 *      {
 *          wait_for_any(standard_input, socket_object);
 *
 *          if (can_read(standard_input)) {
 *             size = read(standard_input, data, sizeof(data));
 *             // (convert to UTF-8 if necessary)
 *             ws_owire_put_text(&owire, data, size);
 *          }
 *
 *          if (can_read(socket_object)) {
 *             size = read(socket_object, data, sizeof(data));
 *             // (convert from UTF-8 if necessary)
 *             ws_iwire_feed(&iwire, data, size);
 *          }
 *      }
 *  }
 *
 * @endcode
 *  
 *
 */

#include "types.h"
#include "iwire.h"
#include "owire.h"

#endif /* _webs_h__ */

