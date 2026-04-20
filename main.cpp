#define VERSION_STRING "0.4.7.3"
// Yass: Yat Another Stupid Sudoku Solver
//
// Written by Moritz Lenz <moritz.lenz@gmail.com>
//
// This program is free software. You may redistribute and change it under the
// terms of the GNU General Public License Version 2 (June 1991) as published
// by the Free Software Fundation, Inc., 59 Temple Place, Suite 330, Boston,
// MA 02111-1307 USA
// or under the terms of the BSD Artistic License.
// Choose the one you like better.
//
// Yasss solves the sudoku game.
// It simply reads fields from standard input one per line, with any character
// other then 1..9 treated as an empty cell. If the line is shorter than 81
// characters, all the missing numbers are treated as empty.
// It solves it (if possible), and prints the solved version in the same
// format.
//
#include <iostream>
#include "sudoku.hpp"
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <getopt.h>


using namespace std;

void help(char* program_name) {
	cout << "Yasss " << VERSION_STRING << "\n"
	<< "Usage:\n"
	<< program_name << " [-hVacCmsSu]\n"
	<< program_name << " --help\n"
	<< program_name << " --version\n"
	<< program_name << " --generate[=num]\n"
	<< "\nOptions:\n"
	<< "\t-a|--answer\tPrints out the solved Sudoku\n"
	<< "\t-c|--count\tPrints the number of solutions to a given Sudoku\n"
	<< "\t-C|--canonical\tTransforms to a canonical form\n"
	<< "\t-g|--generate[=num]\tGenerates num (default 1) Sudokus\n"
	<< "\t-m|--minimize\tMinimalizes the given Sudoku\n"
	<< "\t-r|--random[=num]\tGenerates num (default 1) Sudokus with a random number of clues\n"
	<< "\t-s|--score\tPrints out a difficulty rating (score)\n"
	<< "\t-S|--svg\tPrints all output Sudokus as SVG\n"
	<< "\t-u|--uniq\tTests if the Sudoku has a uniq solution\n"
	<< "\n"
	"Yasss reads Sudokus from STDIN (one per line) and prints out the solution\n"
	<< "unless one of the Options -c or -s is given.\n"
	<< "Option --answer|-a forces the solved Sudoku to be printed.\n"
	<< "If option -g or -r is given, all other options are ignored.\n"
	<< "if option --canonical is given it is applied before all other Options\n"
	;

}

void random_generate(int count){
	for (int i = 0; i < count; i++){
		sudoku a;
		int clues = 17 + random() % (80 - 17);
		a.random_generate(clues);
		a.print(cout);
	}
}

void generate(int count, bool as_svg){
	for (int i = 0; i < count; i++){
		sudoku a;
		a.random_generate(58);
//		a.minimize();
		if (as_svg){
			a.svg_print(cout);
		} else {
			a.print(cout);
		}
	}
}

void generate_17(void){
	sudoku a;
	a.generate_17();
}

int main(int argc, char** argv){
	// init random number generator with microseconds since begin
	// of the epoch:
	timeval tv;
	gettimeofday(&tv, NULL);

	srand(1000000*tv.tv_sec + tv.tv_usec);

	// parse command line options:
	static struct option long_options[] = {
		{"help", no_argument, 0, 0},
		{"version", no_argument, 0, 2},
		{"count", no_argument, 0, 3},
		{"score", no_argument, 0, 4},
		{"answer", no_argument, 0, 5},
		{"generate", optional_argument, 0, 6},
		{"canonical", no_argument, 0, 7},
		{"17", no_argument, 0, 8},
		{"minimize", no_argument, 0, 9},
		{"uniq", no_argument, 0, 10},
		{"svg", no_argument, 0, 11},
		{"random", optional_argument, 0, 12},
		{0, 0, 0, 0}
	};
	int option_result = 0;
	int option_index = 0;
	bool print_solution_count = false;
	bool print_score = false;
	bool print_solution = false;
	bool print_canonical = false;
	bool minimize = false;
	bool test_if_uniq = false;
	bool print_as_svg = false;
	bool do_generate = false;
	int count = 1;
	while (true){
		option_result = getopt_long(argc, argv, "hVcr::sSamug::C",
				long_options, &option_index);
		if (option_result == -1){
			break;
		}
		switch (option_result){
			case 'h':
			case 0:
				help(argv[0]);
				exit(0);
			case 'V':
			case 2:
				cout << "yasss " << VERSION_STRING << "\n";
				exit(0);
			case 'c':
			case 3: 
				print_solution_count = true;
				break;
			case 's':
			case 4:
				print_score = true;
				break;
			case 'a':
			case 5:
				print_solution = true;
				break;
			case 'g':
			case 6:
				if (optarg){
					count = atoi(optarg);
				}
				do_generate = true;
				break;
			case 'C':
			case 7:
				print_canonical = true;
				break;
			case 8:
				generate_17();
				exit(0);
				break;
			case 'm':
			case 9:
				minimize = true;
				break;
			case 'u':
			case 10:
				test_if_uniq = true;
				break;
			case 'S':
			case 11:
				print_as_svg = true;
				break;
			case 'r':
			case 12:
				if (optarg){
					count = atoi(optarg);
				}
				random_generate(count);
				exit(0);

		}

	}

	if (do_generate){
		generate(count, print_as_svg);
		exit(0);
	}

	if (optind < argc){
		cerr << argv[0] << ": Warning: ignoring additional command line arguments\n";
	}
	


	int f[9][9];
	char buffer[255];
	while (cin.getline(buffer, 255)){
		bool msg = true;
		for (int y = 0; y < 9; y++){
			for (int x = 0; x < 9; x++){
				int index = x + 9*y;
				if (buffer[index] == '\0' && msg){
					cerr << "Premature end of input, padding with zeros...\n";
					msg = false;
				}
				int val = buffer[index] - '0';
				if (val > 0 && val <= 9 && msg){
					f[x][y] = val;
				} else{
					f[x][y] = 0;
				}
			}
		}

		sudoku a(f);
		if (print_canonical){
			a.to_canonical_form();
			if (print_as_svg){
				a.svg_print(cout);
			} else {
				a.print(cout);
			}
		}

		if (minimize) {
			a.minimize();
			if (print_as_svg){
				a.svg_print(cout);
			} else {
				a.print(cout);
			}
		}

		if (test_if_uniq){
			if (a.has_uniq_solution()){
				cout << "The Sudoku has one uniq solution\n";
			} else {
				cout << "The Sudoku is ambigous, i.e. it has multiple solutions\n";
			}
		}


		a.count_solutions = print_solution_count;
		a.calculate_difficulty_rating = print_score;
		a.solve();
		if (print_solution_count){
			cout << "Number of solutions: " 
				<< a.get_solution_count() << "\n";
		} 			
		if (print_score){
			cout << "Score: " << a.get_difficulty_rating() 
				<< "\n";
		}
		
		if (print_solution || (!print_score && !print_solution_count
					&& !print_canonical && !minimize 
					&& !test_if_uniq)){
			if (print_as_svg){
				a.svg_print(cout);
			} else {
				a.print(cout);
			}
		}

	}
	return 0;
}

