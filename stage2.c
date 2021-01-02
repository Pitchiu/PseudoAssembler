#include "stage2.h"

void compile(int memory_n, int order_n, int debug_mode, int code_type)
{
	int records[16];										// records
	int i = 0;		
	int n = 0;
	int order_now;											//number of recent order
	int c;													//char from user
	unsigned long long state_register=0;					//register of state
	int sign=3;												//sign of last operation
	int memory_active, order_active, register_active;		//recent memory number
	int var1, var2;											//first and second variables from code
	int assembler_type = 0;									//0 - original, 1 - translated


	if (code_type == 1)
	{
		assembler_type = 1;
	}
	for (i = 0; i < 16; i++)
	{
		records[i] = 0;
	}
	for (i = 0; i < order_n; i++)
	{
		order_now = ((code[i].value>>8) & 0xFF);
		var1 = ((code[i].value >> 4) & 0xF);
		var2 = ((code[i].value) & 0xF);
		memory_active = -1;
		register_active = -1;
		order_active = -1;
		switch (order_now)
		{
			// --operacje arytmetyczne--
			case 0x10:
				records[var1] += records[var2];
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				order_active = i;
				break;

			case 0x12:
				records[var1] -= records[var2];
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				order_active = i;
				break;

			case 0x14:
				records[var1] *= records[var2];
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				order_active = i;
				break;

			case 0x16:
				records[var1] /= records[var2];
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				order_active = i;
				break;
			
			case 0x18:
				sign = sign_of_operation(records[var1]-records[var2]);
				register_active = var1;
				order_active = i;
				break;

			case 0xD1:
				n = ((records[var2] + code[i].delay) / 4);
				records[var1] += (memory[n].number);
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;

			case 0xD3:
				n = ((records[var2] + code[i].delay) / 4);
				records[var1] -= (memory[n].number);
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;

			case 0xD5:
				n = ((records[var2] + code[i].delay) / 4);
				records[var1] *= (memory[n].number);
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;

			case 0xD7:
				n = ((records[var2] + code[i].delay) / 4);
				records[var1] /= (memory[n].number);
				sign = sign_of_operation(records[var1]);
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;

			case 0xD9:
				n = ((records[var2] + code[i].delay) / 4);
				sign = sign_of_operation(records[var1]-memory[n].number);
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;

			//--instrukcje skoku--			
			case 0xE0:
				order_active = i;
				i = find_adress(code[i].delay, order_n) - 1;
				break;

			case 0xE1:
				order_active = i;
				if (sign == 0) { i = find_adress(code[i].delay, order_n) - 1; }
				break;
				
			case 0xE2:
				order_active = i;
				if (sign == 1) { i = find_adress(code[i].delay, order_n) - 1; }
				break;

			case 0xE3:
				order_active = i;
				if (sign == 2) { i = find_adress(code[i].delay, order_n) - 1; }
				break;

			//--zapisywanie wartoœci--
			case 0x31:
				records[var1] = records[var2];
				register_active = var1;
				order_active = i;
				break;

			case 0xF0:
				n = ((records[var2] + code[i].delay) / 4);
				records[var1] = (memory[n].number);
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;

			case 0xF2:
				n = ((records[var2] + code[i].delay) / 4);
				records[var1] = (memory[n].delay);
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;
			
			case 0xF3:
				n = ((records[var2] + code[i].delay) / 4);
				memory[n].ifused = 1;
				memory[n].number = records[var1];
				register_active = var1;
				memory_active = n;
				order_active = i;
				break;
		}
		if (debug_mode == 1)
		{
			console_print(order_n, memory_n, records, sign, register_active, memory_active, order_active, i+1, code_type, assembler_type);
			while ((c = getch()) != ' ')
			{
				if (c == 'h' || c=='H')
				{
					help_print();
					console_print(order_n, memory_n, records, sign, register_active, memory_active, order_active, i + 1, code_type, assembler_type);
				}
				if (c == 'f' || c == 'F')
				{
					debug_mode=0;
					break;
				}
				if (c == 'a' || c == 'A')
				{
					if (assembler_type == 0) { assembler_type = 1; }
					console_print(order_n, memory_n, records, sign, register_active, memory_active, order_active, i + 1, code_type, assembler_type);
				}
			}
		}
	}
	console_print(order_n, memory_n, records, sign, register_active, memory_active, order_active, i, code_type, assembler_type);
	while ((c = getch()) != ' ')
	{
		;
	}
	make_output_file(order_n, memory_n);
}

int sign_of_operation(int n)
{
	if (n == 0) { return 0; }
	else if (n > 0) { return 1; }
	else if (n < 0) { return 2; }
	else
	{
		return 3;
	}
}

int find_adress(int n, int order_n)
{
	int i;
	for (i = 0; i < order_n; i++)
	{
		if (code[i].own_delay == n)
		{
			return i;
		}
	}
}

void translate_machine(FILE* file, char* path, int debug_mode, int code_type)
{
	int memory_n = 0;
	int order_n = 0;
	char* token;
	char line[MAX_REC_SIZE];
	char operation[20];
	int iforders=0;
	int counter = 0;
	int buffer;
	int delay = 0;
	int i;

	check_lines_len(file, &memory_n, &order_n);
	code = (orders*)calloc(order_n, sizeof(orders));
	memory = (reservation*)calloc(memory_n, sizeof(reservation));
	assembler_code = (assembler_orders*)calloc(order_n, sizeof(assembler_orders));

	file = fopen(path, "r");

	while (fgets(line, MAX_REC_SIZE, file) != NULL)
	{

		if (counter >= memory_n && iforders == 0)
		{
			counter = 0;
			iforders = 1;
			delay = 0;
			continue;
		}

		strcpy(operation, "");			//making operation line empty

		token = strtok(line, " \t\n");
		if(token!=NULL){ strcat(operation, token); }
		token = strtok(NULL, " \t\n");
		if (token != NULL) { strcat(operation, token); }
		token = strtok(NULL, " \t\n");
		if (token != NULL) { strcat(operation, token); }
		token = strtok(NULL, " \t\n");
		if (token != NULL) { strcat(operation, token); }
		if (iforders == 0)
		{
			memory[counter].delay = delay;
			if (operation[0] == '~')
			{
				memory[counter].ifused = 0;
				memory[counter].number = 0;
			}
			else
			{
				memory[counter].ifused = 1;
				sscanf(operation, "%X", &memory[counter].number);
			}
			delay += 4;
			counter++;

		}

		if (iforders == 1)
		{
			sscanf(operation, "%X", &buffer);
			code[counter].own_delay = delay;
			if (strlen(operation) > 4)
			{
				code[counter].value = (buffer>>16 & 0xFFFF);
				code[counter].delay = (buffer>>0 & 0xFFFF);
				delay += 4;
			}
			else
			{
				code[counter].value = buffer;
				delay += 2;
			}
			counter++;
		}
	}
	compile(memory_n, order_n, debug_mode, code_type);
}

void check_lines_len(FILE* file, int* memory_n, int* order_n)
{
	char line[50];
	char* token;
	int	iforders = 0;

	while (fgets(line, MAX_REC_SIZE, file) != NULL)
	{
		token = strtok(line, " \t");
		if (strcmp(token, "\n") == 0)
		{
			iforders = 1;
			continue;
		}
		if (iforders == 0) { (*memory_n)++; }
		if (iforders == 1) { (*order_n)++; }
	}
}

void make_output_file(int order_n, int memory_n)
{
	FILE* file = NULL;
	int i;
	int n;

	file = fopen("output.txt", "w");
	for (i = 0; i < memory_n; i++)
	{
		if (memory[i].ifused == 0)
		{
			fprintf(file, "~~ ~~ ~~ ~~\n");
		}
		else
		{
			fprintf(file, "%02X %02X %02X %02X\n", memory[i].number >> 24 & 0xFF, memory[i].number >> 16 & 0xFF,
				memory[i].number >> 8 & 0xFF, memory[i].number >> 0 & 0xFF);
		}

	}
	fprintf(file, "\n");
	for (i = 0; i < order_n; i++)
	{
		n = (code[i].value >> 8 & 0xFF);
		if (n == 0x10 || n == 0x12 || n == 0x14 || n == 0x16 || n == 0x18 || n == 0x31)	//checking if line is register command
		{
			fprintf(file, "%02X %02X\n", code[i].value >> 8 & 0xFF, code[i].value >> 0 & 0xFF);
		}
		else
		{
			fprintf(file, "%02X %02X %02X %02X\n", code[i].value >> 8 & 0xFF, code[i].value >> 0 & 0xFF,
				code[i].delay >> 8 & 0xFF, code[i].delay >> 0 & 0xFF);
		}
	}
}
