#ifndef my_h
#define my_h

#include <stdlib.h>
#define MAX_SIZE 10

void strToAr(char* string, char *array[MAX_SIZE],int *size);
int komutlar(char* array[MAX_SIZE],int size,int flg);
void signal_callback_handler(int signum);
char* prompt(char * komut, size_t *bufsize);

#endif
