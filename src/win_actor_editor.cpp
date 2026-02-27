#include "win_actor_editor.h"
#include "model_and_views.h"
#include "imgui.h"
#include <magic_enum.hpp>
#include "slavetats_util.h"
#include "slavetatsng_interface.h"
#include "imgui_utils.h"
#include "combos.h"

namespace slavetats_ui {

	struct win_actor_editor_models {
		actor_model_t*             actor_model;
		area_model_t*              area_model;
		nioverride_model_t*        nioverride_model;
		applied_tattoos_model_t*   actor_tattoos_model;
		slots_model_t*             slots_model;
		selected_slot_model_t*     selected_slot_model;
		selected_section_model_t*  selected_section_model;
		tattoo_in_section_model_t* tattoo_in_section_model;

		win_actor_editor_models();
		~win_actor_editor_models();
	};

	win_actor_editor_models::win_actor_editor_models() {
		actor_model = new actor_model_t();
		area_model = new area_model_t();
		nioverride_model = new nioverride_model_t(actor_model);
		actor_tattoos_model = new applied_tattoos_model_t(actor_model);
		slots_model = new slots_model_t(actor_tattoos_model, nioverride_model);
		selected_slot_model = new selected_slot_model_t(area_model, slots_model);
		selected_section_model = new selected_section_model_t(area_model);
		tattoo_in_section_model = new tattoo_in_section_model_t(area_model, selected_section_model);
	}
	
	win_actor_editor_models::~win_actor_editor_models() {
		delete tattoo_in_section_model;
		delete selected_section_model;
		delete selected_slot_model;
		delete slots_model;
		delete actor_tattoos_model;
		delete nioverride_model;
		delete area_model;
		delete actor_model;
	}

	void combo_section_selector_render(area_model_t* area_model, selected_section_model_t* selected_section_model) {
		ImGui::PushID("combo_section_selector");
	
		installed_tattoos_model_t* installed_tattoos_model = installed_tattoos_model_t::GetSingleton();

		tattoo_area area = area_model->get_area();
		if (area != tattoo_area::no_value) {
			auto area_tattoos_it = installed_tattoos_model->installed_tattoo_names().find(area);
			if (area_tattoos_it != installed_tattoos_model->installed_tattoo_names().end()) {
				auto& installed_sections = area_tattoos_it->second;

				std::string preview_section("<Select a section>");
				if (selected_section_model->get_selected_section().has_value()) {
					preview_section = selected_section_model->get_selected_section().value().section;
				}
				if (ImGui::BeginCombo("section", preview_section.c_str(), ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightLarge))
				{
					for (auto& section : installed_sections)
					{
						bool is_selected = false;
						// I think, I could also simply compare to the preview value here
						if (selected_section_model->get_selected_section().has_value()) {
							auto& selected_section = selected_section_model->get_selected_section().value();
							if (selected_section.area == area && selected_section.section == section.first)
								is_selected = true;
						}

						if (ImGui::Selectable(section.first.c_str(), is_selected)) {
							selected_section_model->set_selected_section(
								selected_section_model_t::selection{ area, section.first });
						}
					}
					ImGui::EndCombo();
				}
			}
		}

		ImGui::PopID();
	}

	void combo_tattoo_in_section_selector_render(area_model_t* area_model, selected_section_model_t* selected_section_model, 
		tattoo_in_section_model_t* tattoo_in_section_model) {
		ImGui::PushID("combo_tattoo_in_section_selector");
	
		installed_tattoos_model_t* installed_tattoos_model = installed_tattoos_model_t::GetSingleton();

		tattoo_area area = area_model->get_area();
		if (area != tattoo_area::no_value) {
			auto area_tattoos_it = installed_tattoos_model->installed_tattoo_names().find(area);
			if (area_tattoos_it != installed_tattoos_model->installed_tattoo_names().end()) {
				if (selected_section_model->get_selected_section().has_value()) {
					auto& section = selected_section_model->get_selected_section().value();
					auto section_it = area_tattoos_it->second.find(section.section);
					if (section_it != area_tattoos_it->second.end()) {
						auto& installed_tattoos = section_it->second;
						std::string preview_tattoo("<Select a tattoo>");
						if (tattoo_in_section_model->get_selected_tattoo().has_value()) {
							preview_tattoo = tattoo_in_section_model->get_selected_tattoo().value().name;
						}
						if (ImGui::BeginCombo("tattoo", preview_tattoo.c_str(), ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightLarge))
						{
							for (auto& tattoo : installed_tattoos) {
								bool is_selected = false;
								if (tattoo_in_section_model->get_selected_tattoo().has_value()) {
									auto& selected_tattoo = tattoo_in_section_model->get_selected_tattoo().value();
									if (selected_tattoo.area == area && selected_tattoo.section == section.section && selected_tattoo.name == tattoo.first
										&& selected_tattoo.tattoo_id == tattoo.second)
										is_selected = true;
								}
								if (ImGui::Selectable(tattoo.first.c_str(), is_selected)) {
									tattoo_in_section_model->set_selected_tattoo(
										tattoo_in_section_model_t::selection{ area, section.section, tattoo.first, tattoo.second });
								}
							}
							ImGui::EndCombo();
						}
					}
				}
			}
		}
		ImGui::PopID();
	}

	void  buttons_apply_tattoos(selected_slot_model_t* selected_slot, tattoo_in_section_model_t* tattoo_in_section) {
		if (selected_slot->selected_slot().has_value()) {
			if (selected_slot->selected_slot().value().tattoo_id == 0) {
				if (tattoo_in_section->get_selected_tattoo().has_value()) {
					if (ImGui::Button("Apply new tattoo")) {
						logger::info("'Apply new tattoo' was pressed");
						ImGui::OpenPopup("Apply new tattoo");
					}
				}
			}
			else {
				if (ImGui::Button("Remove tattoo")) {
					logger::info("'Remove tattoo' was pressed");
					ImGui::OpenPopup("Remove tattoo");
				}
				if (tattoo_in_section->get_selected_tattoo().has_value()) {
					ImGui::SameLine();
					if (selected_slot->selected_slot().value().name == tattoo_in_section->get_selected_tattoo().value().name) {
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
	}

	void popup_apply_new_tattoo(actor_model_t* actor_model, selected_slot_model_t* selected_slot, tattoo_in_section_model_t* tattoo_in_section,
		applied_tattoos_model_t* applied_tattoos, nioverride_model_t* nioverride_model, bool* confirmed) {

		auto actor = actor_model->get_actor();
		if (actor) {
			if (selected_slot->selected_slot().has_value() && tattoo_in_section->get_selected_tattoo().has_value()) {
				if (ImGui::BeginPopupModal("Apply new tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					auto& slot_info = selected_slot->selected_slot().value();
					auto& tattoo = tattoo_in_section->get_selected_tattoo().value();
					std::string text(std::format(
						"Apply new tattoo in {}[{}] ?\n"
						"new tattoo will be: section = '{}', name = '{}'",
						magic_enum::enum_name(slot_info.area), slot_info.slot_num,
						tattoo.section, tattoo.name
					));
					ImGui::Text(text.c_str());

					if (ImGui::Button("OK", ImVec2(120, 0))) {
						int new_tattoo = slavetats::add_and_get_tattoo(actor, tattoo.tattoo_id, slot_info.slot_num);
						if (new_tattoo) {
							ImGui::OpenPopup("Success");
							// refresh_models(actor, applied_tattoos, nioverride_model);
						}
						else
							ImGui::OpenPopup("Error");
					}
					// ------- Confirmation popups -----------------
					bool success_shown = true;
					bool error_shown = true;
					show_confirmation_popups(confirmed, &success_shown, &error_shown);
					//if (confirmed) {
					//	ImGui::CloseCurrentPopup();
					//}

					ImGui::SameLine();
					ImGui::SetItemDefaultFocus();
					if (ImGui::Button("Cancel", ImVec2(120, 0))) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
		}
	}

	void popup_remove_tattoo(actor_model_t* actor_model, selected_slot_model_t* selected_slot, applied_tattoos_model_t* applied_tattoos, 
		nioverride_model_t* nioverride_model, bool* confirmed) {
		auto actor = actor_model->get_actor();
		if (actor) {
			if (selected_slot->selected_slot().has_value()) {
				auto& slot_info = selected_slot->selected_slot().value();

				if (ImGui::BeginPopupModal("Remove tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					std::string text(std::format(
						"Remove tattoo in {}[{}] ?\n"
						"Currently applied tattoo is: section = '{}', name = '{}' ?",
						magic_enum::enum_name(slot_info.area), slot_info.slot_num, slot_info.section, slot_info.name));
					ImGui::Text(text.c_str());

					if (ImGui::Button("OK", ImVec2(120, 0))) {
						bool error = slavetats::remove_tattoo_from_slot(actor, magic_enum::enum_name(slot_info.area), slot_info.slot_num);
						if (!error) {
							ImGui::OpenPopup("Success");
							// refresh_models(actor, applied_tattoos, nioverride_model);
						} 
						else
							ImGui::OpenPopup("Error");
					}
					// ------- Confirmation popups -----------------
					bool success_shown = true;
					bool error_shown = true;
					show_confirmation_popups(confirmed, &success_shown, &error_shown);
					//if (confirmed) {
					//	ImGui::CloseCurrentPopup();
					//}

					ImGui::SameLine();
					ImGui::SetItemDefaultFocus();
					if (ImGui::Button("Cancel", ImVec2(120, 0))) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}
		}
	}

	void popup_reapply_tattoo(actor_model_t* actor_model, selected_slot_model_t* selected_slot, tattoo_in_section_model_t* tattoo_in_section,
		applied_tattoos_model_t* applied_tattoos, nioverride_model_t* nioverride_model, bool* confirmed) {

		auto actor = actor_model->get_actor();
		if (actor) {
			if (selected_slot->selected_slot().has_value() && tattoo_in_section->get_selected_tattoo().has_value()) {
				auto& slot_info = selected_slot->selected_slot().value();
				auto& tattoo = tattoo_in_section->get_selected_tattoo().value();

				if (ImGui::BeginPopupModal("Re-apply tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					std::string text(std::format(
						"Re-apply tattoo in {}[{}] ?\n"
						"Tattoo is: section = '{}', name = '{}' ? \n"
						"This will reset the tattoo attributes to their defaults.",
						magic_enum::enum_name(slot_info.area), slot_info.slot_num, slot_info.section, slot_info.name));
					ImGui::Text(text.c_str());

					if (ImGui::Button("OK", ImVec2(120, 0))) {
						int new_tattoo = slavetats::add_and_get_tattoo(actor, tattoo.tattoo_id, slot_info.slot_num);
						if (new_tattoo) {
							ImGui::OpenPopup("Success");
							// refresh_models(actor, applied_tattoos, nioverride_model);
						}
						else
							ImGui::OpenPopup("Error");
					}
					// ------- Confirmation popups -----------------
					bool success_shown = true;
					bool error_shown = true;
					show_confirmation_popups(confirmed, &success_shown, &error_shown);
					//if (confirmed) {
					//	ImGui::CloseCurrentPopup();
					//}

					ImGui::SameLine();
					ImGui::SetItemDefaultFocus();
					if (ImGui::Button("Cancel", ImVec2(120, 0))) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}
		}
	}



	void popup_replace_tattoo(actor_model_t* actor_model, selected_slot_model_t* selected_slot, tattoo_in_section_model_t* tattoo_in_section,
		applied_tattoos_model_t* applied_tattoos, nioverride_model_t* nioverride_model, bool* confirmed) {

		auto actor = actor_model->get_actor();
		if (actor) {
			if (selected_slot->selected_slot().has_value() && tattoo_in_section->get_selected_tattoo().has_value()) {
				auto& slot_info = selected_slot->selected_slot().value();
				auto& tattoo = tattoo_in_section->get_selected_tattoo().value();

				if (ImGui::BeginPopupModal("Replace tattoo", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					std::string text(std::format(
						"Replace tattoo in {}[{}] ?\n"
						"current tattoo is: section = '{}', name = '{}'\n"
						"new tattoo will be: section = '{}', name = '{}'",
						magic_enum::enum_name(slot_info.area), slot_info.slot_num,
						slot_info.section, slot_info.name,
						tattoo.section, tattoo.name
					));
					ImGui::Text(text.c_str());

					if (ImGui::Button("OK", ImVec2(120, 0))) {
						int new_tattoo = 0;
						if (!slavetats::tattoo_matches(tattoo.tattoo_id, slot_info.tattoo_id)) {
							new_tattoo = slavetats::add_and_get_tattoo(actor, tattoo.tattoo_id, slot_info.slot_num);
						}
						if (new_tattoo) {
							ImGui::OpenPopup("Success");
							// refresh_models(actor, applied_tattoos, nioverride_model);
						}
						else
							ImGui::OpenPopup("Error");
					}
					// ------- Confirmation popups -----------------
					bool success_shown = true;
					bool error_shown = true;
					show_confirmation_popups(confirmed, &success_shown, &error_shown);
					//if (confirmed) {
					//	ImGui::CloseCurrentPopup();
					//}

					ImGui::SameLine();
					ImGui::SetItemDefaultFocus();
					if (ImGui::Button("Cancel", ImVec2(120, 0))) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}
		}
	}
	
	void win_actor_editor_render(bool* window_shown) {
		if (!ImGui::Begin("[SlaveTats] Apply/Remove Tattoo", window_shown)) {
			ImGui::End();
			return;
		}

		std::thread t1
		{
			[&] {

		ImGui::PushID("win_actor_editor");

		static win_actor_editor_models* models = new win_actor_editor_models();

		slavetats_model_t::GetSingleton()->refresh();

		combo_actor_selector_render(models->actor_model);

		// Show tattoo version of the selected actor
		if (!models->actor_tattoos_model->tattoo_version().empty()) {
			ImGui::SameLine();
			ImGui::Text("(v%s)", models->actor_tattoos_model->tattoo_version().c_str());
		}

		ImGui::SameLine();
		combo_area_selector_render(models->area_model);

		combo_slot_selector_render(true, models->slots_model, models->area_model, models->selected_slot_model);

		combo_section_selector_render(models->area_model, models->selected_section_model);

		combo_tattoo_in_section_selector_render(models->area_model, models->selected_section_model, models->tattoo_in_section_model);
		
		buttons_apply_tattoos(models->selected_slot_model, models->tattoo_in_section_model);

		// define popups, which can be shown via the buttons above
		bool confirmed = false;
		popup_apply_new_tattoo(models->actor_model, models->selected_slot_model, models->tattoo_in_section_model,
			models->actor_tattoos_model, models->nioverride_model, &confirmed);
		popup_remove_tattoo(models->actor_model, models->selected_slot_model, models->actor_tattoos_model, models->nioverride_model, &confirmed);
		popup_reapply_tattoo(models->actor_model, models->selected_slot_model, models->tattoo_in_section_model,
			models->actor_tattoos_model, models->nioverride_model, &confirmed);
		popup_replace_tattoo(models->actor_model, models->selected_slot_model, models->tattoo_in_section_model,
			models->actor_tattoos_model, models->nioverride_model, &confirmed);

		if (confirmed) {
			ImGui::CloseCurrentPopup();
			refresh_models(models->actor_model, models->actor_tattoos_model, models->nioverride_model);
		}

		ImGui::PopID();

		ImGui::End();

			}
		};
		t1.join();

	}

}

