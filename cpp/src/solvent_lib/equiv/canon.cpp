#include <solvent_lib/equiv/canon.hpp>
#include <solvent_lib/print.hpp>
#include <solvent_util/math.hpp>

#include <iostream>
#include <algorithm> // sort, ranges::sort, ranges::greater
#include <numeric>   // transform_reduce
#include <execution> // parallel_unsequenced_policy
#include <cmath>     // pow
#include <compare>

namespace solvent::lib::equiv {

	//
	template<Order O>
	class Canonicalizer final {
	 static_assert(O > 0 && O < MAX_REASONABLE_ORDER);
	 private:
		using ord1_t = typename size<O>::ord1_t;
		using ord2_t = typename size<O>::ord2_t;
		using ord4_t = typename size<O>::ord4_t;
		using ord5_t = typename size<O>::ord5_t;

		using grid_arr_t = typename std::array<std::array<ord2_t, O*O>, O*O>;
		using input_it_t = typename std::array<ord2_t, O*O>::const_iterator;

		struct AtomSlide final {
			std::array<ord5_t, O> slide_;
			static AtomSlide build(input_it_t atom_it);
			[[gnu::pure]] const ord5_t operator[](ord1_t i) const { return slide_[i]; }
			[[gnu::pure]] std::strong_ordering operator<=>(const AtomSlide& that) const;
			AtomSlide& operator+=(const AtomSlide& other) { for (ord1_t i = 0; i < O; i++) { slide_[i] += other.slide_[i]; } return *this; }
		};
		struct LineSlide final {
			ord1_t orig_blkline;
			std::array<AtomSlide, O> slide_;
			static LineSlide build(ord1_t orig_blkline, const std::array<ord2_t, O*O>& line_it);
			[[gnu::pure]] const AtomSlide& operator[](ord1_t i) const { return slide_[i]; }
			[[gnu::pure]] std::strong_ordering operator<=>(const LineSlide& that) const;
			LineSlide& operator+=(const LineSlide& other) { for (ord1_t i = 0; i < O; i++) { slide_[i] += other.slide_[i]; } return *this; }

		};
		struct ChuteSlide final {
			ord1_t orig_chute;
			std::array<LineSlide, O> slide_;
			static ChuteSlide build(ord1_t orig_chute, const grid_arr_t& chute_it);
			[[gnu::const]] const LineSlide& operator[](ord1_t i) const { return slide_[i]; }
			[[gnu::const]] std::strong_ordering operator<=>(const ChuteSlide& that) const;
			ChuteSlide& operator+=(const ChuteSlide& other) { for (ord1_t i = 0; i < O; i++) { slide_[i] += other.slide_[i]; } return *this; }
		};
		struct GridSlide final {
			std::array<ChuteSlide, O> slide_;
			static GridSlide build(const grid_arr_t& grid_it);
			[[gnu::const]] const ChuteSlide& operator[](ord1_t i) const { return slide_[i]; }
			[[gnu::const]] std::strong_ordering operator<=>(const GridSlide& that) const;
		};

	 public:
		static constexpr ord1_t O1 = O;
		static constexpr ord2_t O2 = O*O;
		static constexpr ord2_t O3 = O*O*O;
		static constexpr ord4_t O4 = O*O*O*O;
		// [[gnu::pure]] ord2_t operator[](ord4_t coord) const override;

		Canonicalizer(const grid_vec_t<O>&);

		grid_vec_t<O> operator()(void);

	 private:
		grid_mtx_t<O> input_;
		void relabel_(void) noexcept;
		void movement_(void);
	};


	template<Order O>
	grid_vec_t<O> canonicalize(const grid_vec_t<O>& input) {
		// TODO assert that input is the correct length and is a complete, valid sudoku?
		Canonicalizer<O> canon(input);
		return canon();
	}


	template<Order O>
	Canonicalizer<O>::Canonicalizer(const grid_vec_t<O>& input) {
		input_ = lib::grid_vec2mtx<O>(input);
	}


	template<Order O>
	grid_vec_t<O> Canonicalizer<O>::operator()(void) {
		relabel_();
		movement_();

		// const std::vector<print::print_grid_t> grid_accessors = {
		// 	print::print_grid_t([this](std::ostream& os, uint16_t coord) {
		// 		os << ' '; print::val2str(os, O, input_[coord/O2][coord%O2]);
		// 	}),
		// };
		// print::pretty(std::cout, O, grid_accessors);

		grid_vec_t<O> ret = grid_mtx2vec<O>(input_);
		return ret;
	}


	template<Order O>
	void Canonicalizer<O>::relabel_(void) noexcept {
		/*
		An entry at coordinate (A,B) contains the number of atoms in the
		grid where the values A and B coexist. Rephrase: the number of blocks
		in the grid where A and B are in the same atom.

		Choosing two labels A and B and swapping the corresponding rows and
		the corresponding columns produces the table generated when instances
		of A and B in `input_` are swapped.

		The probability of values A and B being in the same atom within a
		block is `p(n) = 2/(o+1)` (simplified from `2(o-1)/(o^2-1)`). The
		probability of coordinate (A,B) being n is given by a binomial
		distribution B(o^2, 2/(o+1)).

		- The diagonal is all zeroes.
		- Each row (or column) sums to O2*(2*(O1-1)).
		- The maximum possible value at any coordinate is O2.
		- The expected value at any non-diagonal coordinate is 2*O2/(O1+1).
			Simplified from (O2*(2*(O1-1)))/(O2-1). Using law of large numbers.
			Examples by order: 2: 2.67,  3: 4.50,  4: 6.40,  5: 8.33
			It can also be calculated in the standard way by using p(n).
		*/
		std::array<std::array<ord2_t, O2>, O2> counts = {{0}};

		for (ord2_t line = 0; line < O2; line++) {
			for (ord2_t atom = 0; atom < O2; atom += O1) {
				// Go through all unique pairs in the atom:
				for (ord1_t cell_i = 0; cell_i < O1 - 1; cell_i++) {
					for (ord1_t cell_j = cell_i + 1; cell_j < O1; cell_j++) {
						{ // boxrow
							const ord2_t label_i = input_[line][atom+cell_i], label_j = input_[line][atom+cell_j];
							counts[label_i][label_j]++; counts[label_j][label_i]++;
						}
						{ // boxcol
							const ord2_t label_i = input_[atom+cell_i][line], label_j = input_[atom+cell_j][line];
							counts[label_i][label_j]++; counts[label_j][label_i]++;
						}
					}
				}
			}
		}
		struct SortMapEntry final {
			ord2_t orig; // The original label value
			double sort_basis = 0; // Relabelling has no effect on this calculated value.
		};
		std::array<SortMapEntry, O2> canon2orig_label = {};
		// The reduction calculation's result must not depend on the ordering
		// of the counts entries. It should encapsulate a label's degree of
		// preference to being in an atom with some labels more than others.
		// The specific reduction below is the standard deviation.
		for (ord2_t label = 0; label < O2; label++) {
			static constexpr std::array<double, O2+1> std_dev = [](){
				std::array<double, O2+1> arr;
				for (unsigned count = 0; count < O2+1; count++) {
					static constexpr double expected = static_cast<double>(2*O2)/(O1+1);
					const double p_binomial = static_cast<double>(
						n_choose_r(O2, count) * std::pow(2, count) * std::pow(O1-1, O2-count)
					) / std::pow(O1+1, O2);
					arr[count] = p_binomial * std::pow(
						static_cast<double>(count) - expected,
					2);
				}
				return arr;
			}();
			const double sort_basis = std::transform_reduce(
				std::execution::par_unseq,
				counts[label].cbegin(), counts[label].cend(),
				0.0, std::plus<double>(), [](const ord2_t count) {
					return std_dev[count];
				}
			);
			canon2orig_label[label] = SortMapEntry {
				.orig {label},
				.sort_basis {sort_basis},
			};
			// Note: skipped taking the square root of .sort_basis (the standard
			// deviation), since doing it does not change the outcome of sorting.
		}
		// Make the lower-valued labels "play favourites":
		std::ranges::sort(canon2orig_label, std::ranges::greater(), &SortMapEntry::sort_basis);
		// std::cout << "\n"; for (auto e : canon2orig_label) { std::cout << e.sort_basis << "  "; }

		std::array<ord2_t, O2> label_map = {0};
		for (ord2_t i = 0; i < O2; i++) {
			label_map[canon2orig_label[i].orig] = i;
		}
		for (auto& row : input_) {
			for (auto& e : row) {
				e = label_map[e];
			}
		}
		// The below only done for debugging purposes.
		/* decltype(counts) counts_sorted;
		for (ord2_t i = 0; i < O2; i++) {
			for (ord2_t j = 0; j < O2; j++) {
				counts_sorted[i][j] = counts[canon2orig_label[i].orig][canon2orig_label[j].orig];
			}
			std::ranges::sort(counts_sorted[i]);
		}
		counts = counts_sorted; */
	}


	template<Order O>
	Canonicalizer<O>::AtomSlide Canonicalizer<O>::AtomSlide::build(input_it_t atom_it) {
		std::array<ord5_t, O> slide;
		for (ord1_t i = 0; i < O; i++) { slide[i] = *(atom_it + i); }
		std::sort(slide.begin(), slide.end()); // sort cells in atom
		for (ord1_t i = 1; i < O; i++) { slide[i] += slide[i-1]; }
		return AtomSlide { .slide_ = slide };
	}
	template<Order O>
	Canonicalizer<O>::LineSlide Canonicalizer<O>::LineSlide::build(const ord1_t orig_blkline, const std::array<ord2_t, O*O>& line) {
		std::array<AtomSlide, O> slide;
		for (ord1_t i = 0; i < O; i++) { slide[i] = AtomSlide::build(line.cbegin() + (O*i)); }
		std::sort(slide.begin(), slide.end()); // sort AtomSlides in line
		for (ord1_t i = 1; i < O; i++) { slide[i] += slide[i-1]; }
		return LineSlide { .orig_blkline = orig_blkline, .slide_ = slide };
	}
	template<Order O>
	Canonicalizer<O>::ChuteSlide Canonicalizer<O>::ChuteSlide::build(const ord1_t orig_chute, const grid_arr_t& grid) {
		std::array<LineSlide, O> slide;
		for (ord1_t i = 0; i < O; i++) { slide[i] = LineSlide::build(i, grid[(O*orig_chute)+i]); }
		std::sort(slide.begin(), slide.end()); // sort LineSlides in chute
		for (ord1_t i = 1; i < O; i++) { slide[i] += slide[i-1]; }
		return ChuteSlide { .orig_chute = orig_chute, .slide_ = slide };
	}
	template<Order O>
	Canonicalizer<O>::GridSlide Canonicalizer<O>::GridSlide::build(const grid_arr_t& grid) {
		std::array<ChuteSlide, O> slide;
		for (ord1_t i = 0; i < O; i++) { slide[i] = ChuteSlide::build(i, grid); }
		std::sort(slide.begin(), slide.end()); // sort ChuteSlides in grid
		for (ord1_t i = 1; i < O; i++) { slide[i] += slide[i-1]; }
		return GridSlide { .slide_ = slide };
	}


	template<Order O>
	std::strong_ordering Canonicalizer<O>::AtomSlide::operator<=>(const AtomSlide& that) const {
		ord1_t i = O; do { i--;
			if (auto const cmp = slide_[i] <=> that.slide_[i]; cmp != 0) [[likely]] { return cmp; }
		} while (i != 0);
		return std::strong_ordering::equal;
	}
	template<Order O>
	std::strong_ordering Canonicalizer<O>::LineSlide::operator<=>(const LineSlide& that) const {
		ord1_t i = O; do { i--;
			if (auto const cmp = slide_[i] <=> that.slide_[i]; cmp != 0) [[likely]] { return cmp; }
		} while (i != 0);
		return std::strong_ordering::equal;
	}
	template<Order O>
	std::strong_ordering Canonicalizer<O>::ChuteSlide::operator<=>(const ChuteSlide& that) const {
		ord1_t i = O; do { i--;
			if (auto const cmp = slide_[i] <=> that.slide_[i]; cmp != 0) [[likely]] { return cmp; }
		} while (i != 0);
		return std::strong_ordering::equal;
	}
	template<Order O>
	std::strong_ordering Canonicalizer<O>::GridSlide::operator<=>(const GridSlide& that) const {
		ord1_t i = O; do { i--;
			if (auto const cmp = slide_[i] <=> that.slide_[i]; cmp != 0) [[likely]] { return cmp; }
		} while (i != 0);
		return std::strong_ordering::equal;
	}


	// TODO test the tie-breaking strength of this implementation.
	// if it fails, look into using the same-atom-relationship table
	template<Order O>
	void Canonicalizer<O>::movement_() {
		const GridSlide grid_slide = GridSlide::build(input_);
		const GridSlide transposed_grid_slide = [this](){
			decltype(input_) transposed_input;
			for (ord2_t i = 0; i < O2; i++) {
				for (ord2_t j = 0; j < O2; j++) {
					transposed_input[i][j] = input_[j][i];
				}
			}
			return GridSlide::build(transposed_input);
		}();

		decltype(input_) canon_input = {O2};
		for (ord2_t canon_row = 0; canon_row < O2; canon_row++) {
			const auto& r_chute = grid_slide[canon_row/O1];
			const ord2_t orig_row = (O1*r_chute.orig_chute) + r_chute[canon_row%O1].orig_blkline;
			for (ord2_t canon_col = 0; canon_col < O2; canon_col++) {
				const auto& c_chute = transposed_grid_slide[canon_col/O1];
				const ord2_t orig_col = (O1*c_chute.orig_chute) + c_chute[canon_col%O1].orig_blkline;
				canon_input[canon_row][canon_col] = input_[orig_row][orig_col];
				// canon_input[orig_row][orig_col] = input_[canon_row][canon_col];
			}
		}
		if (transposed_grid_slide < grid_slide) {
			for (ord2_t i = 0; i < O2; i++) {
				for (ord2_t j = 0; j < O2; j++) {
					input_[i][j] = canon_input[j][i];
				}
			}
		} else {
			input_ = canon_input;
		}
	}


	#define SOLVENT_TEMPL_TEMPL(O_) \
		template grid_vec_t<O_> canonicalize<O_>(const grid_vec_t<O_>&); \
		template class Canonicalizer<O_>;
	SOLVENT_INSTANTIATE_ORDER_TEMPLATES
	#undef SOLVENT_TEMPL_TEMPL
}