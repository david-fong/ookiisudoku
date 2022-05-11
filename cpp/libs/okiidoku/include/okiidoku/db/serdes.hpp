#ifndef HPP_OKIIDOKU__DB__SERDES
#define HPP_OKIIDOKU__DB__SERDES

#include <okiidoku/grid.hpp>
#include <okiidoku/detail/order_templates.hpp>

#include <iosfwd>

/**
These are low-level serialization utilities. You likely want the
provided database abstraction instead. These utilities are probably
only useful to you if you want to do data-streaming over a network.
*/
namespace okiidoku::mono::db::serdes {

	constexpr unsigned get_min_bytes_to_store(const unsigned max_value) {
		return (std::bit_width(max_value) + 7) / 8;
	}

	// contract: the grid is filled and follows the one rule.
	template<Order O>
	OKIIDOKU_EXPORT void print_filled(std::ostream& os, const Grid<O>& src) noexcept;

	// contract: the stream contains the result of a call to print_filled.
	template<Order O>
	OKIIDOKU_EXPORT void parse_filled(std::istream& is, Grid<O>& sink) noexcept;

	// best used with sparse puzzles.
	template<Order O>
	OKIIDOKU_EXPORT void print_puzzle(std::ostream& os, const Grid<O>& src) noexcept;

	// contract: the stream contains the result of a call to print_puzzle.
	template<Order O>
	OKIIDOKU_EXPORT void parse_puzzle(std::istream& is, Grid<O>& sink) noexcept;
}


namespace okiidoku::visitor::db::serdes {

	constexpr unsigned get_min_bytes_to_store(const unsigned max_value) {
		return (std::bit_width(max_value) + 7) / 8;
	}

	// contract: the grid is filled and follows the one rule.
	OKIIDOKU_EXPORT void print_filled(std::ostream& os, const Grid& src) noexcept;

	// contract: the stream contains the result of a call to print_filled.
	OKIIDOKU_EXPORT void parse_filled(std::istream& is, Grid& sink) noexcept;

	// best used with sparse puzzles.
	OKIIDOKU_EXPORT void print_puzzle(std::ostream& os, const Grid& src) noexcept;

	// contract: the stream contains the result of a call to print_puzzle.
	OKIIDOKU_EXPORT void parse_puzzle(std::istream& is, Grid& sink) noexcept;
}
#endif