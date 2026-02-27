#pragma once

#include "magic_enum.hpp"
#include "jcontainers_wrapper.h"
#include "nioverride_utils.h"

namespace slavetats_ui
{
    using namespace slavetats_ng::jcwrapper;

    struct slot_descriptor {
        int slot_num = 0;
        std::string name;
        std::string section;
        int tattoo_id = 0;
        // Why do I have to define this ?
        bool operator==(const slot_descriptor&) const = default;
    };

    struct slot_info_t {
        tattoo_area area = tattoo_area::no_value;
        int slot_num = 0;
        std::string label;

        // SlaveTats
        std::string name;             
        std::string section;
        int tattoo_id = 0;            // SlaveTats tattoo for this slot number (if id = 0, there is none)
        // NiOverride
        std::string texture_name;     // if empty, the slot is free

        // Why do I have to define this ?
        bool operator==(const slot_info_t&) const = default;
    };

    typedef std::map<tattoo_area, std::vector<slot_info_t>> area_slots_t;

    // void find_npcs(std::vector<RE::Actor*>& actors);
    std::vector<RE::Actor*> find_actors();

    // 0 - no value, 1 - none, 2 - int, 3 - float, 4 - form, 5 - object, 6 - string
    enum class jvalue_type {
        j_no_value = 0, // you get this, when you ask JContainers for the type of a not existing value, also used in nlohmann-json serialization
        j_none = 1,
        j_int = 2,
        j_float = 3,
        j_complex = 4,
        j_form = 5,
        j_string = 6
    };
    
    // Compiler evaluates 'magic_enum::enum_count<jvalue_type>' correctly, but not IntelliSense (E0146), so using hardcoded '7' here 
    //
    //constexpr static inline std::array<const char*, magic_enum::enum_count<jvalue_type>()>  jvalue_type_pretty_names{
    constexpr static inline std::array<const char*, 7>  jvalue_type_pretty_names{
        "<no value>",
        "None",
        "Int",
        "Float",
        "Complex",
        "Form",
        "String"
    };

    using jvalue_variant_t = std::variant<int, float, RE::TESForm*, std::string>;
    struct jvalue_t {
        jvalue_type type = jvalue_type::j_none;
        jvalue_variant_t value;
        bool operator==(const jvalue_t&) const = default;
    };

    using jfields_t = std::map<std::string, jvalue_t>;
    void jread_tattoo_fields(int tattoo, jfields_t& fields);

    using jactor_tattoos_t = std::map<int, jfields_t>;
    void jread_tattoo_array(int tattoos, jactor_tattoos_t& jtattoos);
    inline void jread_actor_tattoos(RE::Actor* actor, jactor_tattoos_t& jtattoos) {
        int tattoos = JFormDB::getObj(actor, ".SlaveTats.applied");
        jread_tattoo_array(tattoos, jtattoos);
    }
    typedef jactor_tattoos_t::value_type  jactor_tattoo_t;

    using jid_by_area_slot_t = std::map< tattoo_key, int >;
    void jtattoos_by_area_slot(const jactor_tattoos_t& tattoos, jid_by_area_slot_t& tattoos_by_area_slot);

    std::string jvalue_as_string(const jvalue_t& value);

    using section_tattoos_t = std::map<std::string, jactor_tattoos_t>;
    using area_sections_t = std::map<tattoo_area, section_tattoos_t>;
    void read_slavetats_installed_tattoos(area_sections_t& area_tattoos);

    using name_to_tattoo_t = std::map<std::string, int>;
    using section_tattoo_names_t = std::map<std::string, name_to_tattoo_t>;
    using area_section_tattoo_names_t = std::map<tattoo_area, section_tattoo_names_t>;
    void read_slavetats_installed_tattoo_names(const area_sections_t& area_tattoos, area_section_tattoo_names_t& tattoo_names);

    void read_slot_infos(const jactor_tattoos_t& tattoos, const jid_by_area_slot_t& tattoos_by_area_slot, const actor_overrides_t& overrides, area_slots_t& result);

}
