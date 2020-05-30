#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_USER_INPUT 2048
#define STDIN 0
#define STDOUT 1

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
#define WCONTINUED 8

#define MAX_DIR_SIZE 8192
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define SYS_OPEN 5
#define OPEN_READ 0
#define OPEN_WRITE 1
#define OPEN_RW 2
#define OPEN_CREATE 64
#define OPEN_TRUNC 512
#define OPEN_APPEND 1024
#define SYS_CLOSE 6
#define SYS_WRITE 4
#define SYS_READ 3
#define SEEK 19
#define SEEK_SET 0
#define SEEK_CURR 1
#define SEEK_END 2
#define GET_DENTS64 141
#define DONT_MATTER 0

void notifyAndExit(char *errMsg, int exitCode)
{
    perror(errMsg);
    exit(exitCode);
}
void debug(int shouldDebug, char *msg)
{
    if (!shouldDebug)
        return;
    fprintf(stderr, msg);
}
void debugWithParam(int shouldDebug, char *msg, int i)
{
    if (!shouldDebug)
        return;
    fprintf(stderr, msg, i);
}


int main(int argc, char **argv)
{
    int inDebugMode = 0;
    int argIdx = 0;
    while (argIdx < argc)
    {
        if (strncmp(argv[argIdx], "-d", 2) == 0)
            inDebugMode = 1;
        argIdx++;
    }

    int myPipe[2];
    int pipeRes = pipe(myPipe);
    if (pipeRes == -1)
        notifyAndExit("faulty pipe\n", 0);
    debug(inDebugMode, "(parent_process>first forking )\n");
    int pid1 = fork();
    if (pid1 < 0)
        notifyAndExit("faulty fork\n", 0);
    else if (pid1 == 0)
    {
        debug(inDebugMode,"(child1>redirecting stdout to the write end of the pipe…)\n" );
        close(STDOUT);
        dup(myPipe[1]);
        close(myPipe[1]);
        char *args[3];
        args[0] = "ls";
        args[1] = "-l";
        args[2] = NULL;
        debug(inDebugMode,"(child1>going to execute cmd: ls -l )\n");
        execvp(args[0], args);
    }
    else if (pid1 > 0)
    {
        debugWithParam(inDebugMode, "(parent_process>created process with id: %d)\n", pid1);
        debug(inDebugMode, "(parent_process>closing the write end of the pipe…)\n");
        close(myPipe[1]);
        debug(inDebugMode, "(parent_process>second forking )\n");
        int pid2 = fork();
        if (pid2 == 0)
        {
            debug(inDebugMode,"(child2>redirecting stdin to the read end of the pipe…)\n"); 
            close(STDIN);
            dup(myPipe[0]);
            close(myPipe[0]);
            char *args[4];
            args[0] = "tail";
            args[1] = "-n";
            args[2] = "2";
            args[3] = NULL;
            debug(inDebugMode,"(child2>going to execute cmd: tail -n 2)\n");
            execvp(args[0], args);
        }
        else if (pid2 > 0)
        {
            debugWithParam(inDebugMode, "(parent_process>created process with id: %d)\n", pid2);
            debug(inDebugMode, "(parent_process>closing the read end of the pipe…)\n");
            close(myPipe[0]);
            debugWithParam(inDebugMode, "(parent_process>waiting for child processes: %d to terminate…)\n", pid1);
            wait(&pid1);
            debugWithParam(inDebugMode, "(parent_process>waiting for child processes: %d to terminate…)\n", pid2);
            wait(&pid2);            
        }
        debug(inDebugMode,"(parent_process>exiting…)\n");
    }
    return 0;
}