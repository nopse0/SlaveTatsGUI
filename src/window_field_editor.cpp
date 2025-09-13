#include "window_field_editor.h"
#include "slavetats_util.h"
#include "imgui.h"
#include "nioverride_wrapper.h"
#include "jcontainers_wrapper.h"
#include "widget_actor_selector.h"
#include "widget_area_selector.h"
#include "widget_slot_selector.h"
#include "widget_field_selector.h"
#include "widget_int_editor.h"
#include "widget_float_editor.h"
#include "widget_string_editor.h"
#include "tattoo_field_db.h"
#include "widget_new_field_selector.h"

namespace slavetats_ui
{
    using namespace slavetats_ng::jcwrapper;

	void window_field_editor::render(bool* window_shown) {
        
//        std::thread t1
//        {
//            [&] {

        if (!ImGui::Begin("[SlaveTats] Field Editor", window_shown)) {
            ImGui::End();
            return;
        }

        static widget_actor_selector actor_selector;
        actor_selector.render();

        ImGui::SameLine();

        static widget_area_selector area_selector;
        area_selector.render();

        jactor_tattoos_t jtattoos;
        jread_actor_tattoos(actor_selector.actor, jtattoos);

        jid_by_area_slot_t tattoo_ids;
        jtattoos_by_area_slot(jtattoos, tattoo_ids);

        static widget_slavetats_slot_selector slot_selector;
        slot_selector.render(area_selector.area_selected_value, tattoo_ids, jtattoos);

        static widget_field_selector field_selector;

        auto tattoo_id = slot_selector.slot_info.tattoo_id;
        auto jit = jtattoos.find(tattoo_id);
        if (jit != jtattoos.end()) {
            auto tkey = tattoo_key{ area_selector.area_selected_value, slot_selector.slot_info.slot_num };
            auto& jfields = jit->second;
            ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
            field_selector.render(flags, jfields);
            if (field_selector.selected_field != "[new]") {
                auto vit = jfields.find(field_selector.selected_field);
                if (vit != jfields.end()) {
                    auto& jvalue = vit->second;
                    if (jvalue.type == jvalue_type::j_int) {
                        int i = std::get<int>(jvalue.value);
                        static slavetats_ui::widget_int_editor int_editor;
                        int_editor.render(i, actor_selector.actor_label, slot_selector.slot_info.tattoo_id, field_selector.selected_field, false);
                    }
                    else if (jvalue.type == jvalue_type::j_float) {
                        float f = std::get<float>(jvalue.value);
                        static slavetats_ui::widget_float_editor float_editor;
                        float_editor.render(f, actor_selector.actor_label, slot_selector.slot_info.tattoo_id, field_selector.selected_field, false);
                    }
                    else if (jvalue.type == jvalue_type::j_string) {
                        std::string s(std::get<std::string>(jvalue.value));
                        static slavetats_ui::widget_string_editor string_editor;
                        string_editor.render(s, actor_selector.actor_label, slot_selector.slot_info.tattoo_id, field_selector.selected_field, false);
                    }
                }
            }
            else {  // "[new]"
                field_db::field_db_t field_choices;
                for (auto fit : field_db::field_db) {
                    if (!jfields.contains(fit.first) && fit.second.editable) {
                        field_choices.emplace(fit);
                    }
                }
                static widget_new_field_selector new_field_selector;
                new_field_selector.render(field_choices);
                if (!new_field_selector.selected_field.name.empty()) {
                    auto& field = new_field_selector.selected_field;
                    if (field.type == jvalue_type::j_int) {
                        static slavetats_ui::widget_int_editor int_editor;
                        int_editor.render(0, actor_selector.actor_label, slot_selector.slot_info.tattoo_id, field.name, true);
                    }
                    else if (field.type == jvalue_type::j_float) {
                        static slavetats_ui::widget_float_editor float_editor;
                        float_editor.render(0.f, actor_selector.actor_label, slot_selector.slot_info.tattoo_id, field.name, true);
                    }
                    else if (field.type == jvalue_type::j_string) {
                        static slavetats_ui::widget_string_editor string_editor;
                        string_editor.render("", actor_selector.actor_label, slot_selector.slot_info.tattoo_id, field.name, true);
                    }
                }

            }
        
            
        }

        ImGui::End();


//            }
//        };
//        t1.join();

	}

}
