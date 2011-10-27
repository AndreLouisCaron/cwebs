=========================================================================
  `cwebs` --- Incremental parser for the web socket wire protocol, in C
=========================================================================
:authors:
   André Caron
:contact: andre.l.caron@gmail.com

Description
===========

This library provides a parser for the WebSocket [1]_ wire protocol.  The parser
is implemented as a finite state machine (FSM) for use in streaming applications
(i.e. data arrives at an unpredictable rate and the parser must be
interruptible).  As such, the parser itself does not buffer any received data.
It just forwards it to registered callbacks.  It requires little overhead and is
well suited for being used in an object-oriented wrapper.

References
==========

* A nice resource page about WebSockets [1]_
* The latest WebSocket draft speficiation [2]_

.. [1] http://websocket.org/
.. [2] http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol
