#include "widget_new_field_selector.h"
#include "imgui.h"

namespace slavetats_ui
{
	void widget_new_field_selector::render(const field_db::field_db_t& field_defs) {
		ImGui::PushID(this);

        if (!field_defs.contains(selected_field.name))
            selected_field = field_db::field_definition{};

        if (selected_field.name.empty()) {
            if (field_defs.begin() != field_defs.end()) {
                selected_field = field_defs.begin()->second;
            }
        }

        if (!selected_field.name.empty()) {
            const char* combo_preview_value = selected_field.name.c_str();
            ImVec2 vec = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(vec.x * 0.3);
            if (ImGui::BeginCombo("new field", combo_preview_value, ImGuiComboFlags_PopupAlignLeft)) {
                for (auto& fd: field_defs)
                {
                    const bool is_selected = (fd.second.name == selected_field.name);
                    if (ImGui::Selectable(fd.second.name.c_str(), is_selected)) {
                        selected_field = fd.second;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

		ImGui::PopID();
	};

}
