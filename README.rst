=========================================================================
  `cwebs` --- Incremental parser for the web socket wire protocol, in C
=========================================================================
:authors:
   André Caron
:contact: andre.l.caron@gmail.com

Description
===========

This library provides a parser for the `WebSocket`_ wire protocol.  The parser
is implemented as a finite state machine (FSM) for use in streaming applications
(i.e. data arrives at an unpredictable rate and the parser must be
interruptible).  As such, the parser itself does not buffer any received data.
It just forwards it to registered callbacks.  It requires little overhead and is
well suited for being used in an object-oriented wrapper.

Demonstration
=============

There's a C++-based tunnel program with both UNIX and Windows implementations
based on nothing but the native APIs.  They can be used to tunnel arbitrary
data exchange over port 80, all with standard HTTP semantics for virtual
hosting, URL-based dispatch, encryption, etc. (caveat: very few web servers
implement facilities for such a dispatch given WebSockets).

There's a full-blown WebSockets server based on `Qt`_ and the `http-parser`_
library.  You can test the echo server with the echo client project which is
implemented using the `HTML 5 WebSocket API`_ and jQuery_.

Compiling
=========

This library provides a CMake_ build script.  To compile the library, download
and install CMake_, and then generate build scripts for your favorite build
tool.

On Microsoft Windows
--------------------

#. launch the visual studio command prompt

#. check out the source code (or download a source code bundle)

   ::

      git clone git@github.com:AndreLouisCaron/cwebs.git
      cd cwebs
      git submodule init
      git submodule update

#. generate NMake project files

   ::

      mkdir work
      cd work
      cmake -G "NMake Makefiles" ..

#. compile the libraries and tests

   ::

      nmake

#. build the HTML documentation (optional)

   ::

      nmake doc

#. run the tests (optional)

   ::

      nmake /A test

You can use JOM_ as an alternative to NMake to accelerate builds on
multi-processor machines.  Use the ``"NMake Makefiles JOM"`` CMake
generator and use ``jom`` instead of NMake.  Beware that the ``test``
target is broken and you must still invoke ``nmake /A test`` to run
the tests.

On UNIX-like systems
--------------------

#. launch your favorite terminal

#. check out the source code (or download a source code bundle)

   ::

      git clone git@github.com:AndreLouisCaron/cwebs.git
      cd cwebs
      git submodule init
      git submodule update

#. generate Makefiles

   ::

      mkdir work
      cd work
      cmake ..

#. compile the libraries and tests

   ::

      make

#. build the HTML documentation (optional)

   ::

      make doc

#. run the tests (optional)

   ::

      make test

License
=======

The code is distributed under the simplified 2-clause BSD licence.  It is
absolutely free to use in both open source and commercial applications,
provided you don't take credit for my work.

You don't need my consent or anything to use the software, but it would be nice
of you to tell me if you're using it.  It would allow me to keep a list of most
notable uses, giving credibility to the software and ensuring it is maintained
properly.  I also like to know that people are you using my software :-)

Here is a verbatim copy of the license:

::

   Copyright (c) 2011-2012, Andre Caron (andre.l.caron@gmail.com)
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

References
==========

* A nice `resource page about WebSockets`_
* The `WebSocket specification`_

.. _`HTML 5 WebSocket API`: http://dev.w3.org/html5/websockets/
.. _`resource page about WebSockets`: http://websocket.org/
.. _`WebSocket specification`: http://tools.ietf.org/html/rfc6455

.. _WebSocket: http://websocket.org/
.. _CMake: http://www.cmake.org/
.. _NMake: http://msdn.microsoft.com/en-us/library/ms930369.aspx
.. _`Qt`: http://qt.nokia.com/products/
.. _jQuery: http://jquery.com/
.. _`http-parser`: https://github.com/joyent/http-parser
.. _JOM: https://qt.gitorious.org/qt-labs/jom
