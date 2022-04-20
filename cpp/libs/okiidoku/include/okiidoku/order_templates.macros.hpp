#ifndef HPP_OKIIDOKU__ORDER_TEMPLATES__MACROS
#define HPP_OKIIDOKU__ORDER_TEMPLATES__MACROS

#include <okiidoku/config/defaults.hpp>


#if OKIIDOKU_COMPILE_O2
#define OKIIDOKU_TEMPLATE_FOR_O2 OKIIDOKU_FOR_COMPILED_O(2)
#else
#define OKIIDOKU_TEMPLATE_FOR_O2
#endif

#if OKIIDOKU_COMPILE_O3
#define OKIIDOKU_TEMPLATE_FOR_O3 OKIIDOKU_FOR_COMPILED_O(3)
#else
#define OKIIDOKU_TEMPLATE_FOR_O3
#endif

#if OKIIDOKU_COMPILE_O4
#define OKIIDOKU_TEMPLATE_FOR_O4 OKIIDOKU_FOR_COMPILED_O(4)
#else
#define OKIIDOKU_TEMPLATE_FOR_O4
#endif

#if OKIIDOKU_COMPILE_O5
#define OKIIDOKU_TEMPLATE_FOR_O5 OKIIDOKU_FOR_COMPILED_O(5)
#else
#define OKIIDOKU_TEMPLATE_FOR_O5
#endif

#if OKIIDOKU_COMPILE_O6
#define OKIIDOKU_TEMPLATE_FOR_O6 OKIIDOKU_FOR_COMPILED_O(6)
#else
#define OKIIDOKU_TEMPLATE_FOR_O6
#endif

#if OKIIDOKU_COMPILE_O7
#define OKIIDOKU_TEMPLATE_FOR_O7 OKIIDOKU_FOR_COMPILED_O(7)
#else
#define OKIIDOKU_TEMPLATE_FOR_O7
#endif

#if OKIIDOKU_COMPILE_O8
#define OKIIDOKU_TEMPLATE_FOR_O8 OKIIDOKU_FOR_COMPILED_O(8)
#else
#define OKIIDOKU_TEMPLATE_FOR_O8
#endif

#if OKIIDOKU_COMPILE_O9
#define OKIIDOKU_TEMPLATE_FOR_O9 OKIIDOKU_FOR_COMPILED_O(9)
#else
#define OKIIDOKU_TEMPLATE_FOR_O9
#endif

#if OKIIDOKU_COMPILE_O10
#define OKIIDOKU_TEMPLATE_FOR_O10 OKIIDOKU_FOR_COMPILED_O(10)
#else
#define OKIIDOKU_TEMPLATE_FOR_O10
#endif

#if OKIIDOKU_COMPILE_O11
#define OKIIDOKU_TEMPLATE_FOR_O11 OKIIDOKU_FOR_COMPILED_O(11)
#else
#define OKIIDOKU_TEMPLATE_FOR_O11
#endif

#if OKIIDOKU_COMPILE_O12
#define OKIIDOKU_TEMPLATE_FOR_O12 OKIIDOKU_FOR_COMPILED_O(12)
#else
#define OKIIDOKU_TEMPLATE_FOR_O12
#endif

#if OKIIDOKU_COMPILE_O16
#define OKIIDOKU_TEMPLATE_FOR_O16 OKIIDOKU_FOR_COMPILED_O(16)
#else
#define OKIIDOKU_TEMPLATE_FOR_O16
#endif


// At usage sites, first #define OKIIDOKU_FOR_COMPILED_O and then #undef it right after.
// Note: This _seems_ to violate pitchfork library spec:
//  "A library should not offer the user controls for tweaking its public interface."...
//  https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#libraries
//  But I think it's okay. Each order could be technically be treated as its own standalone API.
#define OKIIDOKU_INSTANTIATE_ORDER_TEMPLATES \
OKIIDOKU_TEMPLATE_FOR_O2 \
OKIIDOKU_TEMPLATE_FOR_O3 \
OKIIDOKU_TEMPLATE_FOR_O4 \
OKIIDOKU_TEMPLATE_FOR_O5 \
OKIIDOKU_TEMPLATE_FOR_O6 \
OKIIDOKU_TEMPLATE_FOR_O7 \
OKIIDOKU_TEMPLATE_FOR_O8 \
OKIIDOKU_TEMPLATE_FOR_O9 \
OKIIDOKU_TEMPLATE_FOR_O10 \
OKIIDOKU_TEMPLATE_FOR_O11 \
OKIIDOKU_TEMPLATE_FOR_O12 \
OKIIDOKU_TEMPLATE_FOR_O16

#endif