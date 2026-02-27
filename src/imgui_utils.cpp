#include "imgui_utils.h"
#include "imgui.h"

namespace slavetats_ui {

	void begin_confirmation_popups(ImGuiID* successPopupId, ImGuiID* errorPopupId, bool* onSuccess) {
		*successPopupId = ImGui::GetID("Success");
		if (ImGui::BeginPopupModal("Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			*onSuccess = false;
			ImGui::Text("Operation was successful.");
			if (ImGui::Button("Close", ImVec2(120, 0))) {
				*onSuccess = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		*errorPopupId = ImGui::GetID("Error");
		if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Operation failed !!!");
			if (ImGui::Button("Close", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

}
