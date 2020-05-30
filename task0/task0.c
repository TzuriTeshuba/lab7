#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int debugMode = 0;
int unitSize = 1;
char buffer[100]="";

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*Any additional fields you deem necessary*/
} state;
char *stripNewLine(char* str){
    int i=0;
    while(str[i] != '\n')i++;
    str[i]='\0';
    return str;
}
void flushStdin()
{
    char temp;
    while ((temp != EOF) & (temp != '\n'))
        temp = fgetc(stdin);
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

void quit(state* s)
{
    exit(0);
}
void toggleDebug(state* s)
{
    debugMode = (-1 * debugMode) + 1;
    printf("Debug mode is now %s\n",debugMode? "ON" : "OFF");
}
void setFileName(state* s){
    char fileName[100];
    fgets(fileName,100,stdin);
    stripNewLine(fileName);
    strncpy(s->file_name,fileName,100);//strncpy might malloc??
    if(debugMode) printf("(Debug: set file name to %s)\n",s->file_name);
}

void setUnitSize(state* s){
    char uSize[3];//{'value','\n','\0'}
    fgets(uSize,3,stdin);
    if(uSize[1] != '\n') printf("invalid unit size\n");
    else if((uSize[0] == '1') | (uSize[0] == '2') |(uSize[0] == '4') ){
        s->unit_size= uSize[0]-'0';
        if(debugMode) printf("(Debug: set size to %d)\n",s->unit_size);
    }
    else printf("invalid unit size\n");   
}

struct MenuItem
{
    char *name;
    void (*fun)(state*);
};
const struct MenuItem menu[] = {
    {"Quit", quit},
    {"Toggle Debug", toggleDebug},
    {"Set File Name", setFileName},
    {"Set Unit Size",setUnitSize},
    {NULL, NULL}};


int main(int argc, char **argv)
{
    state* myState = calloc(1,sizeof(state));
    int lowOption = 0;
    int highOption = (sizeof(menu) / sizeof(struct MenuItem)) - 1;
    struct MenuItem menuItem;
    while (1)
    {
        printf("Please choose a function:\n");
        for (int i = 0;; i++)
        {
            menuItem = menu[i];
            if (menuItem.name == NULL)
                break;
            printf("%d) %s\n", i, menuItem.name);
        }
        int choice = fgetc(stdin) - '0';
        flushStdin();
        if ((choice >= lowOption) & (choice < highOption))
            printf("Within bounds\n");
        else
        {
            printf("Not within bounds\n");
            exit(EXIT_SUCCESS);
        }
        menu[choice].fun(myState);
    }
    return 0;
}