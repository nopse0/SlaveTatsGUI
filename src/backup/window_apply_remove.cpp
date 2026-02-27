#include "window_apply_remove.h"
#include "widget_actor_selector.h"
#include "widget_area_selector.h"
#include "widget_slot_selector.h"
#include "widget_cached_tattoo_selector.h"
#include "slavetats_util.h"
#include "slavetatsng_interface.h"
#include "imgui.h"
#include "imgui_utils.h"

namespace slavetats_ui
{
	void window_apply_remove::render(bool* window_shown) {

        //std::thread t1
        //{
        //    [&] {

		if (!ImGui::Begin("[SlaveTats] Apply/Remove Tattoo (old version)", window_shown)) {
			ImGui::End();
			return;
		}

		ImGui::PushID("window_apply_remove_old");

		static widget_available_slot_selector slot_selector;
		static widget_cached_tattoo_selector cache_selector;

		static widget_actor_selector actor_selector;
		actor_selector.render();

		std::string actor_version(JFormDB::getStr(actor_selector.actor, ".SlaveTats.version"));
		if (!actor_version.empty()) {
			ImGui::SameLine();
			ImGui::Text("(v%s)", actor_version.c_str());
		}

		ImGui::SameLine();

		jactor_tattoos_t jtattoos_by_id;
		jread_actor_tattoos(actor_selector.actor, jtattoos_by_id);

		jid_by_area_slot_t jtattoos_by_key;
		jtattoos_by_area_slot(jtattoos_by_id, jtattoos_by_key);

		ImGui::SameLine();
		static widget_area_selector area_selector;
		area_selector.render();
		if (area_selector.area_selected_value != area) {
			// Urgh, this manual refresh logic is really ugly, I have to think something off ... 
			slot_selector.reset();
			cache_selector.reset();
			area = area_selector.area_selected_value;
			
		}
		
		bool refresh_cache = false;
		if (ImGui::Button("Refresh installed tattoo cache")) {
			refresh_cache = true;
		}
		if (!area_sections.has_value() || refresh_cache) {
			// TODO: We should also rebuild the SlaveTats cache here, so tattoo packs can be installed at runtime (but currently that's not possible, because 'compile_cache'
			// isn't part of the SlaveTats api interface
			area_sections_t tmp;
			read_slavetats_installed_tattoos(tmp);
			area_sections.emplace(std::move(tmp));
			slot_selector.reset();
			cache_selector.reset();
		}

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
			slot_selector.reset();
			cache_selector.reset();
		}

		ImGui::SameLine();
		if (ImGui::Button("Synchronize visuals")) {
			if (actor) {
				// If manipulating the JContainer data directly, the '.updated' flag must be set before calling 'synchronize_tattoos' !!!
				JFormDB::setInt(actor, ".SlaveTats.updated", 1);
				slavetats::synchronize_tattoos(actor);
			}
		}


		ImGui::Separator();


		slot_selector.render(area, jtattoos_by_key, jtattoos_by_id, ni_overrides);
		if (slot_selector.slot_info != slot) {
			slot = slot_selector.slot_info;
			cache_selector.reset();
		}
		
		std::optional<const jfields_t*> applied_tattoo;
		if (slot_selector.slot_info.has_value() && slot_selector.slot_info.value().tattoo_id) {
			auto tatit = jtattoos_by_id.find(slot_selector.slot_info.value().tattoo_id);
			if (tatit != jtattoos_by_id.end()) {
				applied_tattoo = &tatit->second;
			}
		}

		auto ait = area_sections.value().find(area);
		if (ait != area_sections.value().end()) {
			// Cache selector resetted ?
			if (cache_selector.section_selected.empty()) {
				// Create 'tattoo by name' lookup table for the sections of the currently selected area
				section_tattoo_names.clear();
				auto ait = area_sections.value().find(area);
				if (ait != area_sections.value().end()) {
					for (auto& sec : ait->second) {
						name_to_tattoo_t tattoo_names;   // tattoo names of this section
						for (auto& tatit : sec.second) {
							auto namit = tatit.second.find("name");
							if (namit != tatit.second.end() && namit->second.type == jvalue_type::j_string) {
								auto& tattoo_name = std::get<std::string>(namit->second.value);
								tattoo_names.emplace(tattoo_name, tatit.first);
							}
						}
						section_tattoo_names.emplace(sec.first, std::move(tattoo_names));
					}
				}
			}
			cache_selector.render(ait->second, applied_tattoo, section_tattoo_names);
		}

		if (slot.has_value()) {
			if (slot.value().name.empty()) {
				if (cache_selector.tattoo_selected != 0) {
					if (ImGui::Button("Apply new tattoo")) {
						logger::info("'Apply new tattoo' was pressed");
						ImGui::OpenPopup("Apply new tattoo");
					}
				}
			}
			else {
				if (slot.value().tattoo_id != 0) {
					if (ImGui::Button("Remove tattoo")) {
						logger::info("'Remove tattoo' was pressed");
						ImGui::OpenPopup("Remove tattoo");
					}
				}
				if (cache_selector.tattoo_selected != 0 && slot.value().tattoo_id != 0) {
					ImGui::SameLine();
					if (slot.value().name == cache_selector.name_selected && slot.value().section == cache_selector.section_selected) {
						if (ImGui::Button("Re-apply tattoo (reset attributes)")) {
							logger::info("'Re-apply tattoo' was pressed");
							ImGui::OpenPopup("Re-apply tattoo");
						}
					}
					else {
						if (ImGui::Button("Replace tattoo")) {
							logger::info("'Replace tattoo' was pressed");
							ImGui::OpenPopup("Replace tattoo");
						}
					}
				}
			}
		}

		// ----- Popups -------------------

		if (ImGui::BeginPopupModal("Apply new tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (slot.has_value()) {
				std::string text(std::format(
					"Apply new tattoo in {}[{}] ?\n"
					"new tattoo will be: section = '{}', name = '{}'",
					magic_enum::enum_name(area), slot.value().slot_num,
					cache_selector.section_selected, cache_selector.name_selected
				));
				ImGui::Text(text.c_str());

				bool confirmed = false;
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					int new_tattoo = slavetats::add_and_get_tattoo(actor, cache_selector.tattoo_selected, slot.value().slot_num);
					if (new_tattoo)
						ImGui::OpenPopup("Success");
					else
						ImGui::OpenPopup("Error");
				}
				// ------- Confirmation popups -----------------
				bool success_shown = true;
				bool error_shown = true;
				show_confirmation_popups(&confirmed, &success_shown, &error_shown);
				if (confirmed) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Remove tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (slot.has_value()) {
				std::string text(std::format(
					"Remove tattoo in {}[{}] ?\n"
					"Currently applied tattoo is: section = '{}', name = '{}' ?", 
					magic_enum::enum_name(area), slot.value().slot_num, slot.value().section, slot.value().name));
				ImGui::Text(text.c_str());

				bool confirmed = false;
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					bool error = slavetats::remove_tattoo_from_slot(actor, magic_enum::enum_name(area), slot.value().slot_num);
					if (!error)
						ImGui::OpenPopup("Success");
					else
						ImGui::OpenPopup("Error");
				}
				// ------- Confirmation popups -----------------
				bool success_shown = true;
				bool error_shown = true;
				show_confirmation_popups(&confirmed, &success_shown, &error_shown);
				if (confirmed) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Re-apply tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (slot.has_value()) {
				std::string text(std::format(
					"Re-apply tattoo in {}[{}] ?\n" 
					"Tattoo is: section = '{}', name = '{}' ? \n"
					"This will reset the tattoo attributes to their defaults.", 
					magic_enum::enum_name(area), slot.value().slot_num, slot.value().section, slot.value().name));
				ImGui::Text(text.c_str());

				bool confirmed = false;
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					int new_tattoo = slavetats::add_and_get_tattoo(actor, cache_selector.tattoo_selected, slot.value().slot_num);
					if (new_tattoo)
						ImGui::OpenPopup("Success");
					else
						ImGui::OpenPopup("Error");
				}
				// ------- Confirmation popups -----------------
				bool success_shown = true;
				bool error_shown = true;
				show_confirmation_popups(&confirmed, &success_shown, &error_shown);
				if (confirmed) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Replace tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (slot.has_value()) {
				std::string text(std::format(
					"Replace tattoo in {}[{}] ?\n"
					"current tattoo is: section = '{}', name = '{}'\n"
					"new tattoo will be: section = '{}', name = '{}'",
					magic_enum::enum_name(area), slot.value().slot_num,
					slot.value().section, slot.value().name,
					cache_selector.section_selected, cache_selector.name_selected
					));
				ImGui::Text(text.c_str());

				bool confirmed = false;
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					int new_tattoo = 0;
					if (!slavetats::tattoo_matches(cache_selector.tattoo_selected, slot.value().tattoo_id)) {
						new_tattoo = slavetats::add_and_get_tattoo(actor, cache_selector.tattoo_selected, slot.value().slot_num);
					}
					if (new_tattoo)
						ImGui::OpenPopup("Success");
					else
						ImGui::OpenPopup("Error");
				}
				// ------- Confirmation popups -----------------
				bool success_shown = true;
				bool error_shown = true;
				show_confirmation_popups(&confirmed, &success_shown, &error_shown);
				if (confirmed) {
					ImGui::CloseCurrentPopup();
				}
				
				ImGui::SameLine();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}


		ImGui::PopID();

		ImGui::End();

		//	}
		//};
		//t1.join();
	}

}
