#include <okiidoku/morph/scramble.hpp>
#include <okiidoku/rng.hpp>

#include <array>
#include <algorithm> // shuffle
#include <cassert>

namespace okiidoku::morph {

	template<Order O>
	requires (is_order_compiled(O))
	Transformation<O> scramble(const grid_span_t<O> grid) {
		Transformation<O> t {};
		{
			std::lock_guard lock_guard_{shared_mt_rng_mutex_};
			std::ranges::shuffle(t.label_map, shared_mt_rng_);
			std::ranges::shuffle(t.row_map, shared_mt_rng_);
			std::ranges::shuffle(t.col_map, shared_mt_rng_);
			for (size_t chute {0}; chute < O; ++chute) {
				std::ranges::shuffle(t.row_map[chute], shared_mt_rng_);
				std::ranges::shuffle(t.col_map[chute], shared_mt_rng_);
			}
			// t.transpose = static_cast<bool>(shared_mt_rng_() % 2); // TODO
		}
		t.apply_in_place(grid);
		assert(grid_follows_rule<O>(grid));
		return t;
	}


	// Note: this is currently not used anywhere and has no explicit template expansions.
	template<class T>
	requires std::is_integral_v<T>
	void scramble(Order order, std::span<T> grid) {
		assert(is_order_compiled(order));
		assert(grid.size() >= order*order*order*order);
		switch (order) {
		#define M_OKIIDOKU_TEMPL_TEMPL(O_) \
			case O_: { \
				constexpr unsigned O4 = O_*O_*O_*O_; \
				using val_t = traits<O_>::o2i_smol_t; \
				std::array<val_t,O4> grid_resize; \
				for (unsigned i {0}; i < O4; ++i) { grid_resize[i] = static_cast<val_t>(grid[i]); } \
				scramble<O_>(std::span(grid_resize)); \
				for (unsigned i {0}; i < O4; ++i) { grid[i] = static_cast<T>(grid_resize[i]); } \
				break; \
			}
		M_OKIIDOKU_INSTANTIATE_ORDER_TEMPLATES
		#undef M_OKIIDOKU_TEMPL_TEMPL
		}
	}


	#define M_OKIIDOKU_TEMPL_TEMPL(O_) \
		template Transformation<O_> scramble<O_>(const grid_span_t<O_>);
	M_OKIIDOKU_INSTANTIATE_ORDER_TEMPLATES
	#undef M_OKIIDOKU_TEMPL_TEMPL
}