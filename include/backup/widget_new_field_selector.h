#pragma once
#include "tattoo_field_db.h"

namespace slavetats_ui
{
	struct widget_new_field_selector {
		field_db::field_definition selected_field;

		void render(const field_db::field_db_t& field_defs);
	};

}
