/**
 * Choose build flags here.
 */

#include "./lib/gen/mod.hpp"
#include "./cli/repl.hpp"

#if WINDOWS_ANSI
#include <windows.h>
#endif

#include <string>
#include <iostream> 	// cout,
#include <fstream>  	// ofstream,
#include <random>   	// random_device,

using solvent::cli::Repl;

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
const unsigned DEFAULT_ORDER = 0;

/**
 * ARGUMENTS
 * 0. executable name (fixed).
 * 1. grid order (default: 4).
 * 2. scramble random key (default: time).
 * 3. output file name.
 */
int main(const int argc, char const *const argv[]) {
	#if WINDOWS_ANSI
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode);
	#endif

	// My implementation specifies this as safe:
	std::ios_base::sync_with_stdio(false);

	unsigned int    user_order;      // 1
	unsigned int    srand_key;       // 2
	std::string     out_file_name;   // 3
	std::ostream*   os;              // 3

	user_order = (argc > 1) ? std::stoi(argv[1]) : DEFAULT_ORDER;
	if (argc > 2 && !std::string(argv[2]).empty()) {
		srand_key = std::stoi(argv[2]);
	} else {
		srand_key = std::random_device()();
	}
	if (argc > 3) {
		// TODO [feat] Handle if file already exists: prompt user for whether to overwrite.
		out_file_name = argv[3];
		os = new std::ofstream(out_file_name);
	} else {
		out_file_name = "std::cout";
		os = &std::cout;
	}

	std::cout << "\nPARSED ARGUMENTS:"
	<< "\n- ARG 1 [[ grid order  ]] : " << user_order
	<< "\n- ARG 2 [[ srand key   ]] : " << srand_key
	<< "\n- ARG 3 [[ output file ]] : " << out_file_name
	<< std::endl;

	// Scramble the random number generator (std::rand is no longer used):
	solvent::lib::gen::Rng.seed(srand_key);

	// Create a Solver of the specified order:
	// (It will automatically enter its REPL).
	switch (static_cast<solvent::Order>(user_order)) {
		case 3: { Repl<3> s(*os); break; }
		case 4: { Repl<4> s(*os); break; }
		case 5: { Repl<5> s(*os); break; }
		case 6: { Repl<6> s(*os); break; }
		//case 6: { Repl<6> s(*os); break; }
		default:
			std::cout << Ansi::RED.ON << "\nILLEGAL ARGUMENT:\n  order must be one of: { ";
			for (unsigned i = 3; i <= 6; i++) {
				std::cout << i << ", ";
			}
			std::cout << "}" << Ansi::RED.OFF << std::endl;
			break;
	}

	// End of program:
	std::cout << "\nbye bye!\n" << std::endl;
	return 0;
}
