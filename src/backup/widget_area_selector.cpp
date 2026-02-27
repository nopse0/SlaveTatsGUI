#include "widget_area_selector.h"
#include "imgui.h"
#include "slavetats_util.h"

namespace slavetats_ui
{
	void widget_area_selector::render() {
        ImGui::PushID(this);

        // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
        const char* area_preview_value = magic_enum::enum_name(area_selected_value).data();

        //ImVec2 vec = ImGui::GetContentRegionAvail();
        //ImGui::SetNextItemWidth(vec.x * 0.1);
        if (ImGui::BeginCombo("area", area_preview_value, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
        {
            for (int n = 0; n < tattoo_areas.size(); n++)
            {
                const bool is_selected = (magic_enum::enum_index(area_selected_value) == n);
                if (ImGui::Selectable(tattoo_areas[n].data(), is_selected)) {
                    area_selected_value = magic_enum::enum_value<tattoo_area>(n);
                }
                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
	
        ImGui::PopID();
    }
}
