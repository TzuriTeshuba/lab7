#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "../task2C/menu_map.c"

const int LOWER_BOUND = 0x20;
const int UPPER_BOUND = 0x7E;

//const char* arrays[6];

void flushStdin()
{
    char temp;
    while ((temp != EOF) & (temp != '\n'))
    {
        //printf("%c",temp);
        temp = fgetc(stdin);
    }
    //printf("\nexiting flush\n");
}
/* Gets a char c and returns its encrypted form by adding 3 to its value. 
          If c is not between 0x20 and 0x7E it is returned unchanged */
char encrypt(char c)
{
    if ((c > LOWER_BOUND) & (c < UPPER_BOUND))
        return c + 3;
    else
        return c;
}

/* Gets a char c and returns its decrypted form by reducing 3 to its value. 
            If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c)
{
    if ((c > LOWER_BOUND) & (c < UPPER_BOUND))
        return c - 3;
    else
        return c;
}

/* dprt prints the value of c in a decimal representation followed by a 
new line, and returns c unchanged. */
char dprt(char c)
{
    printf("%d\n", (int)c);
    return c;
}

/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
the value of c unchanged. */
char cprt(char c)
{
    if ((c > LOWER_BOUND) & (c < UPPER_BOUND))
        printf("%c\n", c);
    else
        printf(".\n");

    return c;
}

/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c)
{
    char output = fgetc(stdin);
    if(output=='\n') output ='\0';
    return output;
}

char censor(char c)
{
    if (c == '!')
        return '.';
    else
        return c;
}

/* Gets a char c,  and if the char is 'q' ,
 ends the program with exit code 0. Otherwise returns c. */
char quit(char c)
{
    if (c == 'q')
        exit(EXIT_SUCCESS);
    else
        return c;
}

char *map(char *array, int array_length, char (*f)(char))
{
    char *mapped_array = (char *)(malloc(array_length * sizeof(char)));
    /* TODO: Complete during task 2.a */
    for (int i = 0; i < array_length; i++)
    {
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}
struct fun_desc
{
    char *name;
    char (*fun)(char);
};

const int carrayLength = 5;
const struct fun_desc menu[] = {
    {"Censor", censor},
    {"Encrypt", encrypt},
    {"Decrypt", decrypt},
    {"Print dec", dprt},
    {"Print string", cprt},
    {"Get string", my_get},
    {"Quit", quit},
    {NULL, NULL}};

int main(int argc, char **argv)
{
    //atexit(freeResources);
    char carray[5];
    strncpy(carray, "", carrayLength);
    int lowOption = 0;
    int highOption = (sizeof(menu) / sizeof(struct fun_desc)) - 1;
    struct fun_desc fun;
    while (1)
    {
        printf("Please choose a function:\n");
        for (int i = 0;; i++)
        {
            fun = menu[i];
            if (fun.name == NULL)
                break;
            printf("%d) %s\n", i, fun.name);
        }

        char choice = fgetc(stdin);
        flushStdin();
        if ((choice >= lowOption + '0') & (choice < highOption + '0'))
            printf("Within bounds\n");
        else
        {
            printf("Not within bounds\n");
            exit(EXIT_SUCCESS);
        }
        int choiceInt = choice - '0';
        char *temp;
        temp = map(carray, carrayLength, menu[choiceInt].fun);
        strncpy(carray, temp, carrayLength);
        free(temp);
    }
    return 0;
}