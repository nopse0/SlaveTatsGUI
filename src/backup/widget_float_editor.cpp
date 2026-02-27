#include "widget_float_editor.h"
#include "imgui.h"
#include "imgui_utils.h"
#include "jcontainers_wrapper.h"

namespace slavetats_ui
{
    using namespace slavetats_ng::jcwrapper;

	void widget_float_editor::render(float current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key, bool is_new) {

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

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

        // Save dialog

        if (ImGui::Button("Save")) {
            //logger::info("Save Button was pressed");
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

        center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

        if (ImGui::BeginPopupModal("Change field value", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::string text(std::format(
                "Change value of field {} in tattoo with id {} ?\n"
                "Target actor is {}\n"
                "Old value: {} (hex: {:a})\n"
                "New value: {} (hex: {:a})\n",
                field_key, tattoo_id, actor_label, _float_old, _float_old, _float_new, _float_new));
            ImGui::Text(text.c_str());

            bool confirmed = false;
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                JMap::setFlt(tattoo_id, field_key, _float_new);
                // Check if this was successful
                bool result = JMap::hasKey(tattoo_id, field_key) && JMap::getFlt(tattoo_id, field_key) == _float_new;
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
                "Value of new field: {} (hex: {:a})\n",
                field_key, tattoo_id, actor_label, _float_new, _float_new));
            ImGui::Text(text.c_str());

            bool confirmed = false;
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                JMap::setFlt(tattoo_id, field_key, _float_new);
                // Check if this was successful
                bool result = JMap::hasKey(tattoo_id, field_key) && JMap::getFlt(tattoo_id, field_key) == _float_new;
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
                "Current field value is: {} (hex: {:a})\n",
                field_key, tattoo_id, actor_label, _float_old, _float_old));
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
