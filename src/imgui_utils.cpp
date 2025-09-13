#include "imgui_utils.h"
#include "imgui.h"

namespace slavetats_ui {
	void show_confirmation_popups(bool* confirmed, bool* success_shown, bool* error_shown) {
		if (ImGui::BeginPopupModal("Success", success_shown, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Operation was successful.");
			if (ImGui::Button("Close", ImVec2(120, 0))) {
				*confirmed = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Error", error_shown, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Operation failed !!!");
			if (ImGui::Button("Close", ImVec2(120, 0))) {
				*confirmed = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
