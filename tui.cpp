// SPDX-FileCopyrightText: 2006 Moritz Andreas Lenz <moritz.lenz@gmail.com>
//
// SPDX-License-Identifier: GPL-1.0-or-later OR Artistic-1.0

#include "sudoku.hpp"
#include "sudoku_nosane.hpp"
#include <curses.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;
static void finish(int);

int cx = 0;
int cy = 0;

static const int msg_x = 0;
static const int msg_y = 14;
sudoku_nosane s = sudoku_nosane();
static const int y_map[9] = {1, 2, 3, 5, 6, 7, 9, 10, 11};

void check(bool hilight){
	bool wrong = false;
	attrset(COLOR_PAIR(2));
	for (int x = 0; x < 9; x++){
		for (int y = 0; y < 9; y++){
			if (s.data[x][y] != 0 && s.data[x][y] 
					!= s.solved.get_item(x, y)){
				wrong = true;
				if (hilight){
					int tmp = s.data[x][y];
					move(y_map[y], 2 + x * 4);
					addch('0' + tmp);
				}
			}

		}
	}
	attrset(A_NORMAL);
	if (wrong){
		move(msg_y, msg_x);
		addstr("You are wrong, sorry.");
	} else {
		move(msg_y, msg_x);
		addstr("You are right so far.");
	}


}

void refresh_xy(int x, int y){
	int tmp = s.data[x][y];
	move(y_map[y], 2 + x * 4);
	if (s.locked[x][y]){
		attrset(A_BOLD | COLOR_PAIR(1));
	} else {
		attrset(A_NORMAL);
	}
	if (tmp == 0){
		addch(' ');
	} else {
		addch((char) '0' + tmp);
	}

}

void draw_initial_window() {
	if (! (COLS >= 37 && LINES >= 19)){
		endwin();
		cerr << "Terminal to small, dying!\n";
		exit(-1);
	}
	move(0, 0);
	addstr("+-----------+-----------+-----------+");
	move(4, 0);
	
	addstr("+-----------+-----------+-----------+");
	move(8, 0);
	addstr("+-----------+-----------+-----------+");
	move(12, 0);
	addstr("+-----------+-----------+-----------+");

	for (int i = 0; i < 9; i++){
		move(y_map[i], 0);
		addstr("|           |           |           |");
	}

	for (int x = 0; x < 9; x++){
		for (int y = 0; y < 9; y++){
			refresh_xy(x, y);
		}
	}
}

int main(int argc, char** argv){
	srand(time(0));
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	cbreak();

	mousemask(BUTTON1_CLICKED | BUTTON2_CLICKED | BUTTON3_CLICKED, 0);
	MEVENT mouse_event;

	if (has_colors()){
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_RED, COLOR_WHITE);
	}

	draw_initial_window();	
	move(1, 2);
	int key = 0;
	while (key != 'q' && key != 'Q'){
		key = getch();
		switch (key){
			case KEY_MOUSE:
				// TODO
				if (OK != getmouse(&mouse_event)){
					break;
				}
				for (int x = 0; x < 9; x++){
					if (abs(2+4*x - mouse_event.x) < 3){
						cx = x;
					}
				}
				for (int y = 0; y < 9; y++){
					if (abs(y_map[y] - mouse_event.y + 1) < 2){
						cy = y;
					}
				}

				break;

			case KEY_DOWN:
			case 'j': /*be friendly to vi users ;-) */
				refresh_xy(cx, cy);
				cy = (cy+1) % 9;
				break;
			case KEY_UP:
			case 'k':
				refresh_xy(cx, cy);
				cy = (cy+8) % 9;
				break;
			case KEY_RIGHT:
			case KEY_SRIGHT:
			case 'l':
				refresh_xy(cx, cy);
				cx = (cx+1) % 9;
				break;
			case KEY_LEFT:
			case KEY_SLEFT:
			case 'h':
				refresh_xy(cx, cy);
				cx = (cx+8) % 9;
				break;
			case KEY_DC:
			case KEY_DL:
			case 'd':
			case '0':
				if (!s.locked[cx][cy]){
					s.data[cx][cy] = 0;
				}
				break;
			case 'L':
				s.locked[cx][cy] = !s.locked[cx][cy];
				break;
			case 'c':
				check(false);
				break;
			case 'C':
				check(true);
				break;
			case 'H':
				s.data[cx][cy] = s.solved.get_item(cx, cy);
				s.locked[cx][cy] = true;
				break;
			case 'n':
				s = sudoku_nosane();
				draw_initial_window();
				break;
		}
		if (key > '0' && key <= '9' && !s.locked[cx][cy]){
			s.data[cx][cy] = key - '0';

		}

		refresh_xy(cx, cy);
		if (s.is_solved()){
			move(msg_y, msg_x);
			addstr("Hurray, solved! (exit with 'q', start over with 'n')");

		}

		// move cursor to current position:
		move(y_map[cy], 2 + 4 * cx);


	}


	finish(0);
	return 0;
}

static void finish(int sig)
{
    endwin();
    /* do your non-curses wrapup here */
    exit(sig);
}



