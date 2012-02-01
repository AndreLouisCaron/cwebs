#ifndef _options_hpp__
#define _options_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/options.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 *
 * @brief Simple command-line parser helpers.
 */

#include <cstring>
#include <sstream>
#include <stdexcept>

namespace {

    int findarg ( int argc, char ** argv, const char * name )
    {
        int i = 0;
        while ((i < argc) && (std::strcmp(argv[i],name)!=0)) {
            ++i;
        }
        return (i);
    }

    bool hasarg ( int argc, char ** argv, const char * name )
    {
        return (findarg(argc, argv, name) < argc);
    }

    template<typename T>
    T getarg ( int argc, char ** argv, const char * name, const T& fallback )
    {
        T value = fallback;
        const int match = findarg(argc, argv, name);
        if (match == (argc-1))
        {
            std::cerr
                << "Missing value for argument '" << name << "'."
                << std::endl;
            throw (std::invalid_argument(name));
        }
        if (match < (argc-1))
        {
            std::istringstream input(argv[match+1]);
            if (!(input >> value)) {
                throw (std::invalid_argument(name));
            }
        }
        return (value);
    }

}

#endif /* _options_hpp__ */
