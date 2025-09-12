#pragma once

#include "slavetats_util.h"

namespace slavetats_ui
{
	struct widget_actor_selector {

		std::string actor_label;
		int actor_selected_index = 0;
		RE::Actor* actor = nullptr;

		void render();
	};

}
