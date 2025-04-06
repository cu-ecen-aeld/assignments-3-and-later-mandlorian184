#include "systemcalls.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    int res = system(cmd);
    printf("do_system() called with CMD<%s> and return <%d>\n",cmd,res);
    if(res == -1)
        return false ;
    else 
        return (WIFEXITED(res) && WEXITSTATUS(res) == 0);
    
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
    if((command[0][0]!='/') | ((count > 2) & (command[count-1][0]!='/')))
    {
        printf("comand count -1 is and 0 char is %s, %c\n",command[count-1],command[count-1][0]);
        return false;
    }
    fflush(stdout);
    pid_t pid = fork(); // Create a new process
    if (pid == -1) {
        // Fork failed
        perror("Fork failed");
        return false;
    } else if (pid == 0) {
        // Child process
        const char* path = command[0];
        execv(path, command); // Execute the command  ---------> ?
        // If execv returns, it must have failed
        perror("execv failed");
        return false;
    } else {
        // Parent process
        int status;
        wait(&status); // Wait for the child process to finish
        // Check if the child process terminated normally and returned 0
        if (WIFEXITED(status)) {
            printf("Command executed with exit status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Command did not terminate normally.\n");
            return false;
        }
    }

    va_end(args);
    printf("do_exec()returned successfully \n");
    return true;
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
    command[count] = command[count];
    pid_t childpid;
    int status;
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) {
     perror("open"); 
     return false;    
    }
    fflush(stdout);
    switch (childpid = fork())
    { 
    case -1:
        perror("fork"); 
        return false;    
    case 0:
        if (dup2(fd, 1) < 0) { 
            perror("dup2"); 
            return false;    
        }
        close(fd);
        execv(command[0], command); 
        perror("execvp"); 
        return false;
    default:
        close(fd);
        /* do whatever the parent wants to do. */
    }

    if (waitpid (childpid, &status, 0) == -1)
    {
        return -1;
    }
    else if (WIFEXITED (status)){
        return WEXITSTATUS (status);
    }

    va_end(args);
    printf("do_exec()returned successfully \n");
    return true;
}
