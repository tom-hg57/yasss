/* sudoku.hpp
 * a sudoku playing field.
 * 
 * Copyright (C) Moritz Lenz <moritz@faui2k3.org> 2005
 *
 * Part of the sudoku Projekt.
 * License: take a look at main.cpp (in short: GPL or BSD artistic, whatever
 * you like)
 */

#ifndef _MORITZ_FIELD_
#define _MORITZ_FIELD_
#include <iostream>

// a sudoku playing field implemented as a 2d fixed size array
// contains consitency checks and a solver.
class sudoku {
	public:
		sudoku();
		// creates a field with inital data. 0 means "not set".
		// Note that the first coordinate ist considered as x, so if
		// you create an array char f= {{1 ,2 ...}, {..}} you will get
		// the transpoesed sudoku field. but don't worry, sudoku is
		// invariant under transposition
		sudoku(char init_data[9][9]);
		sudoku(char* init_data);

		// creates a field with inital data. 0 means "not set".
		// Note that the first coordinate ist considered as x, so if
		// you create an array char f= {{1 ,2 ...}, {..}} you will get
		// the transpoesed sudoku field. but don't worry, sudoku is
		// invariant under transposition
		sudoku(int init_data[9][9]);

		// generates a rather simplistic output to the given stream
		// call as pretty_print(cout) or something like that...
		void pretty_print(std::ostream &handle);

		// generates a very pretty looking svg output 
		// that is understood for example by firefox, opera 
		// and inkscape

		void svg_print(std::ostream &handle);

		// just print all chars in one row
		void print(std::ostream &handle);

		// sets item (x, y) to val
		// assumes that it doesn't lead to an intermediate
		// confilict with sudoku rules
		// which is equivalent to saying it requires 
		// allowed_set(val, x, y) to be true
		void set_item(char val, int x, int y);

		// get entry at position (x, y)
		// 0 means "unset"
		int get_item(int x, int y);
		
		// returns true if it doesn't lead to a direct error if you
		// set (x, y) to val
		// If data[x][y] != 0 the return value is always false
		inline bool allowed_set(char val, int x, int y){
			return allowed[x][y][val - 1];
		}

		// try to solve the puzzle. Returns true on success.
		bool solve();
		
		// returns true if there is no zero entry left, i.e. the
		// problem is solved correctly.
		bool is_solved();

		// returns true if there is no possibilty to continue without
		// violating rule
		bool is_stuck();

		int get_solution_count();

		bool count_solutions;

		bool calculate_difficulty_rating;

		inline int get_difficulty_rating() {return difficulty_rating;}

		// set difficulty to 0 to produce the must difficult Sudokus.
		// If set to any value > 0 it strives to set that many numbers
		// e.g. if you call random_generate(50), you will have 50
		// numbers given.
		// Note that this works reliably for 35 < difficulty < 81
		// Values > 81 are forbidden.
		void random_generate(int difficulty);

		// WARNING: is very slowly, and prints any found Sudoku with
		// 19 or less clues. You can expect 5 19-clues-Sudokus per day
		// if you are lucky ;-)
		void generate_17();

		void add_random_number();

		void to_canonical_form();

		void minimize();

		bool has_uniq_solution();
	protected:

		// contains 0 for unset values and the corresponding value 
		// if the value is set
		char data[9][9];

		// allowed[x][y][i] is true if and only if it is possible to
		// set data[x][y] to i+1 without conjuring an immediate
		// collision.
		// if data[x][y] != 0 then allowed[x][y][i] is false for all i
		bool allowed[9][9][9]; 
		bool simple_solve();
		bool simple_solve1();
		bool simple_solve2();
		bool simple_solve3();
		bool simple_solve4();
		bool simple_solve5();
		bool backtrack();
		void null_init();
		void init();

		int recursion_depth;
		void set_recursion_depth(int rd) {recursion_depth = rd;};

		int solution_count;
		void print_mask(int n);

		int difficulty_rating;

		int count_entries();

		bool test_if_uniq;
	private:
		void perm_copy(char source[9][9], char* dest, int i, int j);
		void compare_and_update (char* least, char tmp[9][9], int i, int j);
};

#endif /* _MORITZ_FIELD */

