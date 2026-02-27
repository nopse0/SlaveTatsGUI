#pragma once
#include "slavetats_util.h"
#include "imgui.h"

namespace slavetats_ui
{
	struct widget_field_selector {
		std::string selected_field;
		std::string selected_field_last;

		void render(ImGuiTableFlags flags, const jfields_t& jfields);
	};

}
