#pragma once
#include "slavetats_util.h"

namespace slavetats_ui
{
	struct widget_cached_tattoo_selector {
		std::string section_selected;
		std::string name_selected;
		int tattoo_selected = 0;
		
		inline void reset() { 
			section_selected.clear(); 
			name_selected.clear();
			tattoo_selected = 0;
		}
		void render(const section_tattoos_t& section_tattoos, std::optional<const jfields_t*> applied_tattoo, const section_tattoo_names_t& section_tattoo_names);
	};

}
