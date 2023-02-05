#include "curses.h"
#include <stdio.h>
#include<stdbool.h>
#include <stdlib.h>
#include <string.h>

// struct linkedList
// {
//     int value;
//     struct linkedList *next;
// };

// #include "functions/others.h"
// #include "functions/find.h"
// #include "functions/grep.h"
// #include "functions/tree.h"
#include "functions/terminal.h"

#define WIN_MODE_HEIGHT 1
#define WIN_MODE_WIDTH 8
#define WIN_FILE_HEIGHT 1
#define WIN_FILE_WIDTH COLS - WIN_MODE_WIDTH
#define WIN_CMND_HEIGHT 1
#define WIN_CMND_WIDTH COLS
#define WIN_LINE_HEIGHT LINES - 2
#define WIN_LINE_WIDTH 5
#define WIN_CONT_HEIGHT LINES - 2
#define WIN_CONT_WIDTH COLS - WIN_LINE_WIDTH

#define COLOR_NORMAL_MODE 5
#define COLOR_VISUAL_MODE 6
#define COLOR_INSERT_MODE 7
#define COLOR_FILE 4
#define COLOR_CMND 3
#define COLOR_LINE 2
#define COLOR_LINE_EMPTY 10
#define COLOR_CONT 1
#define COLOR_CONT_HIGHLIGHT 11

#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define MAX_NUMBER_OF_LINES 1000000
#define MAX_CHAR_PER_LINE 5000

char *insertString(char *arr, int index, char ch);
char *removeString(char *arr, int index);
WINDOW *init_window_mode();
WINDOW *init_window_file();
WINDOW *init_window_command();
WINDOW *init_window_line();
WINDOW *init_window_content();
void update_window_mode(WINDOW *win_mode, int mode);
void update_window_file(WINDOW *win_file, bool saved, char *fileName);
int  update_window_cmnd(WINDOW *win_cmnd, char *command, int cursor_x, int pos_x);
void update_window_line(WINDOW *win_line, int lines[WIN_LINE_HEIGHT], int start_from);
void update_window_cont(WINDOW *win_cont, char **content, int *line_length, int start_from);
void navigate_win_cont(WINDOW *win_cont, int direction, char **content, int *line_length, int *strarting_line);
void clear_highlight(WINDOW *win_cont);
void highlight(WINDOW *win_cont, char **content, int *line_length, int *strarting_line, int from_y, int from_x, bool clear);
int countLines(char *fileAdddress);
int countLinesFromArray(int *line_length);
int wordListLength(char **list);
char ** insertToList(char **initial_list, char *item, int index);
char ** removeFromList(char **initial_list, int index);
int *line_length(char *fileAddress);
char ** fileToString(char *fileAddress, int *line_length);
void stringToFile(char **content, char *fileAddress);
void set_line_length(char **content, int **line_length, int **lineNo);
void copy_content(char **content, int *line_length, int line_from, int pos_from, int line_to, int pos_to);
void cut_content(char ***content, int *line_length, int line_from, int pos_from, int line_to, int pos_to);
void remove_content(char ***content, int *line_length, int line_from, int pos_from, int line_to, int pos_to);



int main()
{
	startApp();

	int mode = 1;
	bool startCommand = false;
	int cmnd_length = 0;
	int *linea = (int *) calloc(WIN_CONT_HEIGHT + 10, 4);
	int *lineNo = (int *) calloc(WIN_CONT_HEIGHT + 10, 4);
	char **content;
	int starting_line = 0;
	int start_visual_x, start_visual_y, start_visual_line;
	char address[CAPACITY] = ".hidden/temperoryFile.txt";
	bool isSaved = false, needsSave = false;

	initscr();
	start_color();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();

	refresh();

	init_pair(COLOR_NORMAL_MODE, COLOR_WHITE, COLOR_CYAN);
	init_pair(COLOR_INSERT_MODE, COLOR_WHITE, COLOR_YELLOW);
	init_pair(COLOR_VISUAL_MODE, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(COLOR_FILE, COLOR_WHITE, COLOR_BLUE);
	init_pair(COLOR_CMND, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_LINE, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_LINE_EMPTY, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_CONT, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_CONT_HIGHLIGHT, COLOR_BLACK, COLOR_WHITE);

	WINDOW *win_mode = init_window_mode();
	WINDOW *win_file = init_window_file();
	WINDOW *win_cmnd = init_window_command();
	WINDOW *win_line = init_window_line();
	WINDOW *win_cont = init_window_content();

	keypad(win_cmnd, TRUE);
	
	FILE *tmp_file = fopen(address, "w");
	fclose(tmp_file);
	content = fileToString(address, linea);
	set_line_length(content, &linea, &lineNo);

	
	refresh();


	wmove(win_cont, 0, 0);
	wmove(win_cmnd, 0, 0);

	

	update_window_line(win_line, lineNo, starting_line);
	update_window_file(win_file, isSaved, address);

	wmove(win_cont, 0, 0);
	wrefresh(win_cont);

	while (1)
	{
		if (mode == 1)
		{
			keypad(win_cont, TRUE);

			int cmnd_y, cmnd_x;
			getyx(win_cmnd, cmnd_y, cmnd_x);

			int ch;
			ch = wgetch(win_cont);

			// content = removeFromList(content, 1);
			// set_line_length(content, &linea, &lineNo);
			// update_window_cont(win_cont, content, linea, starting_line);

			if (cmnd_x == 0 && ch == ':')
			{
				for (int i = 0; i < WIN_CMND_WIDTH; i++)
					wprintw(win_cmnd, " ");
				mvwprintw(win_cmnd, 0, 0, ":");
				char *command = calloc(1, sizeof(char));
				cmnd_length++;
				int pos_x = 0;
				while (1)
				{
					getyx(win_cmnd, cmnd_y, cmnd_x);
					if (cmnd_length == 0)
					{
						update_window_cmnd(win_cmnd, " ", 0, 0);
						mvwprintw(win_cmnd, 0, 0, " ");
						wmove(win_cmnd, 0, 0);
						wrefresh(win_cmnd);
						break;
					}

					if (cmnd_x > pos_x + 1)
					{
						wmove(win_cmnd, 0, pos_x + 1);
						continue;
					}

					ch = wgetch(win_cmnd);
					if (ch == 10)
					{
						int index = 0;
						char *first = getString(command, &index);
						char output[CAPACITY] = {0};
						if (!strcmp(first, "open"))
						{
							stringToFile(content, address);
							wmove(win_cmnd, 0, 0);
							char *tmp_address = getString(command, &index);
							if (!fileExists(tmp_address + 1))
							{
								createNewFile(tmp_address + 1);
							}
							strcpy(address, tmp_address + 1);
							isSaved = true;
							needsSave = false;
							update_window_file(win_file, true, address);
							free(content);
							free(linea);
							linea = calloc(countLines(address) + WIN_CONT_HEIGHT, sizeof(int));
							content = fileToString(address, linea);
							set_line_length(content, &linea, &lineNo);
							starting_line = 0;
							update_window_line(win_line, lineNo, starting_line);
							update_window_cont(win_cont, content, linea, starting_line);
							wmove(win_cont, 0, 0);
						}
						else if (!strcmp(first, "save"))
						{
							if(!strcmp(address, ".hidden/temperoryFile.txt"))
							{
								mvwprintw(win_cmnd, 0, 0, "file name does not exist");
								for (int i = 0; i < WIN_FILE_WIDTH; i++)
									wprintw(win_cmnd, " ");
								wmove(win_cmnd, 0, 0);
								wrefresh(win_cmnd);
								break;
							}
							stringToFile(content, address);
							isSaved = true;
							needsSave = false;
							update_window_file(win_file, isSaved, address);
						}
						else if (!strcmp(first, "saveas"))
						{
							char *tmp_address = getString(command, &index);
							if (!fileExists(tmp_address + 1))
							{
								createNewFile(tmp_address + 1);
							}
							strcpy(address, tmp_address + 1);
							stringToFile(content, address);
							isSaved = true;
							needsSave = false;
							update_window_file(win_file, isSaved, address);
						}
						else
						{
							strcpy(output, terminal_v2(command));
							
						}
						mvwprintw(win_cmnd, 0, 0, "%s", output);
						for (int i = strlen(output); i < WIN_CMND_WIDTH; i++)
							mvwprintw(win_cmnd, 0, i, " ");
						wmove(win_cmnd, 0, 0);
						wrefresh(win_cmnd);
						break;
					}
					else if (ch == 8)
					{
						if (pos_x != 0 || cmnd_length == 1)
						{
							command = removeString(command, pos_x - 1);
							cmnd_length--;
							pos_x--;
							int ans = update_window_cmnd(win_cmnd, command, cmnd_x - 1, pos_x);
							if (ans == 1)
							{
								getyx(win_cmnd, cmnd_y, cmnd_x);
								wmove(win_cmnd, cmnd_y, cmnd_x - 2);
							}
							else if (ans == 2)
							{
								getyx(win_cmnd, cmnd_y, cmnd_x);
								wmove(win_cmnd, cmnd_y, cmnd_x - 1);
							}
						}
						
						//wmove(win_cmnd, 0, cmnd_x - 1);

						//wmove(win_cmnd, cmnd_y, cmnd_x - 1);
					}
					else if (ch == KEY_LEFT)
					{
						if (cmnd_x <= 5 && pos_x >= cmnd_x)
						{
							update_window_cmnd(win_cmnd, command, cmnd_x - 1, --pos_x);
						}
						else if (cmnd_x > 1)
						{
							pos_x--;
							wmove(win_cmnd, cmnd_y, cmnd_x - 1);
						}
					}
					else if (ch == KEY_RIGHT)
					{
						if (cmnd_x >= WIN_CMND_WIDTH - 7 && pos_x < cmnd_length - 6)
						{
							update_window_cmnd(win_cmnd, command, cmnd_x - 1, ++pos_x);
						}
						else if (cmnd_x < cmnd_length && cmnd_x < WIN_CMND_WIDTH - 2)
						{
							pos_x++;
							wmove(win_cmnd, cmnd_y, cmnd_x + 1);
						}
					}
					else if (ch <= 256)
					{
						command = insertString(command, pos_x, ch);
						cmnd_length++;
						pos_x++;
						update_window_cmnd(win_cmnd, command, cmnd_x - 1, pos_x);
						//wmove(win_cmnd, cmnd_y, cmnd_x + 1);
					}
					getyx(win_cmnd, cmnd_y, cmnd_x);
					wrefresh(win_cmnd);
				}
			}
			else switch(ch)
			{
				// case 'p':
				// 	stringToFile(content, ".hidden/tmp_paste.txt");
				// 	free(content);
				// 	int current_y, current_x;
				// 	getyx(win_cont, current_y, current_x);
				// 	//struct pos p = {current_y + starting_line + 1, current_x};
				// 	//pastestr(".hidden/tmp_paste.txt", p);
				// 	content = fileToString(".hidden/tmp_paste.txt", linea);
				// 	set_line_length(content, &linea, &lineNo);
				// 	update_window_line(win_line, lineNo, starting_line);
				// 	update_window_cont(win_cont, content, linea, starting_line);
				// 	wmove(win_cont, current_y, current_x);
				case 'n':
					update_window_mode(win_mode, 1);
					mode = 1;
					break;
				case 'v':
					update_window_mode(win_mode, 3);
					mode = 3;
					getyx(win_cont, start_visual_y, start_visual_x);
					start_visual_y += starting_line;
					starting_line = starting_line;
					break;
				case 'i':
					update_window_mode(win_mode, 2);
					mode = 2;
					break;
				case 'k':
				case KEY_UP:
					navigate_win_cont(win_cont, UP, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
				case 'j':
				case KEY_DOWN:
					navigate_win_cont(win_cont, DOWN, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
				case 'h':
				case KEY_LEFT:
					navigate_win_cont(win_cont, LEFT, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
				case 'l':
				case KEY_RIGHT:
					navigate_win_cont(win_cont, RIGHT, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
			}
		}
		else if (mode == 3)
		{
			int ch = wgetch(win_cont);
			int current_y, current_x;
			int goto_y, goto_x;
			getyx(win_cont, current_y, current_x);

			if (current_y + starting_line > start_visual_y)
			{
				goto_y = start_visual_y - starting_line;
				goto_x = start_visual_x;
			}
			else if (current_y + starting_line < start_visual_y)
			{
				goto_y = current_y;
				goto_x = current_x;
			}
			else if (current_x > start_visual_x)
			{
				goto_y = start_visual_y - starting_line;
				goto_x = start_visual_x;
			}
			else
			{
				goto_y = current_y;
				goto_x = current_x;
			}
			if (goto_y < 0)
				goto_y = 0;

			switch(ch)
			{
				case 'd':
					cut_content(&content, linea, start_visual_y, start_visual_x, current_y + starting_line, current_x);
					set_line_length(content, &linea, &lineNo);
					update_window_mode(win_mode, 1);
					update_window_cont(win_cont, content, linea, starting_line);
					update_window_line(win_line, lineNo, starting_line);
					clear_highlight(win_cont);
					wmove(win_cont, goto_y, goto_x);
					mode = 1;
					break;
				case 'y':
					copy_content(content, linea, start_visual_y, start_visual_x, current_y, current_x);
				case 'n':
					update_window_mode(win_mode, 1);
					clear_highlight(win_cont);
					mode = 1;
					break;
				case 'v':
					update_window_mode(win_mode, 3);
					mode = 3;
					break;
				case 'i':
					update_window_mode(win_mode, 2);
					mode = 2;
					break;
				case 'k':
				case KEY_UP:
					navigate_win_cont(win_cont, UP, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					highlight(win_cont, content, linea, &starting_line, start_visual_y - starting_line, start_visual_x, true);
					break;
				case 'j':
				case KEY_DOWN:
					navigate_win_cont(win_cont, DOWN, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					highlight(win_cont, content, linea, &starting_line, start_visual_y - starting_line, start_visual_x, true);
					break;
				case 'h':
				case KEY_LEFT:
					navigate_win_cont(win_cont, LEFT, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					highlight(win_cont, content, linea, &starting_line, start_visual_y - starting_line, start_visual_x, true);
					break;
				case 'l':
				case KEY_RIGHT:
					navigate_win_cont(win_cont, RIGHT, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					highlight(win_cont, content, linea, &starting_line, start_visual_y - starting_line, start_visual_x, true);
					break;
			}
		}
		else
		{

			int current_y, current_x;
			getyx(win_cont, current_y, current_x);

			int ch = wgetch(win_cont);
			if (ch == 27)
			{
				update_window_mode(win_mode, 1);
				mode = 1;
				continue;
			}

			isSaved = false;
			needsSave = true;
			update_window_file(win_file, isSaved, address);

			if (ch >= 32 && ch <= 126)
			{
				content[current_y + starting_line] = insertString(content[current_y + starting_line], current_x, ch);
				linea[current_y + starting_line] += 1;
				update_window_cont(win_cont, content, linea, starting_line);
				wmove(win_cont, current_y, current_x + 1);
			}
			else if (ch == 10)
			{
				int leng = linea[current_y + starting_line] - current_x;
				char *tmp = (char *) calloc(leng, sizeof(char));
				for (int i = 0; i < leng; i++)
				{
					tmp[i] = *(*(content + current_y + starting_line) + current_x);
					*(content + current_y + starting_line) = removeString(*(content + current_y + starting_line), current_x);
				}
				content = insertToList(content, tmp, current_y + starting_line + 1);
				set_line_length(content, &linea, &lineNo);
				if (current_y == WIN_CONT_HEIGHT - 1)
				{
					navigate_win_cont(win_cont, DOWN, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
				}
				else
				{
					update_window_cont(win_cont, content, linea, starting_line);
					wmove(win_cont, current_y + 1, 0);
					update_window_line(win_line, lineNo, starting_line);
				}
			}
			else if (ch == 9)
			{
				for (int x = 0; x < 4; x++)
					content[current_y + starting_line] = insertString(content[current_y + starting_line], current_x, ' ');
				linea[current_y + starting_line] += 4;
				update_window_cont(win_cont, content, linea, starting_line);
				wmove(win_cont, current_y, current_x + 4);
			}
			else if (ch == 8)
			{
				if (current_y + starting_line == 0 && current_x == 0)
					continue;
				if (current_x == 0)
				{
					char *tmp = calloc(linea[current_y + starting_line] + linea[current_y + starting_line -1] + 1, sizeof(char));
					int goto_x = linea[current_y + starting_line - 1];
					strcat(tmp, content[current_y + starting_line - 1]);
					strcat(tmp, content[current_y + starting_line]);
					content = removeFromList(content, current_y + starting_line);
					content = removeFromList(content, current_y + starting_line - 1);
					content = insertToList(content, tmp, current_y + starting_line - 1);
					set_line_length(content, &linea, &lineNo);
					if (current_y == 0)
					{
						wmove(win_cont, current_y, goto_x);
						navigate_win_cont(win_cont, UP, content, linea, &starting_line);
						update_window_line(win_line, lineNo, starting_line);
					}
					else
					{
						update_window_cont(win_cont, content, linea, starting_line);
						wmove(win_cont, current_y - 1, goto_x);
						update_window_line(win_line, lineNo, starting_line);
					}
				}
				else
				{
					*(content + current_y + starting_line) = removeString(*(content + current_y + starting_line), current_x - 1);
					linea[current_y + starting_line] -= 1;
					update_window_cont(win_cont, content, linea, starting_line);
					wmove(win_cont, current_y, current_x - 1);
				}
				
			}
			else switch(ch)
			{
				case KEY_UP:
					navigate_win_cont(win_cont, UP, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
				case KEY_DOWN:
					navigate_win_cont(win_cont, DOWN, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
				case KEY_LEFT:
					navigate_win_cont(win_cont, LEFT, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
				case KEY_RIGHT:
					navigate_win_cont(win_cont, RIGHT, content, linea, &starting_line);
					update_window_line(win_line, lineNo, starting_line);
					break;
			}
		}
	}

	// while (1)
	// {
	// 	int x, y;
	// 	getyx(win_cmnd, y, x);
	// 	if (x == 0)
	// 	{
	// 		startCommand = false;
	// 		wmove(win_cmnd, 0, 0);
	// 	}

	// 	int c = wgetch(win_cmnd);

	// 	if (c == ':' && mode == 1)
	// 		startCommand = true;
		
	// 	if (startCommand)
	// 	{
	// 		if (c == 8)
	// 		{
	// 			mvwprintw(win_cmnd, y, x - 1, " ");
	// 			wmove(win_cmnd, y, x - 1);
	// 		}
	// 		else
	// 			wprintw(win_cmnd, "%c", c);
	// 	}
	// 	else switch(c)
	// 	{
	// 		case 'n':
	// 			update_window_mode(win_mode, 1);
	// 			mode = 1;
	// 			break;
	// 		case 'v':
	// 			update_window_mode(win_mode, 3);
	// 			mode = 3;
	// 			break;
	// 		case 'i':
	// 			update_window_mode(win_mode, 2);
	// 			mode = 2;
	// 			break;
	// 	}
	// }


	getch();
	endwin();
}

char *insertString(char *arr, int index, char ch)
{
	char *output = calloc(strlen(arr) + 2, sizeof(char));
	for (int i = 0; i < index; i++)
		output[i] = arr[i];
	output[index] = ch;
	for (int i = index; i < strlen(arr); i++)
		output[i + 1] = arr[i];
	output[strlen(arr) + 1] = '\0';
	free(arr);
	return output;
}

char *removeString(char *arr, int index)
{
	char *output = calloc(strlen(arr), sizeof(char));
	for (int i = 0; i < index; i++)
		output[i] = arr[i];
	for (int i = index; i < strlen(arr); i++)
		output[i] = arr[i + 1];
	output[strlen(arr) - 1] = '\0';
	free(arr);
	return output;
}

WINDOW *init_window_mode()
{
	int y, x;
	getmaxyx(stdscr, y, x);
	WINDOW *win_mode = newwin(WIN_MODE_HEIGHT, WIN_MODE_WIDTH, y - 2, 0);
	wattron(win_mode, A_BOLD | COLOR_PAIR(COLOR_NORMAL_MODE));
	mvwprintw(win_mode, 0, 1, "NORMAL");
	wmove(win_mode, 0, 0);
	wchgat(win_mode, WIN_MODE_WIDTH, A_BOLD, COLOR_NORMAL_MODE, NULL);
	wrefresh(win_mode);
	return win_mode;
}

WINDOW *init_window_file()
{
	int y, x;
	getmaxyx(stdscr, y, x);
	WINDOW *win_file = newwin(WIN_FILE_HEIGHT, WIN_FILE_WIDTH, y - 2, WIN_MODE_WIDTH);
	wattron(win_file, A_BOLD | COLOR_PAIR(COLOR_FILE));
	wmove(win_file, 0, 0);
	wchgat(win_file, WIN_FILE_WIDTH, A_BOLD, COLOR_FILE, NULL);
	wrefresh(win_file);
	return win_file;
}

WINDOW *init_window_command()
{
	int y, x;
	getmaxyx(stdscr, y, x);
	WINDOW *win_cmnd = newwin(WIN_CMND_HEIGHT, WIN_CMND_WIDTH, y - 1, 0);
	wattron(win_cmnd, A_BOLD | COLOR_PAIR(COLOR_CMND));
	wmove(win_cmnd, 0, 0);
	wchgat(win_cmnd, WIN_CMND_WIDTH, A_BOLD, COLOR_CMND, NULL);
	wrefresh(win_cmnd);
	return win_cmnd;
}

WINDOW *init_window_line()
{
	int y, x;
	getmaxyx(stdscr, y, x);
	WINDOW *win_line = newwin(WIN_LINE_HEIGHT, WIN_LINE_WIDTH, 0, 0);
	wattron(win_line, A_BOLD | COLOR_PAIR(COLOR_LINE));
	for (int i = 0; i < WIN_LINE_HEIGHT; i++)
	{
		wmove(win_line, i, 0);
		wchgat(win_line, WIN_LINE_WIDTH, A_BOLD, COLOR_LINE, NULL);
	}
	wrefresh(win_line);
	return win_line;
}

WINDOW *init_window_content()
{
	int y, x;
	getmaxyx(stdscr, y, x);
	WINDOW *win_cont = newwin(WIN_CONT_HEIGHT, WIN_CONT_WIDTH, 0, WIN_LINE_WIDTH);
	wattron(win_cont, COLOR_PAIR(COLOR_CONT));
	for (int i = 0; i < WIN_CONT_HEIGHT; i++)
	{
		wmove(win_cont, i, 0);
		wchgat(win_cont, WIN_CONT_WIDTH, A_NORMAL, COLOR_CONT, NULL);
	}
	wrefresh(win_cont);
	return win_cont;

}

void update_window_mode(WINDOW *win_mode, int mode)
{
	switch(mode)
	{
		case 1:
			mvwprintw(win_mode, 0, 1, "NORMAL");
			wmove(win_mode, 0, 0);
			wchgat(win_mode, WIN_MODE_WIDTH, A_BOLD, COLOR_NORMAL_MODE, NULL);
			break;
		case 2:
			mvwprintw(win_mode, 0, 1, "INSERT");
			wmove(win_mode, 0, 0);
			wchgat(win_mode, WIN_MODE_WIDTH, A_BOLD, COLOR_INSERT_MODE, NULL);
			break;
		case 3:
			mvwprintw(win_mode, 0, 1, "VISUAL");
			wmove(win_mode, 0, 0);
			wchgat(win_mode, WIN_MODE_WIDTH, A_BOLD, COLOR_VISUAL_MODE, NULL);
			break;
	}
	wrefresh(win_mode);
}

void update_window_file(WINDOW *win_file, bool saved, char *fileName)
{
	for (int i = 0; i < WIN_FILE_WIDTH; i++)
		mvwprintw(win_file, 0, i, " ");
	if (!saved)
		mvwprintw(win_file, 0, 1, "+");
	if (strcmp(fileName, ".hidden/temperoryFile.txt"))
		mvwprintw(win_file, 0, 3, "%s", fileName);
	else
		mvwprintw(win_file, 0, 3, "[no file name]");
	wrefresh(win_file);
}

int update_window_cmnd(WINDOW *win_cmnd, char *command, int cursor_x, int pos_x)
{
	int length = strlen(command);


	if (length < WIN_CMND_WIDTH - 2)
	{
		mvwprintw(win_cmnd, 0, 0, ":%s", command);
		for (int i = length + 1; i < WIN_CMND_WIDTH - 1; i++)
			mvwprintw(win_cmnd, 0, i, " ", command);
		wrefresh(win_cmnd);
		wmove(win_cmnd, 0, cursor_x + 2);
		if (cursor_x > pos_x + 1)
			wmove(win_cmnd, 0, pos_x + 3);
		return 0;
	}
	else if (pos_x >= cursor_x)
	{
		mvwprintw(win_cmnd, 0, 0, ":%s", command + pos_x - cursor_x);
		mvwprintw(win_cmnd, 0, WIN_CMND_WIDTH - 2, "  ");
		wmove(win_cmnd, 0, cursor_x + 1);
		wrefresh(win_cmnd);
		return 0;
	}
	else
	{
		mvwprintw(win_cmnd, 0, 0, ":%s", command);
		mvwprintw(win_cmnd, 0, WIN_CMND_WIDTH - 2, "  ");
		wmove(win_cmnd, 0, cursor_x + 1);
		wrefresh(win_cmnd);
		return 0;
	}
}

void update_window_line(WINDOW *win_line, int lines[WIN_LINE_HEIGHT], int start_from)
{
	for (int i = 0; i < WIN_LINE_HEIGHT; i++)
	{
		if (lines[i + start_from])
			mvwprintw(win_line, i, 0, "%4d", lines[i + start_from]);
		else
		{
			mvwprintw(win_line, i, 0, "~");
			for (int i = 1; i < WIN_LINE_WIDTH; i++)
				wprintw(win_line, " ");
			wmove(win_line, i, 0);
			wchgat(win_line, WIN_LINE_WIDTH, A_BOLD, COLOR_LINE_EMPTY, NULL);
		}
	}
	wrefresh(win_line);
}

void update_window_cont(WINDOW *win_cont, char **content, int *line_length, int start_from)
{
	int length = wordListLength(content);
	int i;
	for (i = 0; i < length && i < WIN_CONT_HEIGHT; i++)
	{
		mvwprintw(win_cont, i, 0, "%s", content[i + start_from]);
		for (int j = line_length[i + start_from]; j < WIN_CONT_WIDTH; j++)
			mvwprintw(win_cont, i, j, " ");
	}
	while (i < WIN_CONT_HEIGHT)
	{
		for (int j = 0; j < WIN_CONT_WIDTH; j++)
			mvwprintw(win_cont, i, j, " ");
		i++;
	}
	wrefresh(win_cont);
}

void navigate_win_cont(WINDOW *win_cont, int direction, char **content, int *line_length, int *strarting_line)
{
	int y, x, line_count = wordListLength(content);
	getyx(win_cont, y, x);
	if (direction == LEFT && x > 0)
		wmove(win_cont, y, x - 1);
	else if (direction == RIGHT && x < line_length[*strarting_line + y])
		wmove(win_cont, y, x + 1);
	else if (direction == UP && y < 5 && *strarting_line > 0)
	{
		update_window_cont(win_cont, content, line_length, *strarting_line - 1);
		*strarting_line -= 1;
		if (x > line_length[*strarting_line + y - 1])
			wmove(win_cont, y, line_length[*strarting_line + y - 1]);
		else
			wmove(win_cont, y, x);
	}
	else if (direction == UP && y > 0)
	{
		if (x > line_length[*strarting_line + y - 1])
			wmove(win_cont, y - 1, line_length[*strarting_line + y - 1]);
		else
			wmove(win_cont, y - 1, x);
	}
	else if (direction == DOWN && y > WIN_CONT_HEIGHT - 6 && *strarting_line + WIN_CONT_HEIGHT < line_count)
	{
		update_window_cont(win_cont, content, line_length, *strarting_line + 1);
		*strarting_line += 1;
		if (x > line_length[*strarting_line + y + 1])
			wmove(win_cont, y, line_length[*strarting_line + y + 1]);
		else
			wmove(win_cont, y, x);
	}
	else if (direction == DOWN && y < line_count - 1)
	{
		if (x > line_length[*strarting_line + y + 1])
			wmove(win_cont, y + 1, line_length[*strarting_line + y + 1]);
		else
			wmove(win_cont, y + 1, x);
	}
	wrefresh(win_cont);
}

void clear_highlight(WINDOW *win_cont)
{
	int y, x;
	getyx(win_cont, y, x);
	for (int i = 0; i < WIN_CONT_HEIGHT; i++)
	{
		wmove(win_cont, i, 0);
		wchgat(win_cont, -1, A_NORMAL, COLOR_CONT, NULL);
	}
	wmove(win_cont, y, x);
	wrefresh(win_cont);
}

void highlight(WINDOW *win_cont, char **content, int *line_length, int *strarting_line, int from_y, int from_x, bool clear)
{
	if (clear)
		clear_highlight(win_cont);

	int to_x, to_y;
	getyx(win_cont, to_y, to_x);

	if (to_y == from_y)
	{
		if (to_x == from_x)
			return;
		else if (to_x > from_x)
		{
			wmove(win_cont, from_y, from_x);
			wchgat(win_cont, to_x - from_x, A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
		}
		else if (to_x < from_x)
		{
			wmove(win_cont, to_y, to_x);
			wchgat(win_cont, from_x - to_x, A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
		}
	}

	if (to_y > from_y)
	{
		int start = 0;
		if (from_y >= 0)
		{
			wmove(win_cont, from_y, from_x);
			wchgat(win_cont, line_length[from_y + *strarting_line] - from_x, A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
			start = from_y + 1;
		}
		for (int i = start; i < to_y; i++)
		{
			wmove(win_cont, i, 0);
			wchgat(win_cont, line_length[i + *strarting_line], A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
		}
		wmove(win_cont, to_y, 0);
		wchgat(win_cont, to_x, A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
	}

	if (to_y < from_y)
	{
		wchgat(win_cont, line_length[to_y + *strarting_line] - to_x, A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
		
		for (int i = to_y + 1; i < from_y && i < WIN_CONT_HEIGHT; i++)
		{
			wmove(win_cont, i, 0);
			wchgat(win_cont, line_length[i + *strarting_line], A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
		}

		if (from_y < WIN_CONT_HEIGHT)
		{
			wmove(win_cont, from_y, 0);
			wchgat(win_cont, from_x, A_NORMAL, COLOR_CONT_HIGHLIGHT, NULL);
		}
	}

	wmove(win_cont, to_y, to_x);
	wrefresh(win_cont);
}


int wordListLength(char **list)
{
	int i = 0;
	while(list[i] != NULL)
		i++;
	return i;
}

char ** insertToList(char **initial_list, char *item, int index)
{
	int length = wordListLength(initial_list);
	char **output = calloc(length + 3, sizeof(char *));
	for (int i = 0; i < index; i++)
		output[i] = initial_list[i];
	output[index] = item;
	for (int i = index; i < length; i++)
		output[i + 1] = initial_list[i];
	free(initial_list);
	output[length + 1] = NULL;
	return output;
}

char ** removeFromList(char **initial_list, int index)
{
	int length = wordListLength(initial_list);
	char **output = calloc(length + 1, sizeof(char *));
	for (int i = 0; i < index; i++)
		output[i] = initial_list[i];
	for (int i = index; i < length; i++)
		output[i] = initial_list[i + 1];
	free(initial_list);
	output[length - 1] = NULL;
	return output;
}

int countLines(char *fileAddress)
{
	int att[4] = {1, 0, 0, 0};
	return find(fileAddress, "\n", att)->value + 1;
}

int countLinesFromArray(int *line_length)
{
	int i = 0;
	while (line_length[i] > 0)
		i++;
	return i;
}

int * line_length(char *fileAddress)
{
	FILE *read = fopen(fileAddress, "r");
	int *output = (int *) calloc(countLines(fileAddress), sizeof(int));
	int line = 0;
	char c;
	while ((c = fgetc(read)) != EOF)
	{
		if (c != '\n')
			output[line]++;
		else
			line++;
	}
	fclose(read);
	return output;
}



char ** fileToString(char *fileAddress, int *line_length)
{
	FILE *read = fopen(fileAddress, "r");
	int length = countLines(fileAddress);
	char **output = (char **) calloc(length + 1, sizeof(char *));
	char *word = (char *) calloc(MAX_CHAR_PER_LINE, sizeof(char));
	int line = 0;
	char c;
	while ((c = fgetc(read)) != EOF)
	{
		int x = line_length[line];
		if (c != '\n')
		{
			word[x] = c;
			line_length[line] += 1;
		}
		else
		{
			word[x] = '\0';
			char *tmp = calloc(x + 1, sizeof(char));
			strcpy(tmp, word);
			output[line] = tmp;
			line++;
		}
	}
	int x = line_length[line];
	word[x] = '\0';
	char *tmp = calloc(x + 1, sizeof(char));
	strcpy(tmp, word);
	output[line] = tmp;
	line++;
	fclose(read);
	output[length] = NULL;
	return output;
}

void stringToFile(char **content, char *fileAddress)
{
	FILE *write = fopen(fileAddress, "w");
	int length = wordListLength(content);
	for (int i = 0; i < length - 1; i++)
	{
		fputs(content[i], write);
		fputc('\n', write);
	}
	fputs(content[length - 1], write);
	fclose(write);
}

void set_line_length(char **content, int **line_length, int **lineNo)
{
	int length = wordListLength(content);
	free(*line_length);
	free(*lineNo);
	*line_length = (int *) calloc(length + WIN_CONT_HEIGHT, sizeof(int));
	*lineNo      = (int *) calloc(length + WIN_CONT_HEIGHT, sizeof(int));
	for (int i = 0; i < length; i++)
	{
		(*line_length)[i] = strlen(content[i]);
		(*lineNo)[i] = i + 1;
	}
}

void copy_content(char **content, int *line_length, int line_from, int pos_from, int line_to, int pos_to)
{
	if ((line_from > line_to) || (line_from == line_to && pos_from > pos_to))
	{
		int tmp = line_from;
		line_from = line_to;
		line_to = tmp;
		tmp = pos_from;
		pos_from = pos_to;
		pos_to = tmp;
	}

	int i = line_from, j = pos_from;
	FILE *write = fopen(".hidden/clipboard.txt", "w");
	while (1)
	{
		if (j >= pos_to && i >= line_to)
			break;

		char c = *(*(content + i) + j);
		if (c == '\0')
		{
			i++;
			j = 0;
			fputc('\n', write);
		}
		else
		{
			fputc(c, write);
			j++;
		}
	}
	fclose(write);
}

void cut_content(char ***content, int *line_length, int line_from, int pos_from, int line_to, int pos_to)
{
	copy_content(*content, line_length, line_from, pos_from, line_to, pos_to);
	remove_content(content, line_length, line_from, pos_from, line_to, pos_to);
}

void remove_content(char ***content, int *line_length, int line_from, int pos_from, int line_to, int pos_to)
{
	if ((line_from > line_to) || (line_from == line_to && pos_from > pos_to))
	{
		int tmp = line_from;
		line_from = line_to;
		line_to = tmp;
		tmp = pos_from;
		pos_from = pos_to;
		pos_to = tmp;
	}

	int i = line_from, j = pos_from;
	int first_char_to_remove = pos_from;
	int first_line_to_remove = line_from;
	
	if (line_from == line_to)
	{
		for (int x = pos_from; x < pos_to; x++)
			(*content)[line_from] = removeString((*content)[line_from], pos_from);
	}
	else
	{
		for (int x = pos_from; x < line_length[line_from]; x++)
			(*content)[line_from] = removeString((*content)[line_from], pos_from);
		for (int x = 0; x < pos_to; x++)
			(*content)[line_to] = removeString((*content)[line_to], 0);
		for (int x = 0; x < line_length[line_to] - pos_to; x++)
			(*content)[line_from] = insertString((*content)[line_from], pos_from + x, *(*((*content) + line_to) + x));
		for (int x = line_from + 1; x <= line_to; x++)
			(*content) = removeFromList((*content), line_from + 1);
	}
}

// void paste_content(char **content, int line, int pos)
// {
// 	if (!fileExists(".hidden/clipboard.txt"))
//         return;
// 	FILE *read = fopen(".hidden/clipboard.txt", "r");
// 	while (1)
// 	{
// 		char c = fgetc(read);
// 		if (c == EOF)

// 	}
// }