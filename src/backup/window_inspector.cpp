#include "window_inspector.h"
#include "slavetatsng_wrapper.h"
#include "jcontainers_wrapper.h"
#include "widget_actor_selector.h"
#include "widget_area_selector.h"
#include "slavetats_util.h"
#include "nioverride_utils.h"
#include "widget_override_list.h"
#include "imgui.h"

namespace slavetats_ui
{
	using namespace slavetats_ng::jcwrapper;

	void window_inspector::render(bool* window_shown) {

		//		std::thread t1
		//		{
		//			[&] {

		if (!ImGui::Begin("[SlaveTats] Inspector (old version)", window_shown)) {
			ImGui::End();
			return;
		}

		static widget_actor_selector actor_selector;
		actor_selector.render();

		jactor_tattoos_t jtattoos_by_id;
		jread_actor_tattoos(actor_selector.actor, jtattoos_by_id);

		jid_by_area_slot_t jtattoos_by_key;
		jtattoos_by_area_slot(jtattoos_by_id, jtattoos_by_key);

		ImGui::SameLine();
		static widget_area_selector area_selector;
		area_selector.render();

		ImGui::SameLine();
		bool refresh_nioverride = false;
		if (ImGui::Button("Refresh NiOverride data")) {
			refresh_nioverride = true;
		}

		if (actor_selector.actor != actor || refresh_nioverride) {
			ni_overrides.reset();
			actor = actor_selector.actor;
			if (actor != nullptr) {
				actor_overrides_t overrides;
				read_actor_overrides(actor, overrides);
				ni_overrides.emplace(std::move(overrides));
			}
		}

		if (ni_overrides.has_value() || jtattoos_by_key.size() > 0) {
			static widget_override_list override_list;
			override_list.render(area_selector.area_selected_value, ni_overrides.value(), jtattoos_by_id, jtattoos_by_key);
		}

		ImGui::End();


		//			}
		//		};
		//		t1.join();
	}

}
