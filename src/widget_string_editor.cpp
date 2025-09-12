#include "widget_string_editor.h"

#include "imgui.h"
#include "imgui_stdlib.h"

namespace slavetats_ui
{

	void widget_string_editor::render(const std::string& current_value, const std::string& actor_label, const int tattoo_id, const std::string& field_key) {

        std::stringstream ss;
        ss << "actor: " << actor_label << ", tattoo_id: " << tattoo_id << std::endl << "field: " << field_key <<
            ", current value: '" << current_value << "', type: string";
        std::string save_description = ss.str();
        ImGui::TextUnformatted(save_description.c_str());

        if (current_value != _string_old) {
            _string_old = current_value;
            // _string_new = current_value;
            int count = current_value.size() < ARRAYSIZE(_char_buffer) ? current_value.size() : ARRAYSIZE(_char_buffer) - 1;
            memcpy(_char_buffer, current_value.data(), count);
            _char_buffer[count] = '\0';
        }

        // ImGui::InputText("new value", &_string_new);

        //static char _buffer[2048] = "Hello, world!";
        ImGui::InputText("input text", _char_buffer, ARRAYSIZE(_char_buffer));

        // Save dialog

        if (ImGui::Button("Save")) {
            logger::info("Save Button was pressed");
            _string_new = std::string(_char_buffer);
            ImGui::OpenPopup("Save string attribute");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

        if (ImGui::BeginPopupModal("Save string attribute", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(save_description.c_str());
            ImGui::Text("New value '%s'", _string_new.c_str());
            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }


        ImGui::PushID(this);


	}
}
