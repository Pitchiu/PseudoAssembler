#include "stage3.h"

void console_print(int order_n, int memory_n, int registers[], int sign, int register_active, int memory_active, int order_active, int next_operation, int code_type, int assembler_type)
{
	print_boxes(order_n, memory_n);
	print_programstate(next_operation, order_n, memory_n, sign);
	print_assemblerlist(order_n, order_active, assembler_type);
	print_orderlist(order_n, order_active);
	print_registerlist(register_active, registers);
	print_memorylist(memory_n, memory_active);
	noecho();
}

void print_boxes(int order_n, int memory_n)
{
	int i;
	int border_order;
	int border_memory;
	if (order_n > 23) { border_order = 25; }
	else
		border_order = order_n + 2;
	if (memory_n > 23) { border_memory = 25; }
	else
		border_memory = memory_n + 2;

	initscr();
	WINDOW* program_state, * order_list, * register_list, * memory_list, * assembler_list;
	mvprintw(0, 45, "%s", "REJESTR STANU PROGRAMU");
	mvprintw(4, 6, "%s", "ROZKAZY PSEUDOASSEMBLERA");
	mvprintw(4, 45, "%s", "ROZKAZY");
	mvprintw(4, 68, "%s", "REJESTRY");
	mvprintw(4, 88, "%s", "SEKCJA DANYCH");

	program_state = newwin(3, 32, 1, 40);
	box(program_state, 0, 0);

	assembler_list = newwin(border_order, 36, 5, 0);
	box(assembler_list, 0, 0);

	order_list = newwin(border_order, 21, 5, 38);
	box(order_list, 0, 0);

	register_list = newwin(18, 21, 5, 61);
	box(register_list, 0, 0);

	memory_list = newwin(border_memory, 22, 5, 84);
	box(memory_list, 0, 0);
	refresh();

	wrefresh(program_state);
	wrefresh(assembler_list);
	wrefresh(order_list);
	wrefresh(register_list);
	wrefresh(memory_list);

 }

void print_programstate(int next_operation, int order_n, int memory_n, int sign)
{
	WINDOW* win;
	win = newwin(1, 30, 2, 41);
	int location; 
	location = memory_n * 4 + code[next_operation].own_delay;
	mvwprintw(win, 0, 0, "%02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X", 0, 0, sign << 6 & 0xFF, 0, location >> 24 & 0xFF, location >> 16 & 0xFF, location >> 8 & 0xFF, location >> 0 & 0xFF);
	wrefresh(win);
}
void print_assemblerlist(int order_n, int order_active, int show_assembler_type)
{
	int i;
	int length;
	WINDOW* win;

	win = newpad(order_n, 34);
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_WHITE);
 	for (i = 0; i < order_n; i++)
	{
		if (show_assembler_type == 1)
		{
			construct_assembler_line(i);
		}
		if (i == order_active)
		{
			wattron(win, COLOR_PAIR(1));
			wmove(win, i, 0);
			wprintw(win, "%-35s", assembler_code[i].line);
			wattroff(win, COLOR_PAIR(1));
		}
		else
		{
			wmove(win, i, 0);
			wprintw(win, "%-35s", assembler_code[i].line);
		}
	}
	if (order_active > 22) { i = order_active-22; }
	else
		i = 0;

	if (order_n > 23) { length = 25; }
	else
		length = order_n+2;

	prefresh(win, i, 0, 6, 1, length+3, 34);
}

void print_orderlist(int order_n, int order_active)
{
	int i;
	int length;
	char buffer[20];
	WINDOW* win;
	int n;

	win = newpad(order_n, 19);
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_WHITE);
	for (i = 0; i < order_n; i++)
	{
		n = (code[i].value >> 8 & 0xFF);
		if (n == 0x10 || n == 0x12 || n == 0x14 || n == 0x16 || n == 0x18 || n == 0x31)	//checking if line is register command
		{
			sprintf(buffer, "%4d %02X  %02X        ", code[i].own_delay, code[i].value >> 8 & 0xFF,
			code[i].value >> 0 & 0xFF);
		}
		else
		{
			sprintf(buffer, "%4d %02X  %02X  %02X  %02X", code[i].own_delay, code[i].value >> 8 & 0xFF,
			code[i].value >> 0 & 0xFF, code[i].delay >> 8 & 0xFF, code[i].delay >> 0 & 0xFF);
		}

		if (i == order_active)
		{
			wattron(win, COLOR_PAIR(1));
			wmove(win, i, 0);
			wprintw(win, "%.19s", buffer);
			wattroff(win, COLOR_PAIR(1));
		}
		else
		{
			wmove(win, i, 0);
			wprintw(win, "% .19s", buffer);
		}
	}
	if (order_active > 22) { i = order_active - 22; }
	else
		i = 0;

	if (order_n > 23) { length = 25; }
	else
		length = order_n+2;

	prefresh(win, i, 0, 6, 39, length + 3, 58);
}

void print_registerlist(int register_active, int *registers)
{
	WINDOW* win;
	int i;
	char buffer[19];
	win = newwin(16, 18, 6, 62);

	for (i = 0; i < 16; i++)
	{
		sprintf(buffer, "%2d%c %02X  %02X  %02X  %02X", i, '.', registers[i] >> 24 & 0xFF, registers[i] >> 16 & 0xFF,
			registers[i] >> 8 & 0xFF, registers[i] >> 0 & 0xFF);
		if (i == register_active)
		{
			wattron(win, COLOR_PAIR(1));
			wmove(win, i, 0);
			wprintw(win, "%.18s", buffer);
			wattroff(win, COLOR_PAIR(1));
		}
		else
		{
			wmove(win, i, 0);
			wprintw(win, "%.18s", buffer);
		}
	}
	wrefresh(win);
}

void print_memorylist(int memory_n, int memory_active)
{
	int i;
	int length;
	char buffer[21];
	WINDOW* win;

	win = newpad(memory_n, 20);
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_WHITE);
	for (i = 0; i < memory_n; i++)
	{
		if (memory[i].ifused == 1)
		{
			sprintf(buffer, "%4d  %02X  %02X  %02X  %02X", memory[i].delay, memory[i].number >> 24 & 0xFF,
				memory[i].number >> 16 & 0xFF, memory[i].number >> 8 & 0xFF, memory[i].number >> 0 & 0xFF);
		}
		else
		{
			sprintf(buffer, "%4d  %s", memory[i].delay, "~~  ~~  ~~  ~~");
		}

		if (i == memory_active)
		{
			wattron(win, COLOR_PAIR(1));
			wmove(win, i, 0);
			wprintw(win, "%.20s", buffer);
			wattroff(win, COLOR_PAIR(1));
		}
		else
		{
			wmove(win, i, 0);
			wprintw(win, "%.20s", buffer);
		}
	}
	if (memory_active > 22) { i = memory_active - 22; }
	else
		i = 0;

	if (memory_n > 25) { length = 25; }
	else
		length = memory_n+2;

	prefresh(win, i, 0, 6, 85, length + 3, 105);
}

void help_print()
{
	int c;
	clear();
	initscr();
	noecho();
	move(0, 0);
	printw("%s", "[SPACE] - NEXT STEP\n[F] - FINISH PROGRAM WITHOUT DEBUG MODE\n[A] - CHANGE WAY OF SHOWING PSEUDOASSEMBLER CODE\n(PRESS H TO CLOSE)");
	while ((c = getch()) != 'h' && c != 'H')
	{
		;
	}
	endwin();
}

void construct_assembler_line(int n)
{
	int order_now;
	order_now = ((code[n].value >> 8) & 0xFF);
	switch (order_now)
	{
		// --operacje arytmetyczne--
	case 0x10:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d", code[n].label, "AR", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF);
		break;

	case 0x12:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d", code[n].label, "SR", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF);
		break;

	case 0x14:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d", code[n].label, "MR", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF);
		break;

	case 0x16:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d", code[n].label, "DR", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF);
		break;

	case 0x18:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d", code[n].label, "CR", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF);
		break;

	case 0xD1:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "A ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xD3:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "S ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xD5:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "M ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xD7:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "D ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xD9:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "C ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

		//--instrukcje skoku--			
	case 0xE0:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "J ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xE1:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "JZ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xE2:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "JP", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xE3:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "JN", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

		//--zapisywanie wartoœci--
	case 0x31:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d", code[n].label, "LR", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF);
		break;

	case 0xF0:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "L ", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xF2:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "LA", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;

	case 0xF3:
		sprintf(assembler_code[n].line, "%-15s%s   %d%c %d%c%d%c", code[n].label, "ST", (code[n].value >> 4) & 0xF, ',', (code[n].value) & 0xF, '(', code[n].delay, ')');
		break;
	}
}