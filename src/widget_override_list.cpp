#include "widget_override_list.h"
#include "nioverride_utils.h"
#include "imgui.h"

namespace slavetats_ui
{
	void  widget_override_list::render(tattoo_area area, const actor_overrides_t& overrides, const jactor_tattoos_t& jtattoos_by_id, const jid_by_area_slot_t& jtattoos_by_key) const {
		ImGui::PushID(this);
	
		int num_slots = get_num_slots(area);

		ImGuiTreeNodeFlags slot_tree_flags = ImGuiTreeNodeFlags_Framed; // | ImGuiTreeNodeFlags_DefaultOpen;

		if (ImGui::TreeNodeEx("Slots", slot_tree_flags)) {
			int num_slots = get_num_slots(area);
			for (int slot = 0; slot < num_slots; slot++) {
				// NiOverride display
				auto oit = overrides.find(tattoo_key{ area, slot });
				// SlaveTats display
				std::optional<const jfields_t*> jfields_opt;
				auto jid = jtattoos_by_key.find(tattoo_key{ area, slot });
				if (jid != jtattoos_by_key.end()) {
					auto jit = jtattoos_by_id.find(jid->second);
					if (jit != jtattoos_by_id.end()) {
						jfields_opt = &(jit->second);
					}
				}

				if (oit != overrides.end() || jfields_opt.has_value()) {
					auto title = std::format("{} {}", magic_enum::enum_name(area), slot);
					if (ImGui::TreeNodeEx(title.c_str(), slot_tree_flags))
					{
						// NiOverride display
						if (oit != overrides.end()) {
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

									for (auto& ovl : oit->second) {
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
						} // NiOverride end

						// SlaveTats display
						if (jfields_opt.has_value()) {
							if (ImGui::TreeNodeEx("SlaveTats", slot_tree_flags)) {
								ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
									ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
								if (ImGui::BeginTable("overrides", 3, table_flags)) {
									ImGui::TableSetupColumn("key", ImGuiTableColumnFlags_WidthFixed);
									ImGui::TableSetupColumn("type", ImGuiTableColumnFlags_WidthFixed);
									ImGui::TableSetupColumn("value");
									ImGui::TableHeadersRow();
									for (auto& field : *jfields_opt.value()) {
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
		ImGui::PopID();
	}

}
