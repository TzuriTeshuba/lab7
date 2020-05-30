#include "util.h"

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
#define GET_DENTS 141
#define DONT_MATTER 0

extern int system_call();

void print(char *str, int file)
{
    system_call(SYS_WRITE, file, str, strlen(str));
}
void println(char *str, int file)
{
    system_call(SYS_WRITE, file, str, strlen(str));
    system_call(SYS_WRITE, file, "\n", 1);
}
void printMany(char *str[], int numStrings, int file)
{
    int i = 0;
    for (i = 0; i < numStrings; i++)
    {
        print(str[i], file);
    }
    system_call(SYS_WRITE, file, "\n", 1);
}

typedef struct ent
{
    int inode;
    int offset;
    short len;
    char buf[1];
} ent;

int main(int argc, char **argv)
{
    char buffer[MAX_DIR_SIZE];
    int fd = 0;
    ent *entp = (ent*)buffer;
    int isInDebugMode = 0;

    /*check for debug mode*/
    int progArgIndex;
    for (progArgIndex = 1; progArgIndex < argc; progArgIndex++)
    {
        if (strncmp(argv[progArgIndex], "-d", 2) == 0)
            isInDebugMode = 1;
    }

    println("Top of the charts\nBack in their hearts\nFlame2 strikes again - whoa", STDOUT);
    fd = system_call(SYS_OPEN, ".", OPEN_READ, DONT_MATTER);
    if (isInDebugMode)
    {
        char *toPrint[] = {"opening current directory returned ", itoa(fd)};
        printMany(toPrint, 2, STDERR);
    }
    /*count = system_call(GET_DENTS,fd,buffer,100);*/
    int dirSize = system_call(GET_DENTS, fd, buffer, MAX_DIR_SIZE);
    if (isInDebugMode)
    {
        char *toPrint[] = {"calling get_dents returned ", itoa(dirSize)};
        printMany(toPrint, 2, STDERR);
    }
    println(itoa(dirSize), STDOUT);

    int offset = 0;
    while (offset < dirSize)
    {
        entp = (ent*)(buffer + offset);
        char *toPrint[] = {"inode: ", itoa(entp->inode), "\toffset: ", itoa(entp->offset), "\tsize: ", itoa(entp->len), "\tname: ", entp->buf};
        printMany(toPrint, 8, STDOUT);
        offset = offset + entp->len;
    }
    int closeRes = system_call(SYS_CLOSE, fd);
    if (isInDebugMode)
    {
        char *toPrint[] = {"closing current directory returned ", itoa(closeRes)};
        printMany(toPrint, 2, STDERR);
    }
    println(itoa(closeRes), STDOUT);
    return 0;
}