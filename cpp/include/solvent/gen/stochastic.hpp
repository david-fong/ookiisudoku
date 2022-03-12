#ifndef HPP_SOLVENT__GEN__STOCHASTIC
#define HPP_SOLVENT__GEN__STOCHASTIC

#include "solvent/grid.hpp"
#include "solvent/size.hpp"
#include "solvent_config.hpp"
#include "solvent_export.h"

#include <random>  // minstd_rand
#include <array>
#include <span>
#include <memory>  // unique_ptr
#include <cassert>

namespace solvent::gen::ss {

	// used for manual branch likelihood profiling
	// SOLVENT_EXPORT extern unsigned long long total;
	// SOLVENT_EXPORT extern unsigned long long true_;

	enum class SOLVENT_EXPORT ExitStatus : std::uint8_t {
		Ok, Abort, // Exhausted,
	};


	class SOLVENT_EXPORT Generator {
	public:
		using val_t = size<O_MAX>::ord2i_t;
		using coord_t = size<O_MAX>::ord4x_t;

		// contract: GeneratorO<O> is compiled
		static std::unique_ptr<Generator> create(Order O);

		virtual void operator()() = 0;
		// contract: must not be called before a call to `operator()`.
		virtual void continue_prev() = 0;

		[[nodiscard]] virtual Order get_order() const noexcept = 0;
		[[nodiscard]] constexpr Order get_order2() const noexcept { return get_order()*get_order(); }
		[[nodiscard]] constexpr Order get_order4() const noexcept { return get_order2()*get_order2(); }
		// contract: must not be called before a call to `operator()`.
		[[nodiscard]] virtual ExitStatus status() const noexcept = 0;
		[[nodiscard]] virtual bool status_is_ok() const noexcept = 0;

		[[nodiscard]] virtual val_t get_val_at(coord_t) const noexcept = 0;

		// this cannot statically check that T is wide enough. uses static_cast<T>.
		// ie. it is your job to make sure T does not lose precision.
		template<class T>
		requires std::is_integral_v<T> && (!std::is_const_v<T>)
		void write_to(std::span<T> sink) const {
			const unsigned O4 = get_order4();
			assert(sink.size() >= O4);
			for (unsigned i {0}; i < O4; ++i) { sink[i] = static_cast<T>(get_val_at(i)); }
		}
	};


	//
	template<Order O>
	requires ((O > 0) && (O <= O_MAX) && (O < 6)) // added <6 restriction for sanity
	class SOLVENT_EXPORT GeneratorO final : public Generator {
	public:
		using ord1i_t = size<O>::ord1i_t;
		using ord2x_t = size<O>::ord2x_t;
		using ord2i_t = size<O>::ord2i_t;
		using ord4x_t = size<O>::ord4x_t;
		using ord4i_t = size<O>::ord4i_t;

		SOLVENT_NO_EXPORT static constexpr ord1i_t O1 = O;
		SOLVENT_NO_EXPORT static constexpr ord2i_t O2 = O*O;
		SOLVENT_NO_EXPORT static constexpr ord4i_t O4 = O*O*O*O;

		using has_count_t = std::array<ord2i_t, O2>;

		void operator()() override;
		void continue_prev() override;

		[[nodiscard]] constexpr Order get_order() const noexcept { return O; }
		[[nodiscard]] constexpr ExitStatus status() const noexcept {
			if (is_done_) {
				return ExitStatus::Ok;
			} else {
				return ExitStatus::Abort;
			}
		}
		[[nodiscard]] constexpr bool status_is_ok() const noexcept {
			return is_done_;
		}
		[[nodiscard, gnu::hot]] Generator::val_t get_val_at(Generator::coord_t coord) const noexcept { return static_cast<Generator::val_t>(get_val_at_(static_cast<ord4x_t>(coord))); }

		[[nodiscard, gnu::hot]] ord2i_t get_val_at_(ord4x_t coord) const noexcept;

		template<class T>
		requires std::is_integral_v<T> && (!std::is_const_v<T>) && (sizeof(T) >= sizeof(ord2i_t))
		void write_to_(std::span<T, O4> sink) const {
			assert(sink.size() >= O4);
			for (ord4i_t i {0}; i < O4; ++i) { sink[i] = get_val_at_(i); }
		}

	private:
		std::array<std::array<ord2x_t, O2>, O2> cells_ {[]{
			std::array<std::array<ord2x_t, O2>, O2> _;
			for (auto& vto : _) { for (ord2i_t i {0}; i < O2; ++i) { vto[i] = i; } }
			return _;
		}()};

		std::minstd_rand rng_; // other good LCG parameters https://arxiv.org/pdf/2001.05304v3.pdf
		// TODO using mt19937_64 isn't much slower... could it be okay to use?
		bool is_done_;

		SOLVENT_NO_EXPORT [[gnu::hot]] void generate_();
	};


	#define M_SOLVENT_TEMPL_TEMPL(O_) \
		extern template class GeneratorO<O_>;
	M_SOLVENT_INSTANTIATE_ORDER_TEMPLATES
	#undef M_SOLVENT_TEMPL_TEMPL
}
#endif