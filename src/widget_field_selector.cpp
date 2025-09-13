#include "widget_field_selector.h"
#include "imgui.h"
#include "slavetats_util.h"
#include "tattoo_field_db.h"

namespace slavetats_ui
{
	void widget_field_selector::render(ImGuiTableFlags flags, const jfields_t& jfields) {
		ImGui::PushID(this);
		
        if (ImGui::BeginTable("attributes", 3, flags)) {
            ImGui::TableSetupColumn("key", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("value");
            ImGui::TableHeadersRow();
            for (auto kvit = jfields.begin(); kvit != jfields.end(); ++kvit) {
                auto key = kvit->first;
                auto value = kvit->second;
                std::string type_str(jvalue_type_pretty_names[magic_enum::enum_index(kvit->second.type).value()]);
                jvalue_type type_id = kvit->second.type;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(key.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(type_str.c_str());
                ImGui::TableSetColumnIndex(2);
                auto field_def = field_db::find_editable_attribute(key, type_id);
                auto value_string = jvalue_as_string(value);
                if (field_def.has_value()) {
                    const bool is_selected = selected_field == key;
                    if (ImGui::Selectable((value_string + "##" + key).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                        selected_field_last = selected_field;
                        selected_field = key;
                    }
                }
                else {
                    ImGui::TextDisabled(value_string.c_str());
                }
            }
            // new field line
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("[new]");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted("?");
            ImGui::TableSetColumnIndex(2);
            const bool is_selected = selected_field == "[new]";
            if (ImGui::Selectable("?", is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                selected_field_last = selected_field;
                selected_field = "[new]";
            }

            ImGui::EndTable();
        }
        
		ImGui::PopID();
	}
}
