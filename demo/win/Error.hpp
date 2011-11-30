#ifndef _win_Error_hpp__
#define _win_Error_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Error.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>
#include <string>

namespace win {

    class Error
    {
        /* nested types. */
    public:
        typedef ::DWORD Code;

        /* data. */
    private:
        Code myCode;

        /* construction. */
    public:
        Error ( Code code ) throw();

        /* methods. */
    public:
        Code code () const throw();
        std::string what () const;
    };

}

    // Use this macro to signal errors.
#define UNCHECKED_WIN32C_ERROR(function,error) \
    throw (win::Error(error));

#endif /* _win_Error_hpp__ */
