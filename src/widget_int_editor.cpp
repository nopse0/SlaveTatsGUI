#include <imgui.h>
#include "widget_int_editor.h"

namespace slavetats_ui
{

    void widget_int_editor::render(int current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key)
    {
        std::stringstream ss;
        ss << "actor: " << actor_label << ", tattoo_id: " << tattoo_id << std::endl << "field: " << field_key <<
            ", current value: " << current_value << "(hex: " << std::format("{:08x}", current_value) << "), type: int";
        std::string save_description = ss.str();
        ImGui::TextUnformatted(save_description.c_str());

        if (current_value != _int_old) {
            _int_old = current_value;
            _int_new = current_value;
        }

        ImGui::PushID(this);
       
        if (ImGui::BeginCombo("input method", input_method_labels_int[magic_enum::enum_index(input_method_index).value()].second.data(), 
            ImGuiComboFlags_PopupAlignLeft))
        {
            for (int n = 0; n < input_method_labels_int.size(); n++)
            {
                const bool is_selected = (magic_enum::enum_index(input_method_index).value() == n);
                if (ImGui::Selectable(input_method_labels_int[n].second.data(), is_selected)) {
                    input_method_index = input_method_labels_int[n].first;
                    input_method_label = input_method_labels_int[n].second.data();
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // The first entries of 'integer_input_method' specify format strings
        if(magic_enum::enum_index(input_method_index).value() < format_strings_int.size()) {
            ImGui::InputScalar("new value (with format string)", ImGuiDataType_U32, &_int_new, NULL, NULL, 
                format_strings_int[magic_enum::enum_index(input_method_index).value()]);
        }
        else if (input_method_index != integer_input_method::color_pick_palette) {
            ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV |
                ImGuiColorEditFlags_DisplayHex;
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(_int_new);
            if (input_method_index == integer_input_method::color_pick_wheel)
                flags |= ImGuiColorEditFlags_PickerHueWheel;
            else
                flags |= ImGuiColorEditFlags_PickerHueBar;
            if (ImGui::ColorPicker3("new value (color picker)", (float*)&color, flags)) {
                _int_new = ImGui::ColorConvertFloat4ToU32(color);
            }
        }
        else {
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(_int_new);
            ImGui::BeginGroup(); // Lock X position
            ImGui::Text("Current");
            ImGui::ColorButton("##color button", color, ImGuiColorEditFlags_NoPicker, ImVec2(60, 40));
            ImGui::Separator();
            ImGui::Text("Palette");
            for (int n = 0; n < skyui_palette.size(); n++)
            {
                ImGui::PushID(n);
                if (n % (skyui_palette.size()/4) != 0)
                    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);
                ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
                if (ImGui::ColorButton("##palette", skyui_palette_vec4[n], palette_button_flags, ImVec2(60, 60))) {
                    _int_new = skyui_palette[n];
                }
                ImGui::PopID();
            }
            ImGui::EndGroup();
        }

        // Save dialog

        if (ImGui::Button("Save")) {
            logger::info("Save Button was pressed");
            ImGui::OpenPopup("Save int attribute");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

        if (ImGui::BeginPopupModal("Save int attribute", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(save_description.c_str());
            ImGui::Text("New value %d (hex: %x)", _int_new, _int_new);
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
