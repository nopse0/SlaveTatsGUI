#pragma once
#include "slavetats_util.h"

namespace slavetats_ui
{
	struct widget_area_selector {
		tattoo_area area_selected_value = tattoo_area::BODY;
		void render();
	};

}
