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

Compiling
=========

This library provides a CMake_ build script.  To compile the library, download
and install CMake_, and then generate build scripts for your favorite build
tool.

For example, to use NMake_ on Windows, proceed with the following steps::

    (*) rem -- use the "vsvars*.bat" scripts in your visual studio
    (*) rem -- installation to put all the tools in your path or launch
    (*) rem -- the visual studio shell from Visual Studio.
    (*)
    (0) cd your/projects/folder
    (1) git clone git@github.com:AndreLouisCaron/cwebs.git
    (2) cd cwebs
    (3) mkdir work
    (4) cd work
    (5) cmake -G "NMake Makefiles" ..
    (6) nmake

Lines (3) through (5) will help you generate project files.  Line (6) invokes
the real build system used as a back-end and builds the project.  To generate a
build script for another build system or IDE (such as Visual Studio 2005), read
CMake's documentation to know what the generator name is.  Examples include:

* Visual Studio 2008: ``cmake -G "Visual Studio 9 2008" ..``
* Unix Makefiles: ``cmake -G "Unix Makefiles" ..``

References
==========

* A nice resource page about WebSockets [1]_
* The latest WebSocket draft speficiation [2]_

.. [1] http://websocket.org/
.. [2] http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol


.. _CMake: http://www.cmake.org/
.. _NMake: http://msdn.microsoft.com/en-us/library/ms930369.aspx
