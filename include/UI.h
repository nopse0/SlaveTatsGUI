#pragma once

namespace slavetats_ui {

    // SKSEMenuFramework doesn't use std::function, so we have to use a namespace instead of a class (so multiple SlaveTatsUI instances are not possible)
    namespace SlaveTatsUI {
        void initialize();
    };
}
