#include "widget_float_editor.h"
#include "imgui.h"

namespace slavetats_ui
{

	void widget_float_editor::render(float current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key) {

        std::stringstream ss;
        ss << "actor: " << actor_label << ", tattoo_id: " << tattoo_id << std::endl << "field: " << field_key <<
            ", current value: " << current_value << "(hex: " << std::format("{:a}", current_value) << "), type: int";
        std::string save_description = ss.str();
        ImGui::TextUnformatted(save_description.c_str());

        if (current_value != _float_old) {
            _float_old = current_value;
            _float_new = current_value;
        }

        ImGui::PushID(this);

        if (ImGui::BeginCombo("input method", input_method_labels_float[magic_enum::enum_index(input_method_index).value()].second.data(),
            ImGuiComboFlags_PopupAlignLeft))
        {
            for (int n = 0; n < input_method_labels_float.size(); n++)
            {
                const bool is_selected = (magic_enum::enum_index(input_method_index).value() == n);
                if (ImGui::Selectable(input_method_labels_float[n].second.data(), is_selected)) {
                    input_method_index = input_method_labels_float[n].first;
                    input_method_label = input_method_labels_float[n].second.data();
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // The first entries of 'integer_input_method' specify format strings
        if (magic_enum::enum_index(input_method_index).value() < format_strings_float.size()) {
            ImGui::InputFloat("new value (with format string)", &_float_new, 0.0f, 0.0f, format_strings_float[magic_enum::enum_index(input_method_index).value()]);
        }
        else if (input_method_index == float_input_method::slider_0_to_1) {
            ImGui::SliderFloat("new value (with slider 0 to 1)", &_float_new, 0.0f, 1.0f, "%f");
        }

        if (ImGui::Button("Save")) {
            logger::info("Save Button was pressed");
            ImGui::OpenPopup("Save float attribute");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

        if (ImGui::BeginPopupModal("Save float attribute", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(save_description.c_str());
            ImGui::Text("New value %f (hex: %a)", _float_new, _float_new);
            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::PopID();
	}

}
