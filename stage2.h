#pragma once
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include "structures.h"
#include "stage3.h"


#define MAX_REC_SIZE 50
#if defined( _WIN32 )
#pragma warning(disable:4996)
#endif //some neccesery lines

void compile(int memory_n, int order_n, int debug_mode, int code_type);			//main compile function
int sign_of_operation(int n);													//returns sign of operation
int find_adress(int n, int order_n);											//finding adress of next operation
void translate_machine(FILE* file, char* path, int debug_mode, int code_type);	//translate from msck
void check_lines_len(FILE* file, int *memory_n, int* order_n);					//checking number of memory and order lines
void make_output_file(int order_n, int memory_n);								//making output file