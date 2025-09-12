#pragma once
#include "slavetats_util.h"

namespace slavetats_ui {

    namespace field_db {

        // using attribute_variant = std::variant<int, float, std::string>;

        /*
        enum class attribute_name {
            _undefined = -1,
            area,
            section,
            name
        };
        */

        /*
        enum class attribute_type {
            _undefined = -1,
            j_none = 0,
            j_int = 2,
            j_float = 3,
            j_string = 6
        };
        */

        enum class value_range {
            unconstrained,
            rgb,
            boolean,
            between_0_and_1,
            texture_name,
        };

        struct field_definition {
            std::string name;
            jvalue_type type = jvalue_type::j_no_value;
            value_range range = value_range::unconstrained;
            bool editable = false;
        };

        using field_db_t = std::map<std::string, field_definition>;

        // On I/O errors or Json deserialization errors, this function will throw an exception and terminate Skyrim.
        // Install CrashLoggerSSE, the generated crash log will show what went wrong, e.g. the Json parse error (inclusive line and column number).
        void load_field_db();

        extern field_db_t field_db;

        inline std::optional<field_definition> find_editable_attribute(const std::string& key, jvalue_type type) {
            auto d = field_db.find(key);
            if (d != field_db.end() && d->second.type == type && d->second.editable)
                return d->second;
            else
                return std::optional<field_definition>();
        }

        inline std::optional<field_definition> find_editable_attribute(const std::string& key) {
            auto d = field_db.find(key);
            if (d != field_db.end() && d->second.editable)
                return d->second;
            else
                return std::optional<field_definition>();
        }
    }
}
