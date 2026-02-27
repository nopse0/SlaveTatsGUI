#include <imgui.h>
#include "widget_int_editor.h"
#include "jcontainers_wrapper.h"
#include "imgui_utils.h"

namespace slavetats_ui
{
    using namespace slavetats_ng::jcwrapper;

    void widget_int_editor::render(int current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key, bool is_new)
    {
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
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(ARGBtoABGR(_int_new));
            if (input_method_index == integer_input_method::color_pick_wheel)
                flags |= ImGuiColorEditFlags_PickerHueWheel;
            else
                flags |= ImGuiColorEditFlags_PickerHueBar;
            if (ImGui::ColorPicker3("new value (color picker)", (float*)&color, flags)) {
                _int_new = ABGRtoARGB(ImGui::ColorConvertFloat4ToU32(color));
            }
        }
        else {
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(ARGBtoABGR(_int_new));
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
                    _int_new = ABGRtoARGB(skyui_palette[n]);
                }
                ImGui::PopID();
            }
            ImGui::EndGroup();
        }

        // Save dialog

        if (ImGui::Button("Save")) {
            //logger::info("Save button was pressed");
            if (!is_new)
                ImGui::OpenPopup("Change field value");
            else
                ImGui::OpenPopup("Insert new key/value pair");
        }
        if (!is_new) {
            ImGui::SameLine();
            if (ImGui::Button("Remove")) {
                ImGui::OpenPopup("Remove field");
            }
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

        if (ImGui::BeginPopupModal("Change field value", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::string text(std::format(
                "Change value of field {} in tattoo with id {} ?\n"
                "Target actor is {}\n"
                "Old value: {} (hex: {:08x})\n"
                "New value: {} (hex: {:08x})\n",
                field_key, tattoo_id, actor_label, _int_old, _int_old, _int_new, _int_new));
            ImGui::Text(text.c_str());

            bool confirmed = false;
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                JMap::setInt(tattoo_id, field_key, _int_new);
                // Check if this was successful
                bool result = JMap::hasKey(tattoo_id, field_key) && JMap::getInt(tattoo_id, field_key) == _int_new;
                if (result)
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
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal("Insert new key/value pair", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::string text(std::format(
                "Insert new field {} into tattoo with id {} ?\n"
                "Target actor is {}\n"
                "Value of new field: {} (hex: {:08x})\n",
                field_key, tattoo_id, actor_label, _int_new, _int_new));
            ImGui::Text(text.c_str());

            bool confirmed = false;
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                JMap::setInt(tattoo_id, field_key, _int_new);
                // Check if this was successful
                bool result = JMap::hasKey(tattoo_id, field_key) && JMap::getInt(tattoo_id, field_key) == _int_new;
                if (result)
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
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Remove field", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::string text(std::format(
                "Remove field {} from tattoo with id {} ?\n"
                "Target actor is {}\n"
                "Current field value is: {} (hex: {:08x})\n",
                field_key, tattoo_id, actor_label, _int_old, _int_old));
            ImGui::Text(text.c_str());

            bool confirmed = false;
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                JMap::removeKey(tattoo_id, field_key);
                // Check if this was successful
                bool result = JMap::hasKey(tattoo_id, field_key);
                if (result)
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
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

}
