#ifndef _spawn_h__
#define _spawn_h__

/*!
 * @file spawn.h
 * @author Andre Caron (andre.l.caron@gmail.com)
 *
 * @brief Slave process spawning helpers for POSIX systems.
 */

#ifdef __cplusplus
#extern "C" {
#endif

/*!
 * @brief Create pipes, @c fork() and @c exec().
 * @param path Path to program that will be spawned.
 * @param argv List of command-line arguments to pass to the process.
 * @param envp List of environment variables to pass to the process.
 * @param pipes File descriptors for the pair of pipes used to communicate
 *  with the new process.
 * @return On success 0, on failure -1.
 *
 * This is a shorthand to launch a slave process with I/O redirected through
 * pipes for direct control.
 */
int spawnve
  ( const char * path, char *const argv[], char *const envp[], int pipes[2] );

#ifdef __cplusplus
}
#endif

#endif /* _spawn_h__ */
