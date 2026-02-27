#include "widget_slot_selector.h"
#include "imgui.h"
#include "nioverride_wrapper.h"
#include "slavetats_util.h"


namespace slavetats_ui
{
	void widget_slavetats_slot_selector::render(tattoo_area area, const jid_by_area_slot_t& tattoo_ids, const jactor_tattoos_t& tattoos) {
		ImGui::PushID(this);

        int num_slots = get_num_slots(area);

        auto area_name = std::string(magic_enum::enum_name(area));
        std::vector<slot_descriptor> slots;
        for (int i = 0; i < num_slots; i++) {
            auto tkey = tattoo_key{ area, i };
            auto jid = tattoo_ids.find(tkey);
            if (jid != tattoo_ids.end()) {
                auto it = tattoos.find(jid->second);
                if (it != tattoos.end()) {
                    auto name_it = it->second.find("name");
                    std::string name;
                    if (name_it != it->second.end())
                        name = jvalue_as_string(name_it->second);
                    auto section_it = it->second.find("section");
                    std::string section;
                    if (section_it != it->second.end())
                        section = jvalue_as_string(section_it->second);
                    int tattoo_id = 0;
                    tattoo_id = jid->second;
                    slots.push_back(slot_descriptor{ i, name, section, tattoo_id });
                }
            }
        }

        if (slot_selected_index >= slots.size()) {
            slot_selected_index = 0;
        }
        if (slots.size() == 0) {
            slot_info = slot_descriptor{};
        }
        else {
            slot_info = slots[slot_selected_index];
        }

        if (slots.size() > 0) {
            std::string slot_label = std::format("{} - (\"{}\" / \"{}\")", slot_info.slot_num, slot_info.name, slot_info.section);
            const char* slot_preview_value = slot_label.c_str();
            ImVec2 vec = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(vec.x * 0.6);
            if (ImGui::BeginCombo("slot", slot_preview_value, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
            {
                for (int n = 0; n < slots.size(); n++)
                {
                    auto& descr = slots[n];
                    std::string slot_label = std::format("{} - (\"{}\" / \"{}\")", descr.slot_num, descr.name, descr.section);
                    const bool is_selected = (slot_selected_index == n);
                    if (ImGui::Selectable(slot_label.c_str(), is_selected)) {
                        slot_selected_index = n;
                        slot_info = slots[slot_selected_index];
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

		ImGui::PopID();
	}

    void widget_available_slot_selector::render(tattoo_area area, const jid_by_area_slot_t& tattoo_ids, const jactor_tattoos_t& tattoos, 
        const std::optional<actor_overrides_t>& overrides) {
        
        ImGui::PushID(this);

        int num_slots = get_num_slots(area);

        auto area_name = std::string(magic_enum::enum_name(area));
        std::vector<slot_descriptor> slots;
        for (int i = 0; i < num_slots; i++) {
            auto tkey = tattoo_key{ area, i };
            auto jid = tattoo_ids.find(tkey);
            if (jid != tattoo_ids.end()) {
                auto it = tattoos.find(jid->second);
                if (it != tattoos.end()) {
                    auto name_it = it->second.find("name");
                    std::string name;
                    if (name_it != it->second.end())
                        name = jvalue_as_string(name_it->second);
                    auto section_it = it->second.find("section");
                    std::string section;
                    if (section_it != it->second.end())
                        section = jvalue_as_string(section_it->second);
                    int tattoo_id = 0;
                    tattoo_id = jid->second;
                    slots.push_back(slot_descriptor{ i, name, section, tattoo_id });
                }
            }
            else if (!overrides.has_value() || !overrides.value().contains(tkey)) {
                slots.push_back(slot_descriptor{ i });
            }
        }

        if (slots.size() == 0) {
            slot_info.reset();
        }
        else if (!slot_info.has_value())
        {
            slot_info = slots[0];
        }

        if (slots.size() > 0) {
            std::string slot_label;
            if (slot_info.value().name.empty())
                slot_label = slot_label = std::format("{} - <empty>", slot_info.value().slot_num);
            else
                slot_label = std::format("{} - (\"{}\" / \"{}\")", slot_info.value().slot_num, slot_info.value().name, slot_info.value().section);
            if (ImGui::BeginCombo("slot", slot_label.c_str(), ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
            {
                for (int n = 0; n < slots.size(); n++)
                {
                    auto& descr = slots[n];
                    std::string slot_label;
                    if (descr.name.empty())
                        slot_label = std::format("{} - <empty>", descr.slot_num);
                    else
                        slot_label = std::format("{} - (\"{}\" / \"{}\")", descr.slot_num, descr.name, descr.section);
                    const bool is_selected = (descr == slot_info);
                    if (ImGui::Selectable(slot_label.c_str(), is_selected)) {
                        slot_info = descr;
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::PopID();
    }

}
