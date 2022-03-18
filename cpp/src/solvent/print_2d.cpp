#include "solvent/print_2d.hpp"
#include "solvent_util/str.hpp"

#include <iostream>

namespace solvent {

	void print_2d(std::ostream& os, const Order O, const std::span<const print_2d_palette> grid_views) {
		namespace str = solvent::util::str;
		using ord2i_t = std::uint16_t;

		const auto print_blk_row_sep_string_ = [&os, O](const unsigned border_i) -> void {
			#define M_NOOK(NOOK_T, NOOK_C, NOOK_B) \
			if      (border_i == 0) [[unlikely]] { os << NOOK_T; } \
			else if (border_i == O) [[unlikely]] { os << NOOK_B; } \
			else                    { os << NOOK_C; }
			M_NOOK(" ┌", " ├", " └")
			for (unsigned blk_col {0}; blk_col < O; ++blk_col) {
				for (unsigned i {0}; i < 1u + (2u * O); ++i) {
					os << "─";
				}
				if (blk_col < O - 1u) { M_NOOK("┬", "┼", "┴") }
			}
			M_NOOK("┐", "┤", "┘")
			#undef M_NOOK
		};

		auto print_blk_row_sep_strings = [&](const unsigned border_i) mutable {
			os << '\n';
			print_blk_row_sep_string_(border_i);
			for (unsigned i {1}; i < grid_views.size(); ++i) {
				os << "   ";
				print_blk_row_sep_string_(border_i);
			}
		};

		os << str::dim.on;
		for (ord2i_t row {0}; row < O*O; ++row) {
			if (row % O == 0) {
				print_blk_row_sep_strings(row / O);
			}
			os << '\n';
			for (unsigned grid_i {0}; grid_i < grid_views.size(); ++grid_i) {
				for (ord2i_t col {0}; col < O*O; ++col) {
					if ((col % O) == 0) { os << str::dim.on << " │" << str::dim.off; }
					grid_views[grid_i](os, row * O*O + col);
				}
				os << str::dim.on << " │";
				if (grid_i != grid_views.size() - 1) {
					os << "   ";
				}
			}
		}
		print_blk_row_sep_strings(O);
		os << str::dim.off;
	}
}