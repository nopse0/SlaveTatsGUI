#include "tattoo_field_db.h"
#include "menu_api.h"
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

    namespace  SlaveTatsUI {

        void initialize() {
            field_db::load_field_db();

            logger::info("Looking for CatMenu...");

            auto result = CatMenu::RequestCatMenuAPI();
            if (result.index() == 0)
            {
                auto catmenu_api = std::get<0>(result);

                ImGui::SetCurrentContext(catmenu_api->GetContext());

                RE::BSString menu_name2{ "SlaveTats: Apply/Remove Tattoo" };
                catmenu_api->RegisterMenuDrawFunc(menu_name2, render_apply_remove);

                RE::BSString menu_name1{ "SlaveTats: Inspector" };
                catmenu_api->RegisterMenuDrawFunc(menu_name1, render_inspector);

                RE::BSString menu_name3{ "SlaveTats: Field Editor" };
                catmenu_api->RegisterMenuDrawFunc(menu_name3, render_field_editor);

                logger::info("CatMenu integration succeed!");
            }
            else {
                logger::warn("CatMenu integration failed! SlaveTatsGUI disabled. Error: {}", std::get<1>(result));
                return;
            }

            
        }
    }
}
         


