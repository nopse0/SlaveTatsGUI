#pragma once

#pragma once

#include "magic_enum.hpp"

namespace slavetats_ui
{
	// static std::array<const char*, 3> items = { "slider", "%f", "%a" };

	enum class float_input_method {
		dec_fmt = 0,
		hex_fmt = 1,
		slider_0_to_1 = 2,
	};

	static inline constexpr auto input_method_labels_float = magic_enum::enum_entries<float_input_method>();

	// The first entries of integer_input_method specify format strings
	static inline constexpr std::array format_strings_float = {
		"%f",  // dec_fmt
		"%a"   // hex_fmt
	};


	class widget_float_editor {
	public:
		// State
		float _float_old = 0;
		float _float_new = 0;
		float_input_method input_method_index = float_input_method::dec_fmt;
		const char* input_method_label = magic_enum::enum_name(float_input_method::dec_fmt).data();

	public:


		void render(float current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key, bool is_new);
	};


}
