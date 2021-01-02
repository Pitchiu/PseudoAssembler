#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stage1.h"


#define MAX_PATHS_SIZE 500   //max path len

#if defined( _WIN32 )
#pragma warning(disable:4996)
#endif //some neccesery lines

int main(int argc, char** argv)
{
    int debug_mode = 0; //step by step mode
    int code_type = 0;   //assembler(0) or machine(1) code
    int i;
    char path[MAX_PATHS_SIZE];  //path
    FILE* file = NULL;          //assembler file
    //obs³u¿enie otwarcia pliku
    if (argc == 1)
    {
        printf("Prosze podac sciezke wzgledna pliku: ");
        scanf("%s", path);
    }
    else if (argc == 2)
    {
        strcpy(path, argv[1]);
    }
    else if (argc == 3)
    {
        strcpy(path, argv[1]);
        if (strcmp(argv[2], "msck_code") == 0) { code_type = 1; }
    }
    else if (argc == 4)
    {
        strcpy(path, argv[1]);
        if (strcmp(argv[2], "msck_code") == 0) { code_type = 1; }
        if (strcmp(argv[3], "debug") == 0) { debug_mode = 1; }
    }


    while ((file = fopen(path, "r")) == NULL)
    {
        perror("Nie mozna otworzyc pliku do wczytania");
        printf("Podaj sciezke jeszcze raz: ");
        scanf("%s", path);
    }
    if (code_type == 1)
    {
        translate_machine(file, path, debug_mode, code_type);
    }
    else
    {
        translate_assembler(file, path, debug_mode, code_type);
    }

    return 0;
}





