#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const int LOWER_BOUND = 0x20;
const int UPPER_BOUND = 0x7E;
//const char* arrays[6];

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
  /*
  char tmp = output;
  while((tmp != '\n') && (tmp != EOF)){
    tmp = fgetc(stdin);
  }
  */
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
/*
void onQuit(char* toRelease){
  if(toRelease != NULL)
    free(toRelease);
}
*/
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

int main(int argc, char **argv)
{
  /* TODO: Test your code */
  int base_len = 5;
  char arr1[base_len];

  char *arr2 = map(arr1, base_len, my_get);
  char *arr3 = map(arr2, base_len, encrypt);
  char *arr4 = map(arr3, base_len, dprt);
  char *arr5 = map(arr4, base_len, decrypt);
  char *arr6 = map(arr5, base_len, cprt);
  //int value = atexit(&onQuit(arr2));
  free(arr2);
  free(arr3);
  free(arr4);
  free(arr5);
  free(arr6);

  return 0;
}