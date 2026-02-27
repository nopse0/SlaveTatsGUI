#include "combos.h"
#include "imgui.h"

namespace slavetats_ui {

	void combo_actor_selector_render(actor_model_t* actor_model) {
		ImGui::PushID("combo_actor_selector");
		auto& actors = actor_scanner_model::GetSingleton()->actors();
		auto& actor_labels = actor_scanner_model::GetSingleton()->actor_labels();
		auto actor = actor_model->get_actor();

		std::string preview_value("<Select an actor>");
		if (actor) {
			preview_value = actor_model->get_actor_label();
		}
		if (ImGui::BeginCombo("actor", preview_value.c_str(), ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
		{
			for (int n = 0; n < actor_labels.size(); n++)
			{
				const bool is_selected = (preview_value == actor_labels[n]);
				if (ImGui::Selectable(actor_labels[n].c_str(), is_selected)) {
					actor_model->set_actor(actors[n]);
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();
	}

	void combo_area_selector_render(area_model_t* area_model) {
		ImGui::PushID("combo_area_selector");
		const char* preview_value = magic_enum::enum_name(area_model->get_area()).data();
		if (ImGui::BeginCombo("area", preview_value, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
		{
			for (int n = 0; n < tattoo_areas.size(); n++)
			{
				const bool is_selected = (magic_enum::enum_index(area_model->get_area()) == n);
				if (ImGui::Selectable(tattoo_areas[n].data(), is_selected)) {
					area_model->set_area(magic_enum::enum_value<tattoo_area>(n));
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();
	}

	void combo_slot_selector_render(bool select_empty, slots_model_t* slots_model, area_model_t* area_model, selected_slot_model_t* selected_slot_model) {
		ImGui::PushID("combo_slot_selector");
		auto area = area_model->get_area();
		if (area != tattoo_area::no_value) {
			auto it = slots_model->slot_infos().find(area);
			if (it != slots_model->slot_infos().end()) {
				auto& slots = it->second;
				auto& selected_slot = selected_slot_model->selected_slot();
				std::string selected_label("<Select a slot>");
				if (selected_slot.has_value())
					selected_label = selected_slot.value().label;
				if (ImGui::BeginCombo("slot", selected_label.c_str(), ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
				{
					for (int n = 0; n < slots.size(); n++)
					{
						auto& info = slots[n];
						if (info.tattoo_id != 0 || (select_empty && info.texture_name.empty()))  // selectable: internal or empty
						{
							const bool is_selected = selected_slot.has_value() && selected_slot.value().slot_num == info.slot_num && selected_slot.value().area == info.area;
							if (ImGui::Selectable(info.label.c_str(), is_selected)) {
								selected_slot_model->set_selected_slot(info);
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						else {       // external slot or empty => not selectable
							ImGui::TextDisabled(info.label.c_str());
						}
					}
					ImGui::EndCombo();
				}
			}
		}
		ImGui::PopID();

	}


}
