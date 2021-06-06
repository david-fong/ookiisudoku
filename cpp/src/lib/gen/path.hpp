#ifndef HPP_SOLVENT_LIB_GEN_PATH
#define HPP_SOLVENT_LIB_GEN_PATH

#include ":/lib/size.hpp"

#include <iosfwd>
// #include <numeric> // iota
#include <array>
#include <string>

namespace solvent::lib::gen {

	namespace path {
		enum class Kind : unsigned {
			RowMajor,
			DealRwMj,
			BlockCol,
			__MAX__ = BlockCol,
		};
		constexpr size_t NUM_KINDS = static_cast<size_t>(Kind::__MAX__) + 1;
		// Indices of entries must match the
		// literal values_ of their respective enums.
		inline const std::array<std::string, NUM_KINDS> NAMES = {
			"rowmajor",
			"dealrwmj",
			"blockcol",
		};
		extern std::ostream& operator<<(std::ostream& os, Kind path_kind);
		inline const std::string OPTIONS_MENU = "\nGEN-PATH OPTIONS:"
			"\n- rowmajor   horizontal strips as wide as the grid one by one"
			"\n- dealrwmj   like dealing cards to each block using row-major"
			"\n- blockcol   rowmajor, but broken into columns one block wide";

		template<solvent::Order O>
		extern const std::array<typename size<O>::ord4_t (*const)(typename size<O>::ord4_t), NUM_KINDS> PathCoords;
	}
}
#endif