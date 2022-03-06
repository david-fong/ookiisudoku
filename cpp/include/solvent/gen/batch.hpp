#ifndef HPP_SOLVENT__GEN__BATCH
#define HPP_SOLVENT__GEN__BATCH

#include "solvent/gen/mod.hpp"
#include "solvent_util/timer.hpp"
#include "solvent_export.h"

#include <iosfwd>
#include <functional>
#include <vector>
#include <optional>

namespace solvent::lib::gen::batch {

	using trials_t = unsigned long;

	//
	struct SOLVENT_EXPORT Params final {
		gen::Params gen_params;
		unsigned num_threads {0}; // Defaulted if zero.
		unsigned max_dead_end_sample_granularity {0}; // Defaulted if zero.
		bool only_count_oks;
		trials_t stop_after;

		// Cleans self and returns a copy of self.
		Params clean(Order O) noexcept;
	};


	//
	struct SOLVENT_EXPORT BatchReport final {
		trials_t total_anys {0};
		trials_t total_oks {0};
		double fraction_aborted;

		util::Timer timer;
		util::Timer::Elapsed time_elapsed;

		struct MaxDeadEndSample final {
			unsigned long max_dead_ends;
			trials_t marginal_oks;
			double marginal_ops;
			std::optional<double> marginal_average_ops; // marginal_ops / marginal_oks. null if no oks.
			std::optional<double> net_average_ops; // (accumulated marginal_ops) / (accumulated marginal_oks). null if no oks.
		};
		static constexpr unsigned sample_granularity_default {20u};
		static constexpr unsigned sample_granularity_max {50u};
		static constexpr unsigned recommended_oks_per_sample {20u};

		// Data sampled. Each entry showing the outcome if its max_dead_ends
		// value was used.
		std::vector<MaxDeadEndSample> max_dead_end_samples;
		unsigned max_dead_end_samples_best_i {0u};

		void print(std::ostream&, Order O) const;
	};


	constexpr unsigned try_default_num_extra_threads_(const Order O) {
		if (O < 4) { return 0; }
		else if (O == 4) { return 1; }
		else { return 2; }
	};
	unsigned default_num_threads(Order O);


	template<Order O>
	using callback_O_t = std::function<void(const GeneratorO<O>&)>;
	// calls to the callback will be guarded by a mutex.
	template<Order O>
	SOLVENT_EXPORT [[nodiscard]] BatchReport batch_O(Params&, callback_O_t<O>);


	using callback_t = std::function<void(const Generator&)>;
	// calls to the callback will be guarded by a mutex.
	// asserts that the order is compiled.
	SOLVENT_EXPORT [[nodiscard]] BatchReport batch(Order, Params&, callback_t);


	#define M_SOLVENT_TEMPL_TEMPL(O_) \
		extern template BatchReport batch_O<O_>(Params&, callback_O_t<O_>);
	M_SOLVENT_INSTANTIATE_ORDER_TEMPLATES
	#undef M_SOLVENT_TEMPL_TEMPL

// inline void batch() {} // can we do something like this?
}
#endif