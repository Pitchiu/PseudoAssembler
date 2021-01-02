#pragma once
#include "structures.h"
#include <curses.h>
#include <string.h>
#include <stdio.h>


#if defined( _WIN32 )
#pragma warning(disable:4996)
#endif //some neccesery lines

void console_print(int order_n, int memory_n, int registers[], int sign, int register_active, int memory_active, int order_active, int next_operation, int code_type, int assembler_type);
void print_boxes(int order_n, int memory_n);
void print_programstate(int next_operation, int order_n, int memory_n, int sign);
void print_assemblerlist(int order_n, int order_active, int show_assembler_type);
void print_orderlist(int order_n, int order_activ);
void print_registerlist(int register_active, int *registers);
void print_memorylist(int memory_n, int memory_active);
void help_print();
void construct_assembler_line(int n);


