// Copyright(c) Andre Caron <andre.l.caron@gmail.com>, 2011
//
// This document is covered by the an Open Source Initiative approved license. A
// copy of the license should have been provided alongside this software package
// (see "LICENSE.txt"). If not, terms of the license are available online at
// "http://www.opensource.org/licenses/mit".

  // http-server library.
#include "Error.hpp"
#include "Flags.hpp"
#include "Method.hpp"
#include "Message.hpp"
#include "Request.hpp"
#include "Response.hpp"

  // qt library.
#include <QApplication>
#include <QtCore>

  // standard library.
#include <cstdlib>

  // curent project.
#include "qwsserver.hpp"

int main ( int argc, char ** argv )
{
    QApplication application(argc, argv);
    qws::Server server;
    server.show();
    return application.exec();
}
