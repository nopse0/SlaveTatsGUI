#include "tattoo_field_db.h"
#include <nlohmann/json.hpp>

namespace slavetats_ui {

    NLOHMANN_JSON_SERIALIZE_ENUM(jvalue_type, {
        {jvalue_type::j_none, "j_none"},
        {jvalue_type::j_int, "j_int"},
        {jvalue_type::j_float, "j_float"},
        {jvalue_type::j_complex, "j_complex"},
        {jvalue_type::j_form, "j_form"},
        {jvalue_type::j_string, "j_string"},
        {jvalue_type::j_no_value, "j_no_value"}
        });

    namespace field_db {

        NLOHMANN_JSON_SERIALIZE_ENUM(value_range, {
            {value_range::unconstrained, "unconstrained"},
            {value_range::rgb, "rgb"},
            {value_range::boolean, "boolean"},
            {value_range::between_0_and_1, "between_0_and_1"},
            {value_range::texture_name, "texture_name"}
            });

        using json = nlohmann::json;

        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(field_definition, \
            name, \
            type, \
            range, \
            editable\
        )

        field_db_t field_db;

        void load_field_db()
        {
            // I/O and Json deserialization can throw
            try {
                std::ifstream ifs("data/skse/plugins/SlaveTatsNG/tattoo_field_db.json");
                json          jf = json::parse(ifs);
                field_db = jf.template get<field_db_t>();
            }
            catch (const std::exception& e) {
                logger::info("Caught exception: {}", e.what());
                abort();
            }
        }

    }
    
}
