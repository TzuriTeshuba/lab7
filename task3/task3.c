#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h> // Prototype for basename() function
#include <elf.h>

#define HEXA_MODE 0
#define DECI_MODE 1
#define KB(i) ((i)*1<<10)
#define INT sizeof(int)
#define SHORT sizeof(short)
#define BYTE sizeof(char)
char* greeting1 = "hello";
char* greeting2 = "shalom";
char buffer[100] = "abcdef";

typedef struct
{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    /*Any additional fields you deem necessary*/
    char display_mode;
    Elf32_Ehdr* header;
} state;

void freeState(state* s){
    if(s->header != NULL)free(s->header);
    free(s);
}

 

char* format(state* s){
    char* formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n", "%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    return formats[(4*s->display_mode)+s->unit_size-1];
}

/* Reads units from file */
void read_units_to_memory(FILE* input, char* buffer, int count,state* s) {
    fread(buffer, s->unit_size, count, input);    
}

/* Prints the buffer to screen by converting it to text with printf */
void print_units(FILE* output, char* buffer, int count, state* s) {
    char* end = buffer + s->unit_size*count;
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        fprintf(output, format(s), var);
        buffer += s->unit_size;
    }
}

/* Writes buffer to file without converting it to text with write */
void write_units(FILE* output, char* buffer, int count,state* s) {
    fwrite(buffer, s->unit_size, count, output);
}
/*****************************************************************************/

char *stripNewLine(char *str)
{
    int i = 0;
    while (str[i] != '\n')
        i++;
    str[i] = '\0';
    return str;
}
int getHexaInput(int maxStrLen)
{
    int output;
    char str[maxStrLen]; //8 for address, 1 for \n, 1 for \0
    fgets(str, maxStrLen, stdin);
    stripNewLine(str);
    sscanf(str,"%x",&output);
    return output;
}
int getDeciInput(int maxStrLen)
{
    char str[maxStrLen];
    fgets(str, maxStrLen, stdin);
    stripNewLine(str);
    return atoi(str);
}

void flushStdin()
{
    char temp;
    while ((temp != EOF) & (temp != '\n'))
        temp = fgetc(stdin);
}

void quit(state *s)
{
    exit(0);
}
void toggleDebug(state *s)
{
    s->debug_mode = (-1 * s->debug_mode) + 1;
    printf("Debug mode is now %s\n", s->debug_mode ? "ON" : "OFF");
}
void setFileName(state *s)
{
    char fileName[100];
    fgets(fileName, 100, stdin);
    stripNewLine(fileName);
    strncpy(s->file_name, fileName, 100); //strncpy might malloc??
    if (s->debug_mode)
        printf("(DEBUG: set file name to %s)\n", s->file_name);
    //add header now too
    if(s->header ==NULL) s->header = malloc(sizeof(Elf32_Ehdr));
    FILE* file = fopen(s->file_name,"rb");
    fread(s->header,sizeof(Elf32_Ehdr),1,file);
    fclose(file);
}

void setUnitSize(state *s)
{
    char uSize[3]; //{'value','\n','\0'}
    fgets(uSize, 3, stdin);
    if (uSize[1] != '\n')
        printf("invalid unit size\n");
    else if ((uSize[0] == '1') | (uSize[0] == '2') | (uSize[0] == '4'))
    {
        s->unit_size = uSize[0] - '0';
        if (s->debug_mode)
            printf("(DEBUG: set size to %d)\n", s->unit_size);
    }
    else
        printf("invalid unit size\n");
}

void LoadIntoMemory(state *s)
{
    char* buf = buffer;
    if ((s->file_name == NULL) || (!strcmp("", s->file_name)))
    {
        printf("file name not defined or empty. aborting action.\n");
        return;
    }
    FILE *file = fopen(s->file_name, "rb");
    if (file == NULL)
    {
        printf("failed to open file. Aborting action.\n");
        return;
    }
    //get location
    printf("provide location (in Hexa)\n");
    int location = getHexaInput(10); //8 for address, 1 for \n, 1 for \0
    //get length
    printf("provide length up to 10,000 (in Deci)\n");
    int length = getDeciInput(7); //5 for num up to 10000, 1 for \n, 1 for \0
    if (s->debug_mode)
        printf("(DEBUG: name: %s\tlocation: %x\tlength: %d)\n", s->file_name, location, length);
    //copy length*unitSize bytes from fileName starting at 'location' into memory
    fseek(file, location, SEEK_SET);
    fread(s->mem_buf, s->unit_size, length, file);
    fclose(file);
}

void toggleDisplayMode(state *s)
{
    s->display_mode = (-1 * s->display_mode) + 1;
    printf("Display flag mode is now %s representation\n", s->display_mode ? "ON, hexadecimal" : "OFF, decimal");
}

//incomplete - deal with deal with decimal representation
void memoryDisplay(state *s)
{
    printf("provide how many units to present (in decimal form):\n");
    int numUnits = getDeciInput(7); //max 10,000 units
    printf("provide starting address (in hexadecimal form):\n");
    int address = getHexaInput(10);
    //display u units of size unit size starting at address
    char *info = (address == 0 ? s->mem_buf : (char *)address);
    print_units(stdout,info,numUnits,s);
}

void saveIntoFile(state *s)
{
    char* buf = buffer;
    printf("provide source address(Hexa):\n");
    int sourceAdrs = getHexaInput(10);
    char* src = sourceAdrs==0? s->mem_buf : (char*)sourceAdrs;
    printf("provide target file offset (Hexa):\n");
    int fileOffset = getHexaInput(10);
    printf("provide number of units (Decimal):\n");
    int numUnits = getDeciInput(8);

    FILE* file = fopen(s->file_name,"rb+");
    if(!file){
        printf("failed to open file. aborting action\n");
        return;
    }
    if(s->debug_mode)printf("(DEBUG: file %s was opened for writing)\n",s->file_name);
    int seekRes = fseek(file,fileOffset,SEEK_SET);
    if(s->debug_mode)printf("(DEBUG: seeking to offset 0x%x returned %d)\n",fileOffset,seekRes);
    int writeRes = fwrite(src,s->unit_size,numUnits,file);
    if(s->debug_mode)printf("(DEBUG: wrote %d units to %s)\n",writeRes,s->file_name);
    int closeRes = fclose(file);
    if(s->debug_mode)printf("(DEBUG: closing file %s returned %d)\n",s->file_name,closeRes);
}

//incomplete and bad!
void memoryModify(state* s){
    char* buf = buffer;
    printf("buffer at: %x\n",buf);
    unsigned int* valp = malloc(sizeof(int));
    printf("provide location (Hexa):\n");
    int location = getHexaInput(10);
    printf("provide value (Hexa):\n");
    int value = getHexaInput((2*s->unit_size)+2);
    *valp = value;
    if(s->debug_mode)printf("(DEBUG: received location: 0x%x and value: 0x%x)\n",location,value);
    char *mem = (location == 0 ? s->mem_buf : (char *)location);
    unsigned char* values = valp;
    int i =0;
    printf("mem   : %s\n",mem);
    printf("values: %s\n",values);
    for(i=0; i<s->unit_size; i++){
        mem[i]=values[s->unit_size-i-1];
    }
    print_units(stdout,mem,1,s);
    free(valp);
}

void printEntryPoint(state* s){
    Elf32_Ehdr* hdr = malloc(sizeof(Elf32_Ehdr));
    FILE* file = fopen(s->file_name,"rb");
    fread(hdr,sizeof(Elf32_Ehdr),1,file);
    printf("entry at %x\n",(int)hdr->e_entry);
    fclose(file);
    free(hdr);
}

void printBuffer(state* s){
    char* b = buffer;
    printf("%s\n",buffer);
}

struct MenuItem
{
    char *name;
    void (*fun)(state *);
};
const struct MenuItem menu[] = {
    {"Toggle Debug", toggleDebug},
    {"Set File Name", setFileName},
    {"Set Unit Size", setUnitSize},
    {"Load Into Memory", LoadIntoMemory},
    {"Toggle Display Mode", toggleDisplayMode},
    {"Memory Display", memoryDisplay},
    {"Save Into File", saveIntoFile},
    {"Memory Modify", memoryModify},
    {"Print Buffer",printBuffer},
    {"Quit", quit},
    {NULL, NULL}};

void init(state *s){
    strcpy(s->file_name,"deep");
    s->unit_size=4;
}
void printFileBytes(char* fileName){
    FILE* file = fopen(fileName,"rb");
    int i=0;
    while((i = fgetc(file))!= EOF){
        printf("%02x ",i);
    }
    fclose(file);
}
//entry at byte 24
int main(int argc, char **argv)
{
    printFileBytes("abc");
    state *myState = calloc(1, sizeof(state));
    myState->unit_size = 1;
    //init(myState);
    //printEntryPoint(myState);
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
        if (!(choice >= lowOption) | !(choice < highOption))
            printf("Not within bounds\n");
        else
            menu[choice].fun(myState);
    }
    freeState(myState);
    return 0;
}