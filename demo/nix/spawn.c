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
// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)

/*!
 * @file spawn.c
 * @brief Slave process spawning helpers for POSIX systems.
 */

#include "spawn.h"

#include <unistd.h>

// External helper for assigning pipes to stdin and stdout.
int remap_pipe_stdin_stdout ( int rpipe, int wpipe );

int spawnve
  ( const char * path, char *const argv[], char *const envp[], int pipes[2] )
{
    int pushpipes[2]; // master -> slave
    int pullpipes[2]; // master <- slave
    int status;
    status = pipe(pushpipes);
    if (status < 0) {
        return (status);
    } 
    status = pipe(pullpipes);
    if (status < 0) {
        return (status);
    }
    pid_t result = fork();
    if (result < 0) {
        return (-1);
    }
    if (result == 0)
    {
        // forget about unused file descriptors.
        close(pushpipes[1]);
        close(pullpipes[0]);
    
        // communicate with master process using the pipes.
        remap_pipe_stdin_stdout(pushpipes[0], pullpipes[1]);
        
        // ready to continue.
        status = execve(path, argv, envp);
        if (status < 0) {
            return (status);
        }
    }
    else
    {
        // forget about unused file descriptors.
        pipes[0] = pullpipes[0]; close(pullpipes[1]);
        pipes[1] = pushpipes[1]; close(pushpipes[0]);
    }
    
    // ready to continue.
    return (0);
}
