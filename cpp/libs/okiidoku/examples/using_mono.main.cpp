#include <okiidoku/morph/canon.hpp>
#include <okiidoku/morph/scramble.hpp>
#include <okiidoku/serdes.hpp>
#include <okiidoku/print_2d.hpp>
#include <okiidoku/gen.hpp>
#include <okiidoku/grid.hpp>
#include <okiidoku/shared_rng.hpp>

#include <okiidoku_cli_utils/console_setup.hpp>

#include <iostream>  // cout,
#include <random>    // random_device,
#include <array>

// Strong recommendation: do NOT do `using namespace okiidoku;` anywhere.
// Strong recommendation: do NOT use C++'s ADL mechanism to call non-operator
// functions defined in the okiidoku namespace in an unqualified manner.
// You will probably not have a good time if you do any of those things.

// "copy" names under `okiidoku::mono` to okiidoku:
// Recommendation: do NOT do this in header files (only in .cpp files)
namespace okiidoku { using namespace okiidoku::mono; }

int main([[maybe_unused]] const int argc, [[maybe_unused]] char const *const argv[]) {
	namespace oki = okiidoku;
	oki::util::setup_console();
	oki::SharedRng shared_rng;
	shared_rng.rng.seed(std::random_device()());

	constexpr oki::Order O {3};

	oki::Grid<O> gen_grid;
	oki::generate(gen_grid, shared_rng);
	oki::Grid<O> puz_grid;
	// TODO.wait oki::puzzle::make_hard(gen_grid, puz_of_gen_grid);
	auto canon_transform_for_gen_grid {oki::canonicalize(gen_grid)};
	canon_transform_for_gen_grid.apply_in_place(puz_grid);

	oki::Grid<O> canon_grid {gen_grid};
	oki::scramble(canon_grid, shared_rng);
	oki::canonicalize(canon_grid);

	if (gen_grid != canon_grid) {
		// TODO.wait update this example comment when canonicalize works
		// this should never happen (once the library is correctly implemented)
		std::abort();
	}

	{
		const auto palette_ {std::to_array<oki::print_2d_grid_view>({
			[&](auto rmi){ return   gen_grid.at_rmi(rmi); },
			[&](auto rmi){ return canon_grid.at_rmi(rmi); },
		})};
		oki::print_2d(std::clog, O, palette_, shared_rng);
	}

	return 0;
}