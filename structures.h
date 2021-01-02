#pragma once
#include <stdlib.h>

struct record   //strust used in pseudoassembler data parsing
{
    char etykieta[10];  
    char rozkaz[3];     
    int value1;        
    int value2;
    int value3;     
    char typefield[16]; 
    int delay;         
};
typedef struct record record;
record* rec_tab;

struct orders   //struct for order lines
{
    char label[10];
    int delay;      //delay of adress
    int value;
    int own_delay;  //their own delay
};
typedef struct orders orders;
orders* code;

struct reservation      //struct for memory lines
{
    char label[10];
    int number;
    int delay;
    int ifused; //0-just reservation 1-specific number
};
typedef struct reservation reservation;
reservation* memory;

struct assembler_orders     //struct with the whole line of code in psa
{
    char line[40];
};
typedef struct assembler_orders assembler_orders;
assembler_orders* assembler_code;