#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#undef cdecl // Workaround for Clang 14 CMake configure error.

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <fmt/format.h>

#include <ClibUtil/singleton.hpp>
#include <ClibUtil/simpleINI.hpp>

#include <latch>

template <>
struct fmt::formatter<RE::BSFixedString>
{
	// Presentation format: 'f' - fixed, 'e' - exponential.
	char presentation = 'f';

	// Parses format specifications of the form ['f' | 'e'].
	constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator
	{
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && (*it == 'f'))
			presentation = *it++;

		// Check if reached the end of the range:
		if (it != end && *it != '}')
			throw_format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}
	// Formats the point p using the parsed format specification (presentation)
	// stored in this formatter.
	auto format(const RE::BSFixedString& v, format_context& ctx) const -> format_context::iterator
	{
		// ctx.out() is an output iterator to write to.
		return fmt::format_to(ctx.out(), "{}", v.data());
	}
};

// Compatible declarations with other sample projects.
#define DLLEXPORT __declspec(dllexport)

using namespace std::literals;
using namespace REL::literals;

namespace logger = SKSE::log;

namespace util {
    using SKSE::stl::report_and_fail;
}

namespace slavetats_ng
{
	typedef bool fail_t;
}
