#include "win_field_editor.h"
#include "imgui.h"
#include "model_and_views.h"
#include "combos.h"
#include "tattoo_field_db.h"
#include "editors.h"
#include "magic_enum_all.hpp"
#include "imgui_utils.h"

namespace slavetats_ui {

	struct win_field_editor_models {
		actor_model_t* actor_model;
		area_model_t* area_model;
		nioverride_model_t* nioverride_model;
		applied_tattoos_model_t* actor_tattoos_model;
		slots_model_t* slots_model;
		selected_slot_model_t* selected_slot_model;
		field_in_tattoo_model_t* field_in_tattoo_model;
		field_editor_int_model_t* field_editor_int_model;
		field_editor_float_model_t* field_editor_float_model;
		field_editor_string_model_t* field_editor_string_model;

		win_field_editor_models();
		~win_field_editor_models();
	};

	win_field_editor_models::win_field_editor_models() {
		actor_model = new actor_model_t();
		area_model = new area_model_t();
		nioverride_model = new nioverride_model_t(actor_model);
		actor_tattoos_model = new applied_tattoos_model_t(actor_model);
		slots_model = new slots_model_t(actor_tattoos_model, nioverride_model);
		selected_slot_model = new selected_slot_model_t(area_model, slots_model);
		field_in_tattoo_model = new field_in_tattoo_model_t(actor_tattoos_model, selected_slot_model);
		field_editor_int_model = new field_editor_int_model_t(field_in_tattoo_model);
		field_editor_float_model = new field_editor_float_model_t(field_in_tattoo_model);
		field_editor_string_model = new field_editor_string_model_t(field_in_tattoo_model);
	}

	win_field_editor_models::~win_field_editor_models() {
		delete field_editor_string_model;
		delete field_editor_float_model;
		delete field_editor_int_model;
		delete field_in_tattoo_model;
		delete selected_slot_model;
		delete slots_model;
		delete actor_tattoos_model;
		delete nioverride_model;
		delete area_model;
		delete actor_model;
	}

	void table_field_selector_render(ImGuiTableFlags table_flags, applied_tattoos_model_t* applied_tattoos_model, selected_slot_model_t* selected_slot_model, 
		field_in_tattoo_model_t* selected_field_model) {
		ImGui::PushID("table_field_selector");
		
		if (selected_slot_model->selected_slot().has_value() && applied_tattoos_model->tattoos().has_value()) {
			auto& tattoos = applied_tattoos_model->tattoos().value();
			auto& selected_slot = selected_slot_model->selected_slot().value();
			if (selected_slot.tattoo_id) {
				auto tattoo_id = selected_slot.tattoo_id;
				auto tattoo_it = tattoos.find(selected_slot.tattoo_id);
				if (tattoo_it != tattoos.end()) {
					auto& fields = tattoo_it->second;
					if (ImGui::BeginTable("attributes", 3, table_flags)) {
						ImGui::TableSetupColumn("key", ImGuiTableColumnFlags_WidthFixed);
						ImGui::TableSetupColumn("type", ImGuiTableColumnFlags_WidthFixed);
						ImGui::TableSetupColumn("value");
						ImGui::TableHeadersRow();
						for (auto field_it = fields.begin(); field_it != fields.end(); ++field_it) {
							auto& field_name = field_it->first;
							auto& field_value = field_it->second;
							jvalue_type field_type = field_value.type;
							std::string type_name(jvalue_type_pretty_names[magic_enum::enum_index(field_type).value()]);
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::TextUnformatted(field_name.c_str());
							ImGui::TableSetColumnIndex(1);
							ImGui::TextUnformatted(type_name.c_str());
							ImGui::TableSetColumnIndex(2);
							auto field_def = field_db::find_editable_attribute(field_name, field_type); // for checking, if the field is editable
							auto field_value_string = jvalue_as_string(field_value);
							if (field_def.has_value()) {
								bool is_selected = false;
								if (selected_field_model->get_selected_field().has_value()) {
									is_selected = selected_field_model->get_selected_field().value().field_name == field_name;
									}
								if (ImGui::Selectable((field_value_string + "##" + field_name).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
									selected_field_model->set_selected_field(std::optional(field_in_tattoo_model_t::selection{ field_name, field_value, tattoo_id }));
									selected_field_model->set_selected_definition(field_def);
								}
							}
							else {
								ImGui::TextDisabled(field_value_string.c_str());
							}
						}
						// additional table row for adding new fields
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted("[new]");
						ImGui::TableSetColumnIndex(1);
						ImGui::TextUnformatted("?");
						ImGui::TableSetColumnIndex(2);
						bool is_selected = false;
						if (selected_field_model->get_selected_field().has_value())
							is_selected = selected_field_model->get_selected_field().value().field_name == "[new]";
						if (ImGui::Selectable("?", is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
							selected_field_model->set_selected_field(std::optional(field_in_tattoo_model_t::selection{ "[new]", jvalue_t{}, tattoo_id }));
							selected_field_model->set_selected_definition(std::optional<field_db::field_definition>());  // will be filled in later
						}
					}
					ImGui::EndTable();
				}
			}
		}
		ImGui::PopID();
	}


	void combo_field_definition_selector_render(applied_tattoos_model_t* applied_tattoos_model, field_in_tattoo_model_t* selected_field_model) {

		ImGui::PushID("combo_field_definition_selector");
		if (selected_field_model->get_selected_field().has_value()) {
			auto& selected_field = selected_field_model->get_selected_field().value();
			if (applied_tattoos_model->tattoos().has_value()) {
				auto tattoo_it = applied_tattoos_model->tattoos().value().find(selected_field.tattoo_id);
				if (tattoo_it != applied_tattoos_model->tattoos().value().end()) {
					auto& fields = tattoo_it->second;
					if (selected_field.field_name == "[new]") {
						auto preview_value = std::string("<Select a field definition>");
						if (selected_field_model->get_selected_definition().has_value()) {
							preview_value = selected_field_model->get_selected_definition().value().name;
						}
						if (ImGui::BeginCombo("new field", preview_value.c_str(), ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightLarge)) {
							for (auto& field_def : field_db::field_db) {
								if (!fields.contains(field_def.first) && field_def.second.editable) {
									bool is_selected = false;
									if (selected_field_model->get_selected_definition().has_value()) {
										is_selected = selected_field_model->get_selected_definition().value().name == field_def.first;
									}
									if (ImGui::Selectable(field_def.first.c_str(), is_selected)) {
										selected_field_model->set_selected_definition(std::optional(field_def.second));
									}
									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								else {
									ImGui::TextDisabled(field_def.first.c_str());
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

	void group_field_editor_int_render(field_in_tattoo_model_t* selected_field_model, field_editor_int_model_t* field_editor_int_model) {
		ImGui::PushID(" group_field_int_editor");

		if (selected_field_model->get_selected_field().has_value() && selected_field_model->get_selected_definition().has_value()
			&& field_editor_int_model->get_buffer_address().has_value() && field_editor_int_model->get_input_method().has_value()) {
			auto preview_value = input_method_label_int(field_editor_int_model->get_input_method().value());
			if (ImGui::BeginCombo("input method", preview_value.second.data(), ImGuiComboFlags_PopupAlignLeft)) {
				magic_enum::enum_for_each<input_method_int>([field_editor_int_model](auto val) {
					constexpr input_method_int method = val;
					const bool is_selected = field_editor_int_model->get_input_method().value() == method;
					if (ImGui::Selectable(input_method_label_int(method).second.data(), is_selected)) {
						field_editor_int_model->set_input_method(method);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
					});
				ImGui::EndCombo();
			}
			auto buffer = field_editor_int_model->get_buffer_address().value();
			auto method = field_editor_int_model->get_input_method().value();
			if (method == input_method_int::dec_fmt || method == input_method_int::hex_fmt) {
				ImGui::InputScalar("new value (with format string)", ImGuiDataType_U32, buffer, NULL, NULL, input_method_int_formats[method]);
			}
			else if (method == input_method_int::color_pick_palette) {
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(ARGBtoABGR(*buffer));
				ImGui::BeginGroup(); // Lock X position
				ImGui::Text("Current");
				ImGui::ColorButton("##color button", color, ImGuiColorEditFlags_NoPicker, ImVec2(60, 40));
				ImGui::Separator();
				ImGui::Text("Palette");
				for (int n = 0; n < skyui_palette.size(); n++)
				{
					ImGui::PushID(n);
					if (n % (skyui_palette.size() / 4) != 0)
						ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);
					ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
					if (ImGui::ColorButton("##palette", skyui_palette_vec4[n], palette_button_flags, ImVec2(60, 60))) {
						*buffer = ABGRtoARGB(skyui_palette[n]);
					}
					ImGui::PopID();
				}
				ImGui::EndGroup();
			}
			else {
				ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV |
					ImGuiColorEditFlags_DisplayHex;
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(ARGBtoABGR(*buffer));
				if (method == input_method_int::color_pick_wheel)
					flags |= ImGuiColorEditFlags_PickerHueWheel;
				else
					flags |= ImGuiColorEditFlags_PickerHueBar;
				if (ImGui::ColorPicker3("new value (color picker)", (float*)&color, flags)) {
					*buffer = ABGRtoARGB(ImGui::ColorConvertFloat4ToU32(color));
				}
			}
		}
		ImGui::PopID();
	}
		
	void group_field_editor_float_render(field_in_tattoo_model_t* selected_field_model, field_editor_float_model_t* field_editor_float_model) {
		ImGui::PushID(" group_field_float_editor");

		if (selected_field_model->get_selected_field().has_value() && selected_field_model->get_selected_definition().has_value()
			&& field_editor_float_model->get_buffer_address().has_value() && field_editor_float_model->get_input_method().has_value()) {
			auto preview_value = input_method_label_float(field_editor_float_model->get_input_method().value());
			if (ImGui::BeginCombo("input method", preview_value.second.data(), ImGuiComboFlags_PopupAlignLeft)) {
				magic_enum::enum_for_each<input_method_float>([field_editor_float_model](auto val) {
					constexpr input_method_float method = val;
					const bool is_selected = field_editor_float_model->get_input_method().value() == method;
					if (ImGui::Selectable(input_method_label_float(method).second.data(), is_selected)) {
						field_editor_float_model->set_input_method(method);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
					});
				ImGui::EndCombo();
			}
			auto buffer = field_editor_float_model->get_buffer_address().value();
			auto method = field_editor_float_model->get_input_method().value();
			if (method == input_method_float::dec_fmt || method == input_method_float::hex_fmt) {
				ImGui::InputFloat("new value (with format string)", buffer, 0.0f, 0.0f, input_method_float_formats[method]);
			}
			else {
				ImGui::SliderFloat("new value (with slider 0 to 1)", buffer, 0.0f, 1.0f, "%f");
			}
		}
		ImGui::PopID();
	}


	void group_field_editor_string_render(field_in_tattoo_model_t* selected_field_model, field_editor_string_model_t* field_editor_string_model) {
		ImGui::PushID(" group_field_string_editor");
		if (selected_field_model->get_selected_field().has_value() && selected_field_model->get_selected_definition().has_value()
			&& field_editor_string_model->get_buffer_address().has_value()) {
			auto buffer = field_editor_string_model->get_buffer_address().value();
			ImGui::InputText("input text", buffer->data(), buffer->size());
		}
		ImGui::PopID();
	}

	void  buttons_change_field(field_in_tattoo_model_t* selected_field_model, field_editor_int_model_t* field_editor_int_model,
		field_editor_float_model_t* field_editor_float_model, field_editor_string_model_t* field_editor_string_model) {
		// ImGui::PushID("buttons_change_field");
		if (selected_field_model->get_selected_field().has_value() && selected_field_model->get_selected_definition().has_value()
			&& (field_editor_int_model->get_buffer_address().has_value() || field_editor_float_model->get_buffer_address().has_value()
				|| field_editor_string_model->get_buffer_address().has_value())) {
			auto& field = selected_field_model->get_selected_field().value();
			auto& field_def = selected_field_model->get_selected_definition().value();

			bool is_new = field.field_name == "[new]";
			if (ImGui::Button("Save")) {
				//logger::info("Save button was pressed");
				if (!is_new)
					ImGui::OpenPopup("Change field");
				else
					ImGui::OpenPopup("Add field");
			}
			if (!is_new) {
				ImGui::SameLine();
				if (ImGui::Button("Remove")) {
					ImGui::OpenPopup("Remove field");
				}
			}
		}
		// ImGui::PopID();
	}

	void popup_change_or_add_field(field_in_tattoo_model_t* selected_field_model, field_editor_int_model_t* field_editor_int_model,
		field_editor_float_model_t* field_editor_float_model, field_editor_string_model_t* field_editor_string_model, actor_model_t* actor_model,
		selected_slot_model_t* selected_slot_model, ImGuiID idSuccess, ImGuiID idError) {

		if (selected_field_model->get_selected_field().has_value() && selected_field_model->get_selected_definition().has_value()
			&& actor_model->get_actor() && selected_slot_model->selected_slot().has_value()
			&& (field_editor_int_model->get_buffer_address().has_value() || field_editor_float_model->get_buffer_address().has_value()
				|| field_editor_string_model->get_buffer_address().has_value())) {

			auto& field = selected_field_model->get_selected_field().value();
			auto& field_def = selected_field_model->get_selected_definition().value();

			std::string field_name;
			std::string popup_title;
			if (field.field_name != "[new]") {
				field_name = field.field_name;
				popup_title = "Change field";
			}
			else {
				field_name = field_def.name;
				popup_title = "Add field";
			}

			int tattoo_id = field.tattoo_id;

			auto actor_label = actor_model->get_actor_label();
			auto& slot_info = selected_slot_model->selected_slot().value();

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

			if (ImGui::BeginPopupModal(popup_title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				auto text(
					std::format(
						"{} ?\n\n"
						"  Actor: {}\n"
						"  Slot: {}\n"
						"  Field: {}\n", popup_title, actor_label, slot_info.label, field_name)
				);
				ImGui::Text(text.c_str());
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					bool result = false;
					if (field_editor_int_model->get_buffer_address().has_value()) {
						int new_value = *field_editor_int_model->get_buffer_address().value();
						JMap::setInt(tattoo_id, field_name, new_value);
						result = JMap::hasKey(tattoo_id, field_name) && JMap::getInt(tattoo_id, field_name) == new_value;
					}
					else if (field_editor_float_model->get_buffer_address().has_value()) {
						float new_value = *field_editor_float_model->get_buffer_address().value();
						JMap::setFlt(tattoo_id, field_name, new_value);
						result = JMap::hasKey(tattoo_id, field_name) && JMap::getFlt(tattoo_id, field_name) == new_value;
					}
					else {
						auto new_value = field_editor_string_model->get_buffer_address().value();
						RE::BSFixedString papyrus_str(new_value->data());
						JMap::setStr(tattoo_id, field_name, papyrus_str);
						result = JMap::hasKey(tattoo_id, field_name) && JMap::getStr(tattoo_id, field_name) == papyrus_str; // Check if this was successful (RE::BSFixedString is a bit problematic)
					}
					ImGui::CloseCurrentPopup();
					if (result) 
						ImGui::OpenPopup(idSuccess);
					else
						ImGui::OpenPopup(idError);
				}
				ImGui::SameLine();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	}

	void popup_remove_field(field_in_tattoo_model_t* selected_field_model, actor_model_t* actor_model, selected_slot_model_t* selected_slot_model,
		ImGuiID idSuccess, ImGuiID idError) {

		if (selected_field_model->get_selected_field().has_value() && selected_field_model->get_selected_definition().has_value()
			&& actor_model->get_actor() && selected_slot_model->selected_slot().has_value()) {

			auto& field = selected_field_model->get_selected_field().value();
			auto& field_def = selected_field_model->get_selected_definition().value();

			std::string field_name = field.field_name;
			std::string popup_title("Remove field");
			int tattoo_id = field.tattoo_id;

			auto actor_label = actor_model->get_actor_label();
			auto& slot_info = selected_slot_model->selected_slot().value();

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

			if (ImGui::BeginPopupModal(popup_title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				auto text(
					std::format(
						"{} ?\n\n"
						"  Actor: {}\n"
						"  Slot: {}\n"
						"  Field: {}\n", popup_title, actor_label, slot_info.label, field_name)
				);
				ImGui::Text(text.c_str());
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					bool result = false;
					JMap::removeKey(tattoo_id, field_name);
					result = !JMap::hasKey(tattoo_id, field_name);
					ImGui::CloseCurrentPopup();
					if (result)
						ImGui::OpenPopup(idSuccess);
					else
						ImGui::OpenPopup(idError);
				}
				ImGui::SameLine();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	}

	void win_field_editor_render(bool* window_shown) {
		if (!ImGui::Begin("[SlaveTats] Field Editor", window_shown)) {
			ImGui::End();
			return;
		}

		//std::thread t1
		//{
		//	[&] {

		static win_field_editor_models* models = new win_field_editor_models();
		
		ImGuiID successPopupId;
		ImGuiID errorPopupId;
		bool onSuccess = false;
		begin_confirmation_popups(&successPopupId, &errorPopupId, &onSuccess);
		if (onSuccess) {
			refresh_models(models->actor_model, models->actor_tattoos_model, models->nioverride_model);
		}

		ImGui::PushID("win_field_editor");

		slavetats_model_t::GetSingleton()->refresh();

		combo_actor_selector_render(models->actor_model);

		// Show tattoo version of the selected actor
		if (!models->actor_tattoos_model->tattoo_version().empty()) {
			ImGui::SameLine();
			ImGui::Text("(v%s)", models->actor_tattoos_model->tattoo_version().c_str());
		}

		ImGui::SameLine();
		combo_area_selector_render(models->area_model);

		combo_slot_selector_render(false, models->slots_model, models->area_model, models->selected_slot_model);

		ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

		table_field_selector_render(table_flags, models->actor_tattoos_model, models->selected_slot_model, models->field_in_tattoo_model);

		combo_field_definition_selector_render(models->actor_tattoos_model, models->field_in_tattoo_model);

		buttons_change_field(models->field_in_tattoo_model, models->field_editor_int_model, models->field_editor_float_model, models->field_editor_string_model);

		group_field_editor_int_render(models->field_in_tattoo_model, models->field_editor_int_model);
		group_field_editor_float_render(models->field_in_tattoo_model, models->field_editor_float_model);
		group_field_editor_string_render(models->field_in_tattoo_model, models->field_editor_string_model);

		popup_change_or_add_field(models->field_in_tattoo_model, models->field_editor_int_model, models->field_editor_float_model, models->field_editor_string_model, 
			models->actor_model, models->selected_slot_model, successPopupId, errorPopupId);
		popup_remove_field(models->field_in_tattoo_model, models->actor_model, models->selected_slot_model, successPopupId, errorPopupId);

		ImGui::PopID();

		ImGui::End();

		//		}
		//};
		//t1.join();
	}

}
