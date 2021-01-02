#include "stage1.h"


void translate_assembler(FILE* file, char* path, int debug_mode, int code_type)
{
    char linia[MAX_REC_SIZE];             //linia do wczytania z kodu assemblera
    char* token;                //token=
    int i = 0;                  //iterator
    int shift = 0;              //przesuniecie
    int val_counter = 0;        //zmienna pomocnicza
    int end = 0;                //zmienna do iloscii linii asemblera
    int lines = 0;
    int memory_n=0;             //number of memory reservation unit
    int order_n=0;              //number of order lines
    int iforders = 0;        //set 1 when program start read orders

    lines = how_many_lines(file);
    //struct memory allocations
    rec_tab = (record*)calloc(lines, sizeof(record));
    assembler_code = (assembler_orders*)calloc(lines, sizeof(assembler_orders));

    file = fopen(path, "r");

    while (fgets(linia, MAX_REC_SIZE, file) != NULL)
    {
        if (iforders > 0)
        {
            strncpy(assembler_code[iforders - 1].line, linia, 25);
            iforders++;
        }
        token = strtok(linia, " ,()*\t");
        if (strcmp(token, "\n") == 0)       //checking whether line is empty
        {
            iforders = 1;
            strcpy(rec_tab[i].rozkaz, "");
            i++;
            shift = 0;
            continue;
        }

        if (token[0] == '/' && token[1] == '/')     //checking whether line is comment
        {
            continue;
        }


        if (iscommand(token) == 0)       //checking if first token is a command
        {
            strcpy(rec_tab[i].etykieta, "");
            strcpy(rec_tab[i].rozkaz, token);
        }
        else
        {
            strcpy(rec_tab[i].etykieta, token);
            token = strtok(NULL, " ,\n()*\t");
            strcpy(rec_tab[i].rozkaz, token);
        }

        token = strtok(NULL, " ,\n()*\t");    //3 zn

        val_counter = 0;
        while (token != NULL && token[0] != '/')
        {

            if ((isdigit(token[0]) != 0) || (token[0] == '-' && (isdigit(token[1])) != 0))
            {
                if (val_counter == 0)
                {
                    rec_tab[i].value1 = atoi(token);
                    val_counter++;
                }
                else if (val_counter == 1)
                {
                    rec_tab[i].value2 = atoi(token);
                    val_counter++;
                }
                else if (val_counter == 2)
                {
                    rec_tab[i].value3 = rec_tab[i].value2;
                    rec_tab[i].value2 = atoi(token);
                }
            }
            else
            {
                strcpy(rec_tab[i].typefield, token);
                val_counter++;
            }
            token = strtok(NULL, " ,\n()*\t");
        }

        if (rec_tab[i].value1 == NULL && (strcmp(rec_tab[i].rozkaz, "DC") == 0 || strcmp(rec_tab[i].rozkaz, "DS") == 0))
        {
            rec_tab[i].value1 = 1;
        }

        makeshift(rec_tab, &shift, i, &order_n, &memory_n);

        i++;
    }
    end = i;

    code = (orders*)calloc(order_n, sizeof(orders));
    memory = (reservation*)calloc(memory_n, sizeof(reservation));

    output(rec_tab, end, order_n, memory_n);
    fclose(file);
    compile(memory_n, order_n, debug_mode, code_type);
}

int iscommand(char* word)
{
    if (strcmp(word, "AR") == 0)
        return 0;
    else if (strcmp(word, "A") == 0)
        return 0;
    else if (strcmp(word, "SR") == 0)
        return 0;
    else if (strcmp(word, "S") == 0)
        return 0;
    else if (strcmp(word, "MR") == 0)
        return 0;
    else if (strcmp(word, "M") == 0)
        return 0;
    else if (strcmp(word, "DR") == 0)
        return 0;
    else if (strcmp(word, "D") == 0)
        return 0;
    else if (strcmp(word, "CR") == 0)
        return 0;
    else if (strcmp(word, "C") == 0)
        return 0;
    else if (strcmp(word, "J") == 0)
        return 0;
    else if (strcmp(word, "JZ") == 0)
        return 0;
    else if (strcmp(word, "JN") == 0)
        return 0;
    else if (strcmp(word, "JP") == 0)
        return 0;
    else if (strcmp(word, "L") == 0)
        return 0;
    else if (strcmp(word, "LR") == 0)
        return 0;
    else if (strcmp(word, "LA") == 0)
        return 0;
    else if (strcmp(word, "ST") == 0)
        return 0;
    else
        return 1;
}

void output(struct record rek[], int end, int order_n, int memory_n)
{
    int j = 0;
    int i = 0;
    int x = 0;
    char buffor[16];     //code buffor
    char delbuff[16];    //delay buffor

    for (j = 0; j < memory_n; i++)
    {
        //dyrektywy pamieci
        strcpy(memory[j].label, rek[i].etykieta);
        if (strcmp(rek[i].rozkaz, "DC") == 0)
        {
            for (x = 0; x < rek[i].value1; x++)
            {
                memory[j].delay = (rek[i].delay + x * 4);
                sprintf(buffor, "%02X%02X%02X%02X", rek[i].value2 >> 24 & 0xFF, rek[i].value2 >> 16 & 0xFF, rek[i].value2 >> 8 & 0xFF, rek[i].value2 >> 0 & 0xFF);
                sscanf(buffor, "%X", &memory[j].number);
                memory[j].ifused = 1;
                j++;
            }
        }
        else if (strcmp(rek[i].rozkaz, "DS") == 0)
        {
            for (x = 0; x < rek[i].value1; x++)
            {
                memory[j].delay = (rek[i].delay + x * 4);
                memory[j].number = 0;
                memory[j].ifused = 0;
                j++;
            }
        }
    }
       
        i++;

    for (j = 0; j < order_n; j++, i++)
{
        code[j].own_delay = rek[i].delay;
        strcpy(code[j].label, rek[i].etykieta);

        // rejestr-komórka

        if (strcmp(rek[i].rozkaz, "A") == 0)
        {
            if (strcmp(rek[i].typefield, "") == 0)
            {
                sprintf(buffor, "%s%X%X", "D1", rek[i].value1, rek[i].value2);
                sprintf(delbuff, "%02X%02X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
            }
            else
            {
                for (x = 0; x < end; x++)
                    if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                    {
                        sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                        sprintf(buffor,"%s%X%s", "D1", rek[i].value1, "F");
                        break;
                    }
            }
            sscanf(buffor, "%X", &code[j].value);
            sscanf(delbuff, "%X", &code[j].delay);
        }

        else if (strcmp(rek[i].rozkaz, "S") == 0)
        {
            if (strcmp(rek[i].typefield, "") == 0)
            {
                sprintf(buffor, "%s%X%X", "D3", rek[i].value1, rek[i].value2);
                sprintf(delbuff, "%02X%02X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
            }
            else
            {
                for (x = 0; x < end; x++)
                    if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                    {
                        sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                        sprintf(buffor, "%s%X%s", "D3", rek[i].value1, "F");
                        break;
                    }
            }
            sscanf(buffor, "%X", &code[j].value);
            sscanf(delbuff, "%X", &code[j].delay);
        }

        else if (strcmp(rek[i].rozkaz, "M") == 0)
        {
            if (strcmp(rek[i].typefield, "") == 0)
            {
                sprintf(buffor, "%s%X%X", "D5", rek[i].value1, rek[i].value2);
                sprintf(delbuff, "%X%X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
            }
            else
            {
                for (x = 0; x < end; x++)
                    if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                    {
                        sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                        sprintf(buffor, "%s%X%s", "D5", rek[i].value1, "F");
                        break;
                    }
            }
            sscanf(buffor, "%X", &code[j].value);
            sscanf(delbuff, "%X", &code[j].delay);
        }

        else if (strcmp(rek[i].rozkaz, "D") == 0)
        {
            if (strcmp(rek[i].typefield, "") == 0)
            {
                sprintf(buffor, "%s%X%X", "D7", rek[i].value1, rek[i].value2);
                sprintf(delbuff, "%02X%02X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
            }
            else
            {
                for (x = 0; x < end; x++)
                    if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                    {
                        sprintf(delbuff, "%X%X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                        sprintf(buffor, "%s%X%s", "D7", rek[i].value1, "F");
                        break;
                    }
            }
            sscanf(buffor, "%X", &code[j].value);
            sscanf(delbuff, "%X", &code[j].delay);
        }

        else if (strcmp(rek[i].rozkaz, "C") == 0)
        {
             if (strcmp(rek[i].typefield, "") == 0)
            {
                sprintf(buffor, "%s%X%X", "D9", rek[i].value1, rek[i].value2);
                sprintf(delbuff, "%02X%02X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
            }
            else
            {
                for (x = 0; x < end; x++)
                    if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                    {
                        sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                        sprintf(buffor, "%s%X%s", "D9", rek[i].value1, "F");
                        break;
                    }
            }
            sscanf(buffor, "%X", &code[j].value);
            sscanf(delbuff, "%X", &code[j].delay);
        }

        else if (strcmp(rek[i].rozkaz, "L") == 0)
        {
        if (strcmp(rek[i].typefield, "") == 0)
        {
            sprintf(buffor, "%s%X%X", "F0", rek[i].value1, rek[i].value2);
            sprintf(delbuff, "%02X%02X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
        }
        else
        {
            for (x = 0; x < end; x++)
                if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                {
                    sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                    sprintf(buffor, "%s%X%s", "F0", rek[i].value1, "F");
                    break;
                }
        }
        sscanf(buffor, "%X", &code[j].value);
        sscanf(delbuff, "%X", &code[j].delay);
        }

        else if (strcmp(rek[i].rozkaz, "LA") == 0)
        {
        if (strcmp(rek[i].typefield, "") == 0)
        {
            sprintf(buffor, "%s%X%X", "F2", rek[i].value1, rek[i].value2);
            sprintf(delbuff, "%02X%02X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
        }
        else
        {
            for (x = 0; x < end; x++)
                if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                {
                    sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                    sprintf(buffor, "%s%X%s", "F2", rek[i].value1, "F");
                    break;
                }
        }
        sscanf(buffor, "%X", &code[j].value);
        sscanf(delbuff, "%X", &code[j].delay);
        }

        else if (strcmp(rek[i].rozkaz, "ST") == 0)
        {
        if (strcmp(rek[i].typefield, "") == 0)
        {
            sprintf(buffor, "%s%X%X", "F3", rek[i].value1, rek[i].value2);
            sprintf(delbuff, "%02X%02X", rek[i].value3 >> 8 & 0xFF, rek[i].value3 >> 0 & 0xFF);
        }
        else
        {
            for (x = 0; x < end; x++)
                if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                {
                    sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                    sprintf(buffor, "%s%X%s", "F3", rek[i].value1, "F");
                    break;
                }
        }
        sscanf(buffor, "%X", &code[j].value);
        sscanf(delbuff, "%X", &code[j].delay);
        }

        //rejestrowe

        else if (strcmp(rek[i].rozkaz, "AR") == 0)
        {
            sprintf(buffor, "%s%X%X", "10", rek[i].value1, rek[i].value2);
            sscanf(buffor, "%X", &code[j].value);
        }

        else if (strcmp(rek[i].rozkaz, "SR") == 0)
        {
            sprintf(buffor, "%s%X%X", "12", rek[i].value1, rek[i].value2);
            sscanf(buffor, "%X", &code[j].value);
        }

        else if (strcmp(rek[i].rozkaz, "MR") == 0)
        {
            sprintf(buffor, "%s%X%X\n", "14", rek[i].value1, rek[i].value2);
            sscanf(buffor, "%X", &code[j].value);
        }

        else if (strcmp(rek[i].rozkaz, "DR") == 0)
        {
            sprintf(buffor, "%s%X%X", "16", rek[i].value1, rek[i].value2);
            sscanf(buffor, "%X", &code[j].value);
        }

        else if (strcmp(rek[i].rozkaz, "CR") == 0)
        {
            sprintf(buffor, "%s%X%X", "18", rek[i].value1, rek[i].value2);
            sscanf(buffor, "%X", &code[j].value);
        }

        else if (strcmp(rek[i].rozkaz, "LR") == 0)
        {
            sprintf(buffor, "%s%X%X", "31", rek[i].value1, rek[i].value2);
            sscanf(buffor, "%X", &code[j].value);
        }

        // skoki

        else if (strcmp(rek[i].rozkaz, "J") == 0)
        {
            for (x = 0; x < end; x++)
                if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                {
                    sprintf(buffor, "%s", "E00E");
                    sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                    sscanf(buffor, "%X", &code[j].value);
                    sscanf(delbuff, "%X", &code[j].delay);
                    break;
                }
        }

        else if (strcmp(rek[i].rozkaz, "JZ") == 0)
        {
            for (x = 0; x < end; x++)
                if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                {
                    sprintf(buffor, "%s", "E10E");
                    sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                    sscanf(buffor, "%X", &code[j].value);
                    sscanf(delbuff, "%X", &code[j].delay);
                    break;
                }
        }

        else if (strcmp(rek[i].rozkaz, "JP") == 0)
        {
            for (x = 0; x < end; x++)
                if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                {
                    sprintf(buffor, "%s", "E20E");
                    sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                    sscanf(buffor, "%X", &code[j].value);
                    sscanf(delbuff, "%X", &code[j].delay);
                    break;
                }
        }

        else if (strcmp(rek[i].rozkaz, "JN") == 0)
        {
            for (x = 0; x < end; x++)
                if (strcmp(rek[i].typefield, rek[x].etykieta) == 0)
                {
                    sprintf(buffor, "%s", "E30E");
                    sprintf(delbuff, "%02X%02X", rek[x].delay >> 8 & 0xFF, rek[x].delay >> 0 & 0xFF);
                    sscanf(buffor, "%X", &code[j].value);
                    sscanf(delbuff, "%X", &code[j].delay);
                    break;
                }
        }
        else
        {
             j--;
        }

    }
}

void makeshift(struct record *rek, int* del, int i, int* order_n, int* memory_n)
{
    rek[i].delay = *del;        //setting delays of order
    if (strcmp(rek[i].rozkaz, "AR") == 0 || strcmp(rek[i].rozkaz, "SR") == 0 || strcmp(rek[i].rozkaz, "MR") == 0 ||
        strcmp(rek[i].rozkaz, "DR") == 0 || strcmp(rek[i].rozkaz, "CR") == 0 || strcmp(rek[i].rozkaz, "LR") == 0)
    {
        *del += 2;
        (*order_n)++;
    }
    else if ((strcmp(rek[i].rozkaz, "DC") == 0 || strcmp(rek[i].rozkaz, "DS") == 0) && rek[i].value1 > 0)
    {
        *del += (rek[i].value1 * 4);
        *memory_n += rek[i].value1;
    }
    else if((strcmp(rek[i].rozkaz, "DC") == 0 || strcmp(rek[i].rozkaz, "DS") == 0))
    {
        *del += 4;
        (*memory_n)++;
    }
    else
    {
        *del += 4;
        (*order_n)++;
    }
}

int how_many_lines(FILE* plik)
{
    char line[50];
    int n = 1;
    while (fgets(line, MAX_REC_SIZE, plik) != NULL)
    {
        n++;
    }
    return n;
}