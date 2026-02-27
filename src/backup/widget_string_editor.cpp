#include "widget_string_editor.h"
#include "imgui.h"
//#include "imgui_stdlib.h"
#include "imgui_utils.h"
#include "jcontainers_wrapper.h"

namespace slavetats_ui
{
    using namespace slavetats_ng::jcwrapper;

	void widget_string_editor::render(const std::string& current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key, bool is_new) {

        ImGui::PushID(this);
        
        if (current_value != _string_old) {
            _string_old = current_value;
            // _string_new = current_value;
            int count = current_value.size() < ARRAYSIZE(_char_buffer) ? current_value.size() : ARRAYSIZE(_char_buffer) - 1;
            memcpy(_char_buffer, current_value.data(), count);
            _char_buffer[count] = '\0';
        }

        ImGui::InputText("input text", _char_buffer, ARRAYSIZE(_char_buffer));

        // Save dialog

        if (ImGui::Button("Save")) {
            //logger::info("Save Button was pressed");
            _string_new = std::string(_char_buffer);
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
                "Old value: '{}'\n"
                "New value: '{}'\n",
                field_key, tattoo_id, actor_label, _string_old, _string_new));
            ImGui::Text(text.c_str());

            bool confirmed = false;
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                RE::BSFixedString papyrus_str(_string_new);
                JMap::setStr(tattoo_id, field_key, papyrus_str);
                // Check if this was successful (RE::BSFixedString is a problem)
                bool result = JMap::hasKey(tattoo_id, field_key) && JMap::getStr(tattoo_id, field_key) == papyrus_str;
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
                "Value of new field:' '{}'\n",
                field_key, tattoo_id, actor_label, _string_new));
            ImGui::Text(text.c_str());

            bool confirmed = false;
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                RE::BSFixedString papyrus_str(_string_new);
                JMap::setStr(tattoo_id, field_key, papyrus_str);
                // Check if this was successful (RE::BSFixedString is a problem)
                bool result = JMap::hasKey(tattoo_id, field_key) && JMap::getStr(tattoo_id, field_key) == papyrus_str;
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
                "Current field value is: '{}'\n",
                field_key, tattoo_id, actor_label, _string_old));
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
