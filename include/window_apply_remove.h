#pragma once
#include "nioverride_utils.h"
#include "slavetats_util.h"

namespace slavetats_ui
{
	struct window_apply_remove {

		RE::Actor* actor = nullptr;
		tattoo_area area = tattoo_area::no_value;
		std::optional<slot_descriptor> slot;
		std::optional< actor_overrides_t > ni_overrides;
		std::optional< area_sections_t > area_sections;
		// section tattoos (by name) of the currently selected area
		section_tattoo_names_t section_tattoo_names;

		void render(bool* window_shown);
	};
}
