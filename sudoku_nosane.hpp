// SPDX-FileCopyrightText: 2006 Moritz Andreas Lenz <moritz.lenz@gmail.com>
//
// SPDX-License-Identifier: GPL-1.0-or-later OR Artistic-1.0

#ifndef _MORITZ_SUDOKU_NOSANE_
#define _MORITZ_SUDOKU_NOSANE_

#include "sudoku.hpp"

class sudoku_nosane {

	public: 
		sudoku_nosane();
		sudoku sane;
		sudoku solved;
		int data[9][9];
		bool locked[9][9];
		int count();
		bool is_solved();
	protected:
		void null_init();
};

#endif
