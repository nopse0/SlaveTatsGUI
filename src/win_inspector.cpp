#include "win_inspector.h"

#include "imgui.h"
#include "model_and_views.h"
#include "combos.h"

namespace slavetats_ui {

	struct win_inspector_models {
		actor_model_t* actor_model;
		area_model_t* area_model;
		nioverride_model_t* nioverride_model;
		applied_tattoos_model_t* actor_tattoos_model;

		win_inspector_models();
		~win_inspector_models();
	};

	win_inspector_models::win_inspector_models() {
		actor_model = new actor_model_t();
		area_model = new area_model_t();
		nioverride_model = new nioverride_model_t(actor_model);
		actor_tattoos_model = new applied_tattoos_model_t(actor_model);
	}

	win_inspector_models::~win_inspector_models() {
		delete actor_tattoos_model;
		delete nioverride_model;
		delete area_model;
		delete actor_model;
	}

	void tree_overlay_infos_render(area_model_t* area_model, applied_tattoos_model_t* applied_tattoos, nioverride_model_t* nioverride_model) {

		if (area_model->get_area() != tattoo_area::no_value, applied_tattoos->tattoos().has_value() && applied_tattoos->tattoo_ids().has_value()
			&& nioverride_model->overrides().has_value()) {

			auto& tattoos = applied_tattoos->tattoos().value();
			auto& tattoo_ids = applied_tattoos->tattoo_ids().value();
			auto& overrides = nioverride_model->overrides().value();
			auto area = area_model->get_area();
			
			ImGuiTreeNodeFlags slot_tree_flags = ImGuiTreeNodeFlags_Framed; // | ImGuiTreeNodeFlags_DefaultOpen;

			auto num_slots = get_num_slots(area);
			if (ImGui::TreeNodeEx("Slots", slot_tree_flags | ImGuiTreeNodeFlags_DefaultOpen)) {
				for (int slot = 0; slot < num_slots; slot++) {
					// NiOverride display
					auto override_it = overrides.find(tattoo_key{ area, slot });
					// SlaveTats display
					std::optional<const jfields_t*> tattoo_fields_opt;
					auto tattoo_id_it = tattoo_ids.find(tattoo_key{ area, slot });
					if (tattoo_id_it != tattoo_ids.end()) {
						auto tattoo_it = tattoos.find(tattoo_id_it->second);
						if (tattoo_it != tattoos.end()) {
							tattoo_fields_opt = &(tattoo_it->second);
						}
					}

					if (override_it != overrides.end() || tattoo_fields_opt.has_value()) {

						auto title = std::format("{} {}", magic_enum::enum_name(area), slot);
						if (ImGui::TreeNodeEx(title.c_str(), slot_tree_flags))
						{
							// NiOverride part
							if (override_it != overrides.end()) {
								if (ImGui::TreeNodeEx("NiOverride", slot_tree_flags)) {

									ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
										ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

									if (ImGui::BeginTable("overrides", 5, table_flags)) {
										ImGui::TableSetupColumn("key", ImGuiTableColumnFlags_WidthFixed);
										ImGui::TableSetupColumn("index", ImGuiTableColumnFlags_WidthFixed);
										ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed);
										ImGui::TableSetupColumn("type", ImGuiTableColumnFlags_WidthFixed);
										ImGui::TableSetupColumn("value");
										ImGui::TableHeadersRow();

										for (auto& ovl : override_it->second) {
											ImGui::TableNextRow();

											ImGui::TableSetColumnIndex(0);
											ImGui::TextUnformatted(std::format("{}", ovl.first.key).c_str());

											ImGui::TableSetColumnIndex(1);
											ImGui::TextUnformatted(std::format("{}", ovl.first.index).c_str());

											auto ovl_def = ni_override_definitions.find(ovl.first);
											if (ovl_def != ni_override_definitions.end()) {
												ImGui::TableSetColumnIndex(2);
												ImGui::TextUnformatted(ovl_def->second.name);
												switch (ovl_def->second.type) {
												case ni_override_type::ni_int:
													ImGui::TableSetColumnIndex(3);
													ImGui::TextUnformatted("ni_int");
													ImGui::TableSetColumnIndex(4);
													ImGui::TextUnformatted(std::format("{}", std::get<int>(ovl.second)).c_str());
													break;
												case ni_override_type::ni_float:
													ImGui::TableSetColumnIndex(3);
													ImGui::TextUnformatted("ni_float");
													ImGui::TableSetColumnIndex(4);
													ImGui::TextUnformatted(std::format("{}", std::get<float>(ovl.second)).c_str());
													break;
												case ni_override_type::ni_string:
													ImGui::TableSetColumnIndex(3);
													ImGui::TextUnformatted("ni_string");
													ImGui::TableSetColumnIndex(4);
													ImGui::TextUnformatted(std::get<std::string>(ovl.second).c_str());
													break;
												case ni_override_type::ni_texture_set:
													ImGui::TableSetColumnIndex(3);
													ImGui::TextUnformatted("ni_texture_set");
													ImGui::TableSetColumnIndex(4);
													ImGui::TextUnformatted("??? texture set ???");
													break;
												}
											}
										}
										ImGui::EndTable();
									}
									ImGui::TreePop();
								}
							} // NiOverride part end

							// SlaveTats part
							if (tattoo_fields_opt.has_value()) {
								if (ImGui::TreeNodeEx("SlaveTats", slot_tree_flags)) {
									ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
										ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
									if (ImGui::BeginTable("overrides", 3, table_flags)) {
										ImGui::TableSetupColumn("key", ImGuiTableColumnFlags_WidthFixed);
										ImGui::TableSetupColumn("type", ImGuiTableColumnFlags_WidthFixed);
										ImGui::TableSetupColumn("value");
										ImGui::TableHeadersRow();
										for (auto& field : *tattoo_fields_opt.value()) {
											ImGui::TableNextRow();
											ImGui::TableSetColumnIndex(0);
											ImGui::TextUnformatted(field.first.c_str());
											auto& jvalue = field.second;
											if (jvalue.type == jvalue_type::j_int) {
												ImGui::TableSetColumnIndex(1);
												ImGui::TextUnformatted("Int");
												ImGui::TableSetColumnIndex(2);
												ImGui::TextUnformatted(std::format("{}", std::get<int>(jvalue.value)).c_str());
											}
											else if (jvalue.type == jvalue_type::j_float) {
												ImGui::TableSetColumnIndex(1);
												ImGui::TextUnformatted("Float");
												ImGui::TableSetColumnIndex(2);
												ImGui::TextUnformatted(std::format("{}", std::get<float>(jvalue.value)).c_str());
											}
											else if (jvalue.type == jvalue_type::j_string) {
												ImGui::TableSetColumnIndex(1);
												ImGui::TextUnformatted("String");
												ImGui::TableSetColumnIndex(2);
												ImGui::TextUnformatted(std::get<std::string>(jvalue.value).c_str());
											}
											else {
												ImGui::TableSetColumnIndex(1);
												ImGui::TextUnformatted(magic_enum::enum_name(jvalue.type).data());
												ImGui::TableSetColumnIndex(2);
												ImGui::TextUnformatted("???");
											}
										}
										ImGui::EndTable();
									}
									ImGui::TreePop();
								}

							}

							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}
		}
	}

	void win_inspector_render(bool* window_shown) {

		if (!ImGui::Begin("[SlaveTats] Inspector", window_shown)) {
			ImGui::End();
			return;
		}

		//std::thread t1
		//{
		//	[&] {

		static win_inspector_models* models = new win_inspector_models();

		ImGui::PushID("win_inspector");

		slavetats_model_t::GetSingleton()->refresh();

		combo_actor_selector_render(models->actor_model);

		// Show tattoo version of the selected actor
		if (!models->actor_tattoos_model->tattoo_version().empty()) {
			ImGui::SameLine();
			ImGui::Text("(v%s)", models->actor_tattoos_model->tattoo_version().c_str());
		}

		ImGui::SameLine();
		combo_area_selector_render(models->area_model);

		tree_overlay_infos_render(models->area_model, models->actor_tattoos_model, models->nioverride_model);

		ImGui::PopID();

		ImGui::End();


		//            }
		//        };
		//        t1.join();

	}

}
