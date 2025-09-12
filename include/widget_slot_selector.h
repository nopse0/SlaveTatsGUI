#pragma once
#include "slavetats_util.h"
#include "nioverride_utils.h"

namespace slavetats_ui
{
	struct widget_slavetats_slot_selector {
		int slot_selected_index = 0;
		//int slot_selected = 0;
		//int tattoo_selected = 0;
		slot_descriptor slot_info;

		void render(tattoo_area area, const jid_by_area_slot_t& tattoo_ids, const jactor_tattoos_t& tattoos);
	};

	struct widget_available_slot_selector {
		std::optional<slot_descriptor> slot_info;
		inline void reset() { slot_info.reset(); }
		void render(tattoo_area area, const jid_by_area_slot_t& tattoo_ids, const jactor_tattoos_t& tattoos, const std::optional<actor_overrides_t>& overrides);
	};

}
