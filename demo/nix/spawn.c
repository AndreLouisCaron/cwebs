/*!
 * @file spawn.c
 * @author Andre Caron (andre.l.caron@gmail.com)
 *
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
