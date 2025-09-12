#pragma once
//#include "SKSEMenuFramework-2.0/SKSEMenuFramework.h"
//#include <ClibUtil/singleton.hpp>

/*
namespace Configuration {
    namespace Example1 {
        inline int Number = 1;
    }
    namespace Example2 {
        inline char* Buffer = new char[256];
        inline float Color = 0;
    }
};
*/

namespace slavetats_ui {

    // SKSEMenuFramework doesn't use std::function, so we have to use a namespace instead of a class (so multiple SlaveTatsUI instances are not possible)
    namespace SlaveTatsUI {   // : public clib_util::singleton::ISingleton<SlaveTatsUI> {
        void initialize();
        bool render_applied_window();
        bool render_attributes_window();
    };


    //void register_ui();
/*
    namespace Example1 {
        inline uint32_t AddFormId = 0x64B33;
        inline RE::TESBoundObject* AddBoundObject = nullptr;
        void LookupForm();
        void __stdcall Render();
    }
    namespace Example2 {
        void __stdcall Render();
        void __stdcall RenderWindow();
        inline MENU_WINDOW ExampleWindow;
    }
    namespace Example3 {
        inline ImGuiTextFilter* filter;
        inline std::string CheckMark = FontAwesome::UnicodeToUtf8(0xf00c);
        void __stdcall Render();
    }
    namespace Example4 {
        inline std::string TitleText = "This is an " + FontAwesome::UnicodeToUtf8(0xf2b4) + " Font Awesome usage example";
        inline std::string Button1Text = FontAwesome::UnicodeToUtf8(0xf0e9) + " Umbrella";
        inline std::string Button2Text = FontAwesome::UnicodeToUtf8(0xf06e) + " Eye";
        void __stdcall Render();
    }
*/
}
