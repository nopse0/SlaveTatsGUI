#pragma once
#include "slavetats_util.h"
#include "nioverride_utils.h"

namespace slavetats_ui
{
	struct widget_override_list {
		//tattoo_area area_selected_value = tattoo_area::BODY;

		void render(tattoo_area area, const actor_overrides_t& overrides, const jactor_tattoos_t& jtattoos_by_id, const jid_by_area_slot_t& jtattoos_by_key) const;
	};

}
