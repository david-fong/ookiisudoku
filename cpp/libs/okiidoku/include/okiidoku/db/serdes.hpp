#ifndef HPP_OKIIDOKU__DB__SERDES
#define HPP_OKIIDOKU__DB__SERDES

#include <okiidoku/grid.hpp>
#include <okiidoku/order_templates.hpp>

#include <iosfwd>

namespace okiidoku::mono::db::serdes {

	constexpr unsigned get_min_bytes_to_store(const unsigned max_value) {
		return (std::bit_width(max_value) + 7) / 8;
	}

	// contract: the grid is filled.
	template<Order O>
	OKIIDOKU_EXPORT void print_filled(std::ostream& os, GridConstSpan<O> grid_view);

	// contract: the grid is filled.
	template<Order O>
	OKIIDOKU_EXPORT void parse_filled(std::istream& is, GridSpan<O> grid_view);

	// best used with sparse puzzles.
	template<Order O>
	OKIIDOKU_EXPORT void print_puzzle(std::ostream& os, GridConstSpan<O> grid_view);

	// best used with sparse puzzles.
	template<Order O>
	OKIIDOKU_EXPORT void parse_puzzle(std::istream& is, GridSpan<O> grid_view);
}


namespace okiidoku::visitor::db::serdes {

	constexpr unsigned get_min_bytes_to_store(const unsigned max_value) {
		return (std::bit_width(max_value) + 7) / 8;
	}

	// contract: the grid is filled.
	OKIIDOKU_EXPORT void print_filled(std::ostream& os, GridConstSpan grid_view);

	// contract: the grid is filled.
	OKIIDOKU_EXPORT void parse_filled(std::istream& is, GridSpan grid_view);

	// best used with sparse puzzles.
	OKIIDOKU_EXPORT void print_puzzle(std::ostream& os, GridConstSpan grid_view);

	// best used with sparse puzzles.
	OKIIDOKU_EXPORT void parse_puzzle(std::istream& is, GridSpan grid_view);
}
#endif