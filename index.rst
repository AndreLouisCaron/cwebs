==========================================
  `cwebs`: WebSocket wire protocol, in C
==========================================
:authors:
   André Caron
:contact: andre.l.caron@gmail.com

Description
===========

This library provides facilities for processing RFC6455_ WebSocket frames.  The
parser is implemented as a finite state machine (FSM) for use in streaming
applications (i.e. data arrives at an unpredictable rate and the parser must be
interruptible).  As such, the parser itself does not buffer any received data.
It just forwards it to registered callbacks.  It requires little overhead and is
well suited for being used in an object-oriented wrapper.

Source code
===========

Visit the repository_ on GitHub.  The front page includes up-to-date
instructions for compiling and testing the library and its demo projects.

Documentation
=============

Consult the documentation for tagged versions online:

* `v0.2.1 <v0.2.1/>`_
* `v0.2 <v0.2/>`_

.. _repository: https://github.com/AndreLouisCaron/cwebs

.. _RFC6455: http://tools.ietf.org/html/rfc6455
