#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "stage2.h"

#define MAXLINE 1000
#define MAX_REC_SIZE 50   
#define MAX_PATHS_SIZE 50 

#if defined( _WIN32 )
#pragma warning(disable:4996)
#endif //some neccesery lines


int iscommand(char* word);															 //checking if command
void makeshift(struct record* rek, int* del, int i, int* order_n, int* memory_n);	 //calculating delay
void output(struct record rek[], int end, int order_n, int memory_n);				 //data parsing
void translate_assembler(FILE *file, char* path, int debug_mode, int code_type);	 //translating psa code to msck code
int how_many_lines(FILE* plik);														 //function returns number of lines in file

