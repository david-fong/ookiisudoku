if(MSVC)
	# add_compile_options("$<$<CONFIG:Debug>:/RTC>")
	# /RTC is incompatible with asan.

	# https://docs.microsoft.com/en-us/cpp/sanitizers/asan
	# https://docs.microsoft.com/en-us/cpp/sanitizers/asan-building
	# known issues: https://docs.microsoft.com/en-us/cpp/sanitizers/asan-known-issues
	add_compile_options("$<$<CONFIG:Debug>:/fsanitize=address>")

	# TODO.try consider trying /sdl (additional security checks)
	#  Note: It has both compile-time and runtime checks, and I only want the runtime checks
	#  for debug-builds, so it seems more suitable to put it here than in the analyzers file.
	#  https://docs.microsoft.com/en-us/cpp/build/reference/sdl-enable-additional-security-checks?view=msvc-170

else()
	add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
	add_link_options(   "$<$<CONFIG:Debug>:-fsanitize=address,undefined>")

endif()