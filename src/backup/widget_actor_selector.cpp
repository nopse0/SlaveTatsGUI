#include "widget_actor_selector.h"
#include "imgui.h"
#include "model_and_views.h"

namespace slavetats_ui
{

	void widget_actor_selector::render() {
        auto actors = find_actors();
        std::vector<std::string> items;
        for (auto actor : actors) {
            const auto base = actor->GetActorBase();
            items.push_back(std::format("{:#010x} ({})", actor->GetFormID(), base->GetName()));
        }

        // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
        actor_label = items[actor_selected_index];
        actor = actors[actor_selected_index];

        const char* combo_preview_value = actor_label.c_str();
        //ImVec2 vec = ImGui::GetContentRegionAvail();
        //ImGui::SetNextItemWidth(vec.x * 0.3);
        if (ImGui::BeginCombo("actor", combo_preview_value, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
        {
            for (int n = 0; n < items.size(); n++)
            {
                const bool is_selected = (actor_selected_index == n);
                if (ImGui::Selectable(items[n].c_str(), is_selected)) {
                    actor_selected_index = n;
                    actor_label = items[n];
                    actor = actors[n];
                }
                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }



	}

}
