#pragma once
#include "nioverride_utils.h"

namespace slavetats_ui
{
	struct window_inspector {

		RE::Actor* actor = nullptr;
		std::optional< actor_overrides_t > ni_overrides;

		void render(bool *window_shown);
	};

}
