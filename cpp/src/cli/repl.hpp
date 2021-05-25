#ifndef HPP_SOLVENT_CLI_REPL
#define HPP_SOLVENT_CLI_REPL

#include "./trials.hpp"
#include "./enum.hpp"
#include "../lib/gen/mod.hpp"
#include "../lib/size.hpp"

#include <map>
#include <string>
#include <array>


namespace solvent::cli {

	const std::string PROMPT = "\n$ ";

	namespace Command {
		enum class E : unsigned {
			Help,
			Quit,
			ConfigVerbosity,
			ConfigGenPath,
			RunSingle,
			ContinuePrev,
			RunMultiple,
			RunMultipleOk,
		};
		const std::map<std::string, Command::E> Str2Enum = {
			{ "help",       E::Help            },
			{ "quit",       E::Quit            },
			{ "output",     E::ConfigVerbosity },
			{ "genpath",    E::ConfigGenPath   },
			{ "",           E::RunSingle       },
			{ "cont",       E::ContinuePrev    }, // TODO [qol] Change this to "c"? (remember to change help string too)
			{ "trials",     E::RunMultiple     },
			{ "strials",    E::RunMultipleOk   },
		};
		const std::string HelpMessage = "\nCOMMAND MENU:"
			"\n- help               print this help menu"
			"\n- quit               cleanly exit this program"
			"\n- output [<level>]   get / set output level"
			"\n- genpath [<path>]   get / set generator traversal path"
			"\n- {enter}            generate a single solution"
			"\n- cont               continue previous generation"
			"\n- trials <n>         attempt to generate <n> solutions"
			"\n- strials <n>        successfully generate <n> solutions";
	}

	// Returns zero on error.
	unsigned GET_TERM_COLS(void) noexcept {
		char const*const envVar = std::getenv("COLUMNS");
		return (envVar != NULL) ? std::stoul(envVar) : 0u;
	}


	/**
	 *
	 * Notes for me as I learn how to write inheritance in C++:
	 * I can specify base-class members like:
	 * - Derived::Base::member
	 *   https://en.cppreference.com/w/cpp/language/injected-class-name
	 * - this.Base->member
	 * - this->member
	 * - Base<ARGS>::member
	 */
	template<Order O>
	class Repl final {
	 public:
		using opcount_t = lib::gen::opcount_t;
		using generator_t = typename lib::gen::Generator<O>;
		using pathkind_t = lib::gen::path::Kind;

		Repl(void) = delete;
		explicit Repl(std::ostream&);
		bool run_command(std::string const& cmd_line);

		// This is equal to `MAX_EXTRA_THREADS` floored by how many
		// concurrent threads the host processor can support at a time.
		const unsigned num_extra_threads;

		[[gnu::cold, gnu::pure]] pathkind_t get_path_kind(void) const noexcept { return path_kind; }
		// Setters return the old value of the generator path.
		[[gnu::cold]] pathkind_t set_path_kind(pathkind_t) noexcept;
		[[gnu::cold]] pathkind_t set_path_kind(std::string const&) noexcept;

	 private:
		generator_t gen;
		std::ostream& os; // alias to this->gen.os;
		verbosity::Kind output_level;
		pathkind_t path_kind;

		static constexpr unsigned MAX_EXTRA_THREADS = [](){ const unsigned _[] = {0,0,0,0,1,4,2,2}; return _[0]; }();
		const std::string DIM_ON  = (gen.is_pretty ? Ansi::DIM.ON  : "");
		const std::string DIM_OFF = (gen.is_pretty ? Ansi::DIM.OFF : "");

		// Return false if command is to exit the program:
		void run_single(bool contPrev = false);

		void run_multiple(trials_t num_trials, trials::StopAfterWhat);
		void run_multiple(std::string const&, trials::StopAfterWhat);
		void print_trials_work_distribution(trials_t num_total_trials,
			std::array<trials_t, trials::NUM_BINS+1> const& bin_hit_count,
			std::array<double,   trials::NUM_BINS+1> const& bin_ops_total);

	 public:
		[[gnu::cold]] verbosity::Kind get_output_level(void) const noexcept { return output_level; };
		[[gnu::cold]] verbosity::Kind set_output_level(verbosity::Kind);
		[[gnu::cold]] verbosity::Kind set_output_level(std::string const&);
	};
}
#endif