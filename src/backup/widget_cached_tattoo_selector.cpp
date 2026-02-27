#include "widget_cached_tattoo_selector.h"
#include "slavetats_util.h"
#include "imgui.h"

namespace slavetats_ui
{
    void widget_cached_tattoo_selector::render(const section_tattoos_t& section_tattoos, std::optional<const jfields_t*> applied_tattoo,
        const section_tattoo_names_t& section_tattoo_names) {
        ImGui::PushID(this);

        if (!section_tattoos.empty()) {
            if (section_selected.empty()) {
                // 'applied_tattoo' is the tattoo in the currently selected slot. The purpose of the following is to set the initial section and tattoo name 
                // cursor positions to the section and name of the 'applied_tattoo' (of course, it's possible that there is no tattoo in the selected slot,
                // or that the section or name of the applied tattoo isn't installed anymore (i.e. listed in the .json files in the slavetats folder))
                if (applied_tattoo.has_value()) {
                    std::string section_applied;
                    std::string name_applied;
                    auto snit = applied_tattoo.value()->find("section");
                    if (snit != applied_tattoo.value()->end()) {
                        if (snit->second.type == jvalue_type::j_string) {
                            section_applied = std::get<std::string>(snit->second.value);
                        }
                    }
                    auto nit = applied_tattoo.value()->find("name");
                    if (nit != applied_tattoo.value()->end()) {
                        if (nit->second.type == jvalue_type::j_string) {
                            name_applied = std::get<std::string>(nit->second.value);
                        }
                    }
                    if (section_tattoos.contains(section_applied)) {
                        section_selected = section_applied;
                        auto sntit = section_tattoo_names.find(section_selected);
                        if (sntit != section_tattoo_names.end()) {
                            auto namtatit = sntit->second.find(name_applied);
                            if (namtatit != sntit->second.end()) {
                                name_selected = namtatit->first;
                                tattoo_selected = namtatit->second;
                            }
                        }
                    }
                }
                else {
                    section_selected = section_tattoos.begin()->first;
                    auto ntit = section_tattoo_names.find(section_selected);
                    if (ntit != section_tattoo_names.end()) {
                        if (!ntit->second.empty()) {
                            name_selected = ntit->second.begin()->first;
                            tattoo_selected = ntit->second.begin()->second;
                        }
                    }
                }
            }
        
            if (ImGui::BeginCombo("section", section_selected.c_str(), ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightLarge))
            {
                for (auto& s : section_tattoos)
                {
                    const bool is_selected = (section_selected == s.first);
                    if (ImGui::Selectable(s.first.c_str(), is_selected)) {
                        section_selected = s.first;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        // Update selected tattoo, when the section is changed !!!
                        auto last_name = name_selected;
                        name_selected.clear();
                        tattoo_selected = 0;
                        auto sntit = section_tattoo_names.find(section_selected);
                        if (sntit != section_tattoo_names.end()) {
                            auto namtatit = sntit->second.find(last_name);
                            if (namtatit != sntit->second.end()) {
                                name_selected = namtatit->first;
                                tattoo_selected = namtatit->second;
                            }
                            else if (sntit->second.begin() != sntit->second.end()) {
                                name_selected = sntit->second.begin()->first;
                                tattoo_selected = sntit->second.begin()->second;
                            }
                        }
                    }
                }
                ImGui::EndCombo();
            }           
            
            auto secnamtatit = section_tattoo_names.find(section_selected);
            if (secnamtatit != section_tattoo_names.end()) {
                if (ImGui::BeginCombo("name", name_selected.c_str(), ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_HeightLarge))
                {
                    for (auto& nametat : secnamtatit->second) {
                        const bool is_selected = (name_selected == nametat.first);
                        if (ImGui::Selectable(nametat.first.c_str(), is_selected)) {
                            name_selected = nametat.first;
                            tattoo_selected = nametat.second;
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
        }

        ImGui::PopID();
    }
	
}
