#ifndef _win_Thread_hpp__
#define _win_Thread_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Thread.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win {

        /*!
         * @brief Entity within a process that can be scheduled for execution.
         *
         * All threads of a process share its virtual address space and system
         * resources. In addition, each thread maintains exception handlers, a
         * scheduling priority, thread local storage, a unique thread
         * identifier, and a set of structures the system will use to save the
         * thread context until it is scheduled. The thread context includes
         * the thread's set of machine registers, the kernel stack, a thread
         * environment block, and a user stack in the address space of the
         * thread's process.
         *
         * Threads can also have their own security context, which can be used
         * for impersonating clients.
         */
    class Thread
    {
        /* nested types. */
    public:
        typedef ::HANDLE Handle;

            /*!
             * @brief Integer value uniquely identifying a running thread.
             *
             * If you need to identify a thread through inter-process
             * communication, sending this value is more convenient than sending
             * the handle (which is not valid in the other process).
             */
        typedef ::DWORD Identifier;

            /*!
             * @brief Value returned by a thread to indicate status.
             */
        typedef ::DWORD Status;

            /*!
             * @brief Required function signature.
             * @see Parameter
             * @see Status
             * @see adapt
             */
        typedef Status(__stdcall*Function)(void*);

            /*!
             * @brief Converts functions to enfore the required signature.
             *
             * The required function signature is meant to be generally useful,
             * but may not suit you. In particular, if you do not require using
             * the context parameter or status code, you need to provide your
             * function with an unsued argument, and/or fixed status code
             * (typically 0). This adapter allows you to use functions with no
             * argument and/or no status code, yet allowing you to use both.
             *
             * Template parameter @c R must be @c void or @c ::DWORD.
             * Template parameter @c A must be @c void or @c void*.
             *
             * @note Although this only binds with a function at compile-time,
             *   this class is designed to cast to @c Function, so you can
             *   easily resort to casting all candiate functions to @c Function)
             *   and select one of those at run-time.
             *
             * @see Function
             */
        template<typename R, typename A, R(*F)(A)>
        struct adapt;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        explicit Thread ( Function function, void * context = 0 );

    private:
        Thread ( const Thread& );

    public:
        ~Thread ();

        /* methods. */
    public:
        Handle handle () const;

            /*!
             * @brief Obtains the thread identifier.
             *
             * @return Theead identifier.
             * @see ExistingThread
             */
        Identifier identifier () const;

        /*!
         * @brief Wait for the thread to complete execution.
         */
        void join () const;

        /* operators. */
    private:
        Thread& operator= ( const Thread& );
    };

        // "Full-fledged" case.
    template<Thread::Status(*F)(void*)>
    struct Thread::adapt<Thread::Status,void*,F>
    {
        operator Thread::Function () const {
            return (result);
        }
    private:
            // Actual implementation.
        static ::DWORD __stdcall result ( ::LPVOID p )
        {
            return (F(p));
        }
    };

        // For threads that "never fail" and require no context.
    template<void(*F)()>
    struct Thread::adapt<void,void,F>
    {
        operator Thread::Function () const {
            return (result);
        }
    private:
            // Actual implementation.
        static ::DWORD __stdcall result ( ::LPVOID p )
        {
            F(); return (0);
        }
    };

        // For threads that "never fail".
    template<void(*F)(void*)>
    struct Thread::adapt<void,void*,F>
    {
        operator Thread::Function () const {
            return (result);
        }
    private:
            // Actual implementation.
        static ::DWORD __stdcall result ( ::LPVOID p )
        {
            F(p); return (0);
        }
    };

        // For threads requiring no context but wishing to report errors.
    template<Thread::Status(*F)()>
    struct Thread::adapt<Thread::Status,void,F>
    {
        operator Thread::Function () const {
            return (result);
        }
    private:
            // Actual implementation.
        static ::DWORD __stdcall Thread::result ( ::LPVOID )
        {
            return (F());
        }
    };

}

#endif /* _win_Thread_hpp__ */
