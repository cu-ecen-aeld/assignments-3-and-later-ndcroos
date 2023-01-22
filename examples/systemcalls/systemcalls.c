#include "systemcalls.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    // system(cmd) passes the command name or program name specified by command
    // to the host environemnt to be executed and returns after the command has been completed.
    // Notice that you can use system() with a command that has redirection.
    int result = system(cmd);
    if (result == -1) {
        return false;
    }
    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    int pid = fork();
    // creation of a child process was unsuccesful
    if (pid == -1) {
      perror("fork");
      return false;
    }
    // returned to the newly created child process
    if (pid == 0) {
      execv(command[0], command);
      perror("execv");
      exit(1); // If execv runs correctly, will not reach here
    }

    int w_status;
    // 
    if (wait(&w_status) == -1) {
      return false;
    }
    // From sys/wait.h
    // Check return value of wait() if it is not an error
    // Check process has terminated with code code 0 (succes)
    if (WIFEXITED(w_status) && WEXITSTATUS(w_status) == 0) {
      return true;
    }

    // From stdarg.h
    // The C library macro va_end() ends the processing of the variable argument list
    va_end(args);
    return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a reference,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) {
      perror("open");
      return false;
    }
    int pid = fork();
    // Check for error
    if (pid == -1) {
      perror("fork");
      close(fd);
      return false;
    } else if (pid == 0) {
      // From unistd.h
      // Duplicate a file descriptor. Allocate a new file descriptor
      if (dup2(fd, 1) < 0) {
        perror("dup2");
        close(fd);
        return false;
      }
      // Replaces the current process image with a new process image, 
      // specified by command[0] (file)
      execvp(command[0], command);
      perror("execvp");
      close(fd); // not supposed to reach here if execv is successful
      exit(1);
    }

    int w_status;
    if (wait(&w_status) == -1) {
      close(fd);
      return false;
    }

    if (WIFEXITED(w_status) && WEXITSTATUS(w_status) == 0) {
      close(fd);
      return true;
    }

    close(fd);
    va_end(args);
    return false;
}
