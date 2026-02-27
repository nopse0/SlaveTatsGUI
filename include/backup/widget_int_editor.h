#pragma once

#include "magic_enum.hpp"
#include "imgui_utils.h"

namespace slavetats_ui
{
	enum class integer_input_method {
		dec_fmt = 0,
		hex_fmt = 1,
		color_pick_wheel = 2,
		color_pick_bar = 3,
		color_pick_palette = 4
	};

	static inline constexpr auto input_method_labels_int = magic_enum::enum_entries<integer_input_method>();

	// The first entries of integer_input_method specify format strings
	static inline constexpr std::array format_strings_int = {
		"%d",  // dec_fmt
		"%x"   // hex_fmt
	};

	// ColorDialog palette from SkyUI (ABGR)
	static inline constexpr std::array skyui_palette = {
		0x990033, 0xAD0073, 0xA17700, 0x803D0D, 0xBD4F19, 0x007A87, 0x162274, 0x4F2D7F, 0x56364D, 0x618E02, 0x008542, 0x5C4836, 0x999999, 0x000000,
		0xCC0033, 0xE86BA5, 0xEAAB00, 0xB88454, 0xE37222, 0x99FFFF, 0x4060AF, 0x8C6CD0, 0x8F6678, 0x9EAB05, 0x19B271, 0xAA9C8F, 0xCCCCCC, 0xFFFFFF
	};
	
	// Same as ImVec4
	static inline constexpr std::array skyui_palette_vec4 = []{
		std::array<ImVec4, skyui_palette.size()> palette{};
		float s = 1.0f / 255.0f;
		for (int i = 0; i < skyui_palette.size(); i++) {
			auto in = skyui_palette[i];
			// copied from ImGui
			palette[i] = ImVec4(
				((in >> IM_COL32_R_SHIFT) & 0xFF) * s,
				((in >> IM_COL32_G_SHIFT) & 0xFF) * s,
				((in >> IM_COL32_B_SHIFT) & 0xFF) * s,
				((in >> IM_COL32_A_SHIFT) & 0xFF) * s);
		}
		return palette;
		}();


	class widget_int_editor {
	public:	
		// State
		int _int_old = 0;
		int _int_new = 0;
		integer_input_method input_method_index = integer_input_method::dec_fmt;
		const char* input_method_label = magic_enum::enum_name(integer_input_method::dec_fmt).data();
	
	public:
		

		void render(int current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key, bool is_new);
	};
}
