// SPDX-FileCopyrightText: 2006 Moritz Andreas Lenz <moritz.lenz@gmail.com>
//
// SPDX-License-Identifier: GPL-1.0-or-later OR Artistic-1.0

#include "sudoku_nosane.hpp"
sudoku_nosane::sudoku_nosane(void){
	sane = sudoku();
	sane.random_generate(40);
	for (int x = 0; x < 9; x++){
		for (int y = 0; y < 9; y++){
			data[x][y] = sane.get_item(x, y);
			locked[x][y] = (data[x][y] != 0);
		}
	}
	solved = sane;
	solved.solve();

}

void sudoku_nosane::null_init(){
	for (int x = 0; x < 9; x++){
		for (int y = 0; y < 9; y++){
			data[x][y] = 0;
		}
	}
}

int sudoku_nosane::count(){
	int res = 0;
	for (int x = 0; x < 9; x++){
		for (int y = 0; y < 9; y++){
			if (data[x][y] != 0){
				res++;
			}
		}
	}
	return res;
}

bool sudoku_nosane::is_solved(){
	for (int x = 0; x < 9; x++){
		for (int y = 0; y < 9; y++){
			if(data[x][y] != solved.get_item(x, y)){
				return false;
			}
		}
	}
	return true;
}

