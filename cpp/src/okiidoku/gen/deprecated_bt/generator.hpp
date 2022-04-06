#ifndef HPP_OKIIDOKU__GEN__BT__GENERATOR
#define HPP_OKIIDOKU__GEN__BT__GENERATOR

#include <okiidoku/gen/deprecated_bt/path.hpp>
#include <okiidoku/grid.hpp>
#include <okiidoku/traits.hpp>
#include <okiidoku/okiidoku_config.hpp>
#include <okiidoku_export.h>

#include <array>
#include <span>
#include <memory> // unique_ptr
#include <cassert>

namespace okiidoku::gen::bt {

	// used for manual branch likelihood profiling
	// extern long long total;
	// extern long long true_;

	namespace cell_dead_ends {
		// TODO.low these values are tuned for genpath=row_major. make one for each genpath? :/
		constexpr unsigned long long limit_default[]{ 0, 0, 3,
			/*3*/30,
			/*4*/700,
			/*5*/100'000, // changing to anything between this and 100K doesn't seem to have any significant difference? I only tested with gen_ok 20 though.
			/*6*/10'000'000ull, // <- not tested AT ALL ...
			/*7*/10'000'000'000ull, // <- not tested AT ALL ...
		};
		// allows up to and including
		constexpr unsigned long long limit_i_max[]{ 0, 0, 3,
			/*3*/1'000,
			/*4*/10'000,
			/*5*/100'000'000ull, //  <- not tested AT ALL ...
			/*6*/10'000'000'000ull, // <- not tested AT ALL ...
			/*7*/1'000'000'000'000ull, // <- not tested AT ALL ...
		};
		template<Order O>
		using t = uint_smolN_t<std::bit_width(limit_i_max[O])>;
	};


	//
	struct OKIIDOKU_EXPORT Params final {
		path::E path_kind {path::E::row_major};
		std::uint_fast64_t max_dead_ends {0}; // Defaulted if zero.

		// Cleans self and returns a copy of self.
		Params clean(Order O) noexcept;
	};

	//
	enum class OKIIDOKU_EXPORT ExitStatus : std::uint8_t {
		ok, abort, exhausted,
	};

	// Container for a very large number.
	// number of operations taken to generate a solution by grid-order.
	using opcount_t = unsigned long long;


	class OKIIDOKU_EXPORT Generator {
	public:
		using val_t = traits<O_MAX>::o2i_t;
		using coord_t = traits<O_MAX>::o4x_t;
		using dead_ends_t = float; // TODO.mid is this okay?
		using backtrack_origin_t = traits<O_MAX>::o4x_smol_t;

		// contract: GeneratorO<O> is compiled
		static std::unique_ptr<Generator> create(Order O);

		virtual void operator()(Params) = 0;
		// contract: must not be called before a call to `operator()`.
		virtual void continue_prev() = 0;

		[[nodiscard]] virtual Order get_order() const noexcept = 0;
		[[nodiscard]] constexpr Order get_order2() const noexcept { return get_order()*get_order(); }
		[[nodiscard]] constexpr Order get_order4() const noexcept { return get_order2()*get_order2(); }
		[[nodiscard]] virtual const Params& get_params() const noexcept = 0;
		// contract: must not be called before a call to `operator()`.
		[[nodiscard]] virtual ExitStatus status() const noexcept = 0;
		[[nodiscard]] virtual bool status_is_ok() const noexcept = 0;
		[[nodiscard]] virtual backtrack_origin_t get_backtrack_origin() const noexcept = 0;
		[[nodiscard]] virtual dead_ends_t get_most_dead_ends_seen() const noexcept = 0;
		[[nodiscard]] virtual opcount_t get_op_count() const noexcept = 0;

		// these are optimized for reading out specific entries _once_- not for
		// repeated calls with the same coord. For that, use write_to or to_vec.
		[[nodiscard]] virtual val_t extract_val_at(coord_t) const noexcept = 0;
		[[nodiscard]] virtual dead_ends_t extract_dead_ends_at(coord_t) const noexcept = 0;

		// contract: `sink.size() >= O4`.
		// this cannot statically check that T is wide enough. uses static_cast<T>.
		// ie. it is your job to make sure T does not lose precision.
		template<class T>
		requires std::is_integral_v<T> && (!std::is_const_v<T>)
		void write_to(std::span<T> sink) const noexcept {
			const unsigned O4 = get_order4();
			assert(sink.size() >= O4);
			for (unsigned i {0}; i < O4; ++i) { sink[i] = static_cast<T>(extract_val_at(i)); }
		}

		// contract: `sink.size() >= O4`.
		// this cannot statically check that T is wide enough. uses static_cast<T>.
		// ie. it is your job to make sure T does not lose precision.
		template<class T>
		requires std::is_arithmetic_v<T> && (!std::is_const_v<T>)
		void write_dead_ends_to(std::span<T> sink) const noexcept {
			const unsigned O4 = get_order4();
			assert(sink.size() >= O4);
			for (unsigned i {0}; i < O4; ++i) { sink[i] = static_cast<T>(extract_dead_ends_at(i)); }
		}
	};


	//
	template<Order O>
	requires (is_order_compiled(O) /* && (O < 6) */) // added <6 restriction for sanity
	class OKIIDOKU_EXPORT GeneratorO final : public Generator {
	public:
		using has_mask_t = traits<O>::o2_bits_smol; // perf seemed similar and slightly better compared to fast_t
		using o1i_t = traits<O>::o1i_t;
		using o2x_t = traits<O>::o2x_t;
		using o2i_t = traits<O>::o2i_t;
		using o4x_t = traits<O>::o4x_t;
		using o4i_t = traits<O>::o4i_t;
		using dead_ends_t = cell_dead_ends::t<O>;

		OKIIDOKU_NO_EXPORT static constexpr o1i_t O1 = O;
		OKIIDOKU_NO_EXPORT static constexpr o2i_t O2 = O*O;
		OKIIDOKU_NO_EXPORT static constexpr o4i_t O4 = O*O*O*O;

		void operator()(Params) override;
		void continue_prev() override;

		[[nodiscard]] constexpr Order get_order() const noexcept { return O; }
		[[nodiscard]] constexpr const Params& get_params() const noexcept { return params_; }
		[[nodiscard]] constexpr ExitStatus status() const noexcept {
			switch (progress_) {
				case O4-1: return ExitStatus::ok;
				case O4:   return ExitStatus::exhausted;
				default:   return ExitStatus::abort;
			}
		}
		[[nodiscard]] constexpr bool status_is_ok() const noexcept { return progress_ == O4-1; }
		[[nodiscard]] constexpr Generator::backtrack_origin_t get_backtrack_origin() const noexcept { return static_cast<Generator::backtrack_origin_t>(backtrack_origin_); }
		[[nodiscard]] constexpr Generator::dead_ends_t get_most_dead_ends_seen() const noexcept { return static_cast<Generator::dead_ends_t>(most_dead_ends_seen_); }
		[[nodiscard]] constexpr opcount_t get_op_count() const noexcept { return op_count_; }
		[[nodiscard, gnu::hot]] Generator::val_t extract_val_at(Generator::coord_t coord) const noexcept { return static_cast<Generator::val_t>(extract_val_at_(static_cast<o4x_t>(coord))); }
		[[nodiscard]] Generator::dead_ends_t extract_dead_ends_at(Generator::coord_t coord) const noexcept { return static_cast<Generator::dead_ends_t>(extract_dead_ends_at_(static_cast<o4x_t>(coord))); }

		[[nodiscard]] constexpr const auto& get_backtrack_origin_() const noexcept { return backtrack_origin_; }
		[[nodiscard]] constexpr const auto& get_most_dead_ends_seen_() const noexcept { return most_dead_ends_seen_; }
		[[nodiscard, gnu::hot]] o2i_t extract_val_at_(o4x_t coord) const noexcept;
		[[nodiscard]] dead_ends_t extract_dead_ends_at_(o4x_t coord) const noexcept;

		template<class T>
		requires std::is_integral_v<T> && (!std::is_const_v<T>) && (sizeof(T) >= sizeof(o2i_t))
		void write_to_(std::span<T, O4> sink) const {
			assert(sink.size() >= O4);
			for (o4i_t i {0}; i < O4; ++i) { sink[i] = extract_val_at_(i); }
		}

	private:
		struct Cell final {
			o2i_t try_index; // Index into val_try_orders_. O2 if clear.
			OKIIDOKU_NO_EXPORT void clear() noexcept { try_index = O2; }
			OKIIDOKU_NO_EXPORT [[nodiscard, gnu::pure]] bool is_clear() const noexcept { return try_index == O2; }
		};
		struct OKIIDOKU_NO_EXPORT Direction final {
			bool is_back;
			bool is_back_skip; // only meaningful when is_back is true.
		};
		Params params_;
		o4i_t progress_ {0};

		// indexed by `progress_ // O2`
		grid_arr2d_t<O, o2x_t> val_try_orders_ {[]{
			grid_arr2d_t<O, o2x_t> _;
			for (auto& vto : _) { for (o2i_t i {0}; i < O2; ++i) { vto[i] = i; } }
			return _;
		}()};

		std::array<Cell, O4> cells_; // indexed by progress_
		std::array<has_mask_t, O2> rows_has_;
		std::array<has_mask_t, O2> cols_has_;
		std::array<has_mask_t, O2> blks_has_;
		std::array<dead_ends_t, O4> dead_ends_; // indexed by progress_

		uint_fastN_t<std::bit_width(O4)> backtrack_origin_ {0};
		dead_ends_t most_dead_ends_seen_ {0};
		opcount_t op_count_ {0};

		OKIIDOKU_NO_EXPORT [[gnu::hot]] void generate_();
		OKIIDOKU_NO_EXPORT [[gnu::hot]] Direction set_next_valid_(path::coord_converter<O>, bool backtracked) noexcept;
	};


	#define M_OKIIDOKU_TEMPL_TEMPL(O_) \
		extern template class GeneratorO<O_>;
	M_OKIIDOKU_INSTANTIATE_ORDER_TEMPLATES
	#undef M_OKIIDOKU_TEMPL_TEMPL
}
#endif