#pragma once

#include "model_and_views.h"

namespace slavetats_ui {

	void combo_actor_selector_render(actor_model_t* actor_model);
	void combo_area_selector_render(area_model_t* area_model);
	void combo_slot_selector_render(bool select_empty, slots_model_t* slots_model, area_model_t* area_model, selected_slot_model_t* selected_slot_model);

}