//#include "jcontainers_wrapper.h"
//#include "nioverride_wrapper.h"
//#include "utility.h"
//#include "slavetats_util.h"
#include "tattoo_field_definitions.h"
#include "menu_api.h"
//#include "widget_editor_int.h"

#include "UI.h"
#include "window_field_editor.h"
#include "window_inspector.h"
#include "window_apply_remove.h"

namespace slavetats_ui {

    bool render_inspector() {
        bool window_shown = true;
        static slavetats_ui::window_inspector inspector;
        inspector.render(&window_shown);
        return window_shown;
    }

    bool render_apply_remove() {
        bool window_shown = true;
        static slavetats_ui::window_apply_remove apply_remove;
        apply_remove.render(&window_shown);
        return window_shown;
    }

    bool render_field_editor() {
        bool window_shown = true;
        static slavetats_ui::window_field_editor editor;
        editor.render(&window_shown);
        return window_shown;
    }

    // attrib::attribute_definitions_t attrib::attribute_definitions;
    // attrib::attribute_definitions_t attrib::attribute_definitions;

    namespace  SlaveTatsUI {

        //using namespace slavetats_ng::jcwrapper;

        void initialize() {
            field_db::load_field_db();

            logger::info("Looking for CatMenu...");

            auto result = CatMenu::RequestCatMenuAPI();
            if (result.index() == 0)
            {
                auto catmenu_api = std::get<0>(result);

                ImGui::SetCurrentContext(catmenu_api->GetContext());

                RE::BSString menu_name1{ "SlaveTats: Inspector" };
                catmenu_api->RegisterMenuDrawFunc(menu_name1, render_inspector);

                RE::BSString menu_name2{ "SlaveTats: Apply/Remove Tattoo" };
                catmenu_api->RegisterMenuDrawFunc(menu_name2, render_apply_remove);

                RE::BSString menu_name3{ "SlaveTats: Field Editor" };
                catmenu_api->RegisterMenuDrawFunc(menu_name3, render_field_editor);

                logger::info("CatMenu integration succeed!");
            }
            else {
                logger::warn("CatMenu integration failed! Doctor_SlaveTatsNG disabled. Error: {}", std::get<1>(result));
                return;
            }

            
        }


#if 0
        bool render_applied_window() {
            bool show_window = true;

            if (ImGui::Begin("Doctor SlaveTatsNG Menu", &show_window, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
            {
                auto actors = find_actors();
                std::vector<std::string> items;
                for (auto actor : actors) {
                    const auto base = actor->GetActorBase();
                    items.push_back(std::format("{:#010x} ({})", actor->GetFormID(), base->GetName()));
                }

                static int item_selected_idx = 0; // Here we store our selection data as an index.

                // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
                const char* combo_preview_value = items[item_selected_idx].c_str();
                auto vec = ImGui::GetContentRegionAvail();
                ImGui::SetNextItemWidth(vec.x * 0.3);
                if (ImGui::BeginCombo("actor", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
                {
                    for (int n = 0; n < items.size(); n++)
                    {
                        const bool is_selected = (item_selected_idx == n);
                        if (ImGui::Selectable(items[n].c_str(), is_selected)) {
                            item_selected_idx = n;
                        }
                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                std::multimap<tattoo_key, std::map<std::string, std::string>> tattoo_attributes;
                std::map<tattoo_key, int> tattoo_ids;
                if (item_selected_idx < actors.size()) {
                    auto actor = actors[item_selected_idx];
                    int tattoos = JFormDB::getObj(actor, ".SlaveTats.applied");
                    if (tattoos && JValue::isArray(tattoos)) {
                        auto count = JArray::count(tattoos);
                        while (count > 0) {
                            count--;
                            auto tattoo = JArray::getObj(tattoos, count);
                            if (tattoo) {
                                push_tattoo_attributes(tattoo, tattoo_attributes, tattoo_ids);
                            }
                        }
                    }
                }

                //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
                if (ImGui::TreeNodeEx("Applied Tattoos", base_flags)) {
                    for (auto& entry : tattoo_attributes) {
                        // ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        auto title = std::format("{} {}", entry.first.area, entry.first.slot);
                        if (ImGui::TreeNodeEx(title.c_str(), base_flags))
                        {
                            std::stringstream ss;
                            for (auto kv : entry.second) {
                                ss << kv.first << ": " << kv.second << std::endl;
                            }
                            ImGui::TextUnformatted(ss.str().c_str());
                            ImGui::TreePop();
                        }
                        //ImGui::Separator();
                    }
                    ImGui::TreePop();
                }

                ImGui::End();
            }

            return show_window;
        }
#endif

#if 0
        bool render_attributes_window() {


            static int actor_selected_idx = 0; // Here we store our selection data as an index.
            static int area_selected_idx = 0; // Here we store our selection data as an index.
            static int slot_selected_idx = 0;

            static std::string editor_actor_label;

            static int editor_tattoo_id = 0;
            static std::string editor_attribute_key = "color";
            // We want to reset the attribute editor, when one of the above is changed 
            static int editor_tattoo_id_last = 0;
            static std::string editor_attribute_key_last = "color";

            // Attribute editor fields
            static float editor_float_old = 0.f;
            static float editor_float_new = 0.f;
            // static int editor_int_old = 0;
            // static int editor_int_new = 0;

            auto actors = find_actors();
            std::vector<std::string> items;
            for (auto actor : actors) {
                const auto base = actor->GetActorBase();
                items.push_back(std::format("{:#010x} ({})", actor->GetFormID(), base->GetName()));
            }

            // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
            editor_actor_label = items[actor_selected_idx];
            const char* combo_preview_value = editor_actor_label.c_str();
            ImVec2 vec = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(vec.x * 0.3);
            if (ImGui::BeginCombo("actor", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
            {
                for (int n = 0; n < items.size(); n++)
                {
                    const bool is_selected = (actor_selected_idx == n);
                    if (ImGui::Selectable(items[n].c_str(), is_selected)) {
                        actor_selected_idx = n;
                        editor_actor_label = items[n];
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            /*std::vector<std::string> areas;
            areas.push_back("BODY");
            areas.push_back("FACE");
            areas.push_back("FEET");
            areas.push_back("HANDS");*/

            std::array areas{ "BODY", "FACE", "FEET", "HANDS" };

            auto num_body = slavetats_ng::skee_wrapper::NiOverride::GetNumBodyOverlays();
            auto num_face = slavetats_ng::skee_wrapper::NiOverride::GetNumFaceOverlays();
            auto num_feet = slavetats_ng::skee_wrapper::NiOverride::GetNumFeetOverlays();
            auto num_hands = slavetats_ng::skee_wrapper::NiOverride::GetNumHandsOverlays();

            // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
            const char* area_preview_value = areas[area_selected_idx];

            vec = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(vec.x * 0.1);
            if (ImGui::BeginCombo("area", area_preview_value, ImGuiComboFlags_PopupAlignLeft))
            {
                for (int n = 0; n < areas.size(); n++)
                {
                    const bool is_selected = (area_selected_idx == n);
                    if (ImGui::Selectable(areas[n], is_selected))
                        area_selected_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();


            int num_slots = 0;
            switch (area_selected_idx) {
            case 0:
                num_slots = num_body; break;
            case 1:
                num_slots = num_face; break;
            case 2:
                num_slots = num_feet; break;
            case 3:
                num_slots = num_hands; break;
            default:
                num_slots = 0;
            }

            std::multimap<tattoo_key, std::map<std::string, std::string> > tattoo_attributes;
            std::map<tattoo_key, int> tattoo_ids;
            if (actor_selected_idx < actors.size()) {
                auto actor = actors[actor_selected_idx];
                int tattoos = JFormDB::getObj(actor, ".SlaveTats.applied");
                if (tattoos && JValue::isArray(tattoos)) {
                    auto count = JArray::count(tattoos);
                    while (count > 0) {
                        count--;
                        auto tattoo = JArray::getObj(tattoos, count);
                        if (tattoo) {
                            push_tattoo_attributes(tattoo, tattoo_attributes, tattoo_ids);
                        }
                    }
                }
            }

            std::vector<slot_descriptor> slots;
            for (int i = 0; i < num_slots; i++) {
                auto it = tattoo_attributes.find(tattoo_key{ areas[area_selected_idx], i });
                if (it != tattoo_attributes.end()) {
                    auto name_it = it->second.find("name");
                    std::string name;
                    if (name_it != it->second.end())
                        name = name_it->second;
                    auto section_it = it->second.find("section");
                    std::string section;
                    if (section_it != it->second.end())
                        section = section_it->second;
                    slots.push_back(slot_descriptor{ i, name, section });
                    // slots.push_back(std::format("{} - (\"{}\" / \"{}\")", std::to_string(i), name, section));
                }
            }

            if (slot_selected_idx >= slots.size())
                slot_selected_idx = 0;

            if (slots.size() > 0) {
                auto& descr = slots[slot_selected_idx];
                std::string slot_label = std::format("{} - (\"{}\" / \"{}\")", descr.slot_num, descr.name, descr.section);
                const char* slot_preview_value = slot_label.c_str();

                static std::set<std::string> non_editable_attribs{ "section", "name", "texture", "area", "slot" };

                vec = ImGui::GetContentRegionAvail();
                ImGui::SetNextItemWidth(vec.x * 0.6);
                if (ImGui::BeginCombo("slot", slot_preview_value, ImGuiComboFlags_PopupAlignLeft))
                {
                    for (int n = 0; n < slots.size(); n++)
                    {
                        auto& descr = slots[n];
                        std::string slot_label = std::format("{} - (\"{}\" / \"{}\")", descr.slot_num, descr.name, descr.section);
                        const bool is_selected = (slot_selected_idx == n);
                        if (ImGui::Selectable(slot_label.c_str(), is_selected)) {
                            slot_selected_idx = n;
                        }
                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                    ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

                auto tattoo_jid_it = tattoo_ids.find(tattoo_key{ areas[area_selected_idx], slots[slot_selected_idx].slot_num });
                auto tattoo_attr_it = tattoo_attributes.find(tattoo_key{ areas[area_selected_idx], slots[slot_selected_idx].slot_num });
                if (tattoo_jid_it != tattoo_ids.end() && tattoo_attr_it != tattoo_attributes.end()) {
                    editor_tattoo_id_last = editor_tattoo_id;
                    editor_tattoo_id = tattoo_jid_it->second;
                    auto tattoo_attribs = tattoo_attr_it->second;

                    //ImGui::GetContentRegionAvail(&vec);
                    //ImGui::SetNextItemWidth(vec.x * 0.6);
                    if (ImGui::BeginTable("attributes", 3, flags)) {
                        ImGui::TableSetupColumn("key");
                        ImGui::TableSetupColumn("type");
                        ImGui::TableSetupColumn("value");
                        ImGui::TableHeadersRow();

                        for (auto kv_it = tattoo_attribs.begin(); kv_it != tattoo_attribs.end(); ++kv_it) {
                            auto key = kv_it->first;
                            auto value = kv_it->second;
                            auto jc_type_id = JMap::valueType(editor_tattoo_id, key);
                            attrib::attribute_type type_id = attrib::attribute_type::_undefined;
                            std::string type_str;
                            switch (jc_type_id) {
                            case (int)attrib::attribute_type::j_int:
                                type_str = "int";
                                type_id = attrib::attribute_type::j_int;
                                break;
                            case (int)attrib::attribute_type::j_float:
                                type_str = "float";
                                type_id = attrib::attribute_type::j_float;
                                break;
                            case (int)attrib::attribute_type::j_string:
                                type_str = "string";
                                type_id = attrib::attribute_type::j_string;
                                break;
                            }

                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(key.c_str());

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(type_str.c_str());

                            ImGui::TableSetColumnIndex(2);

                            auto def = attrib::find_editable_attribute(key, type_id);
                            if (def) {
                                const bool is_selected = editor_attribute_key == key;
                                if (ImGui::Selectable((value + "##" + key).c_str(), is_selected)) {
                                    editor_attribute_key_last = editor_attribute_key;
                                    editor_attribute_key = key;
                                    logger::info("editor_attribute_key = {}, editor_tattoo_id = {}", editor_attribute_key, editor_tattoo_id);
                                }
                            }
                            else {
                                ImGui::TextDisabled(value.c_str());
                            }
                        }
                        ImGui::EndTable();
                    }


                    // Reset editor, if tattoo or attribute key have been changed
                    if (editor_tattoo_id != editor_tattoo_id_last || editor_attribute_key != editor_attribute_key_last) {
                        editor_float_old = 0.f;
                        editor_float_new = 0.f;
                        editor_tattoo_id_last = editor_tattoo_id;
                        editor_attribute_key_last = editor_attribute_key;
                    }

                    // Attribute editor part (if a button was clicked)
                    if (editor_tattoo_id != 0) {
                        attrib::attribute_type editor_attribute_type = attrib::attribute_type::_undefined;
                        attrib::attribute_variant editor_attribute_value = attrib::attribute_variant();
                        auto jc_type = JMap::valueType(editor_tattoo_id, editor_attribute_key);
                        switch (jc_type) {
                        case  (int)attrib::attribute_type::j_int:
                            editor_attribute_type = attrib::attribute_type::j_int;
                            editor_attribute_value = JMap::getInt(editor_tattoo_id, editor_attribute_key);
                            break;
                        case (int)attrib::attribute_type::j_float:
                            editor_attribute_type = attrib::attribute_type::j_float;
                            editor_attribute_value = JMap::getFlt(editor_tattoo_id, editor_attribute_key);
                            break;
                        case (int)attrib::attribute_type::j_string:
                            editor_attribute_type = attrib::attribute_type::j_string;
                            editor_attribute_value = std::string(JMap::getStr(editor_tattoo_id, editor_attribute_key));
                            break;
                        }

                        // We should not get 'attribute_value_type::a_unhandled' here (those keys aren't selectable, see above) 
                        //if (editor_attribute_type != attribute_value_type::a_unhandled) {
                        //}

                        if (editor_attribute_type == attrib::attribute_type::j_float) {

                            float f = std::get<float>(editor_attribute_value);

                            std::stringstream ss;
                            ss << "actor: " << editor_actor_label << ", tattoo_id: " << editor_tattoo_id << std::endl <<
                                "field: " << editor_attribute_key << ", current value: " << f << "(hex: " << std::format("{:a}", f) << "), type: float";
                            static std::string editor_save_float_description;
                            editor_save_float_description = ss.str();
                            ImGui::TextUnformatted(editor_save_float_description.c_str());

                            if (f != editor_float_old) {
                                editor_float_old = f;
                                editor_float_new = f;
                            }

                            static std::array<const char*, 3> items = { "slider", "%.6f", "%a" };
                            static int float_format_idx = 0;
                            static const char* editor_float_input = items[0];

                            if (ImGui::BeginCombo("input method", editor_float_input, ImGuiComboFlags_PopupAlignLeft))
                            {
                                logger::info("items.size = {}", items.size());
                                for (int n = 0; n < items.size(); n++)
                                {
                                    const bool is_selected = (float_format_idx == n);
                                    if (ImGui::Selectable(items[n], is_selected)) {
                                        float_format_idx = n;
                                        editor_float_input = items[n];
                                    }
                                    if (is_selected)
                                        ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }

                            if (editor_float_input != items[0])
                                ImGui::InputFloat("new value (with format string)", &editor_float_new, 0.0f, 0.0f, editor_float_input); // format string
                            else
                                ImGui::SliderFloat("new value (with slider 0 to 1)", &editor_float_new, 0.0f, 1.0f, "%f"); // slider

                            if (ImGui::Button("Save")) {
                                logger::info("Save Button was pressed");
                                ImGui::OpenPopup("Save float attribute");
                            }

                            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });

                            if (ImGui::BeginPopupModal("Save float attribute", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                            {
                                ImGui::Text(editor_save_float_description.c_str());
                                ImGui::Text("New value %f (hex: %a)", editor_float_new, editor_float_new);
                                ImGui::Separator();
                                if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                                ImGui::SetItemDefaultFocus();
                                ImGui::SameLine();
                                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                                ImGui::EndPopup();
                            }


                        }

                        else if (editor_attribute_type == attrib::attribute_type::j_int) {
                            static slavetats_ui::widget_editor_int ieditor;
                            ieditor.render(std::get<int>(editor_attribute_value), editor_actor_label, editor_tattoo_id, editor_attribute_key);
                        }
                    }
                }
            }
            return true;
        }

#endif
    }
}
         


