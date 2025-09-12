#pragma once
#include "magic_enum.hpp"
#include "nioverride_wrapper.h"
#include "jcontainers_wrapper.h"

namespace slavetats_ui
{
    using namespace slavetats_ng::jcwrapper;

    enum class tattoo_area {
        BODY = 0, 
        FACE = 1, 
        FEET = 2, 
        HANDS = 3,
        no_value = 4
    };

    inline int get_num_slots(tattoo_area area) {
        int num_slots = 0;
        switch (area) {
        case tattoo_area::BODY:
            num_slots = slavetats_ng::skee_wrapper::NiOverride::GetNumBodyOverlays();
            break;
        case  tattoo_area::FACE:
            num_slots = slavetats_ng::skee_wrapper::NiOverride::GetNumFaceOverlays();
            break;
        case  tattoo_area::FEET:
            num_slots = slavetats_ng::skee_wrapper::NiOverride::GetNumFeetOverlays();
            break;
        case  tattoo_area::HANDS:
            num_slots = slavetats_ng::skee_wrapper::NiOverride::GetNumHandsOverlays();
            break;
        }
        return num_slots;
    }

    static inline constexpr auto tattoo_areas = magic_enum::enum_names<tattoo_area>();

    struct tattoo_key {
        tattoo_area area;
        int slot;

        inline bool operator < (const tattoo_key& rhs) const {
            if (area < rhs.area)
                return true;
            else if (area > rhs.area)
                return false;
            else
                return slot < rhs.slot;
        }
    };

    struct slot_descriptor {
        int slot_num = 0;
        std::string name;
        std::string section;
        int tattoo_id = 0;
        // Why do I have to define this ?
        bool operator==(const slot_descriptor&) const = default;
    };

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
    
    constexpr static inline std::array<const char*, magic_enum::enum_count<jvalue_type>()>  jvalue_type_pretty_names{
        "<no value>",
        "None",
        "Int",
        "Float",
        "Complex",
        "Form",
        "String",
    };

    using jvalue_variant_t = std::variant<int, float, RE::TESForm*, std::string>;
    struct jvalue_t {
        jvalue_type type = jvalue_type::j_none;
        jvalue_variant_t value;
    };

    using jfields_t = std::map<std::string, jvalue_t>;
    void jread_tattoo_fields(int tattoo, jfields_t& fields);

    using jactor_tattoos_t = std::map<int, jfields_t>;
    void jread_tattoo_array(int tattoos, jactor_tattoos_t& jtattoos);
    inline void jread_actor_tattoos(RE::Actor* actor, jactor_tattoos_t& jtattoos) {
        int tattoos = JFormDB::getObj(actor, ".SlaveTats.applied");
        jread_tattoo_array(tattoos, jtattoos);
    }

    using jid_by_area_slot_t = std::map< tattoo_key, int >;
    void jtattoos_by_area_slot(const jactor_tattoos_t& tattoos, jid_by_area_slot_t& tattoos_by_area_slot);

    std::string jvalue_as_string(const jvalue_t& value);

    using section_tattoos_t = std::map<std::string, jactor_tattoos_t>;
    using area_sections_t = std::map<tattoo_area, section_tattoos_t>;
    void read_slavetats_cache(area_sections_t& area_tattoos);

    using name_to_tattoo_t = std::map<std::string, int>;
    using section_tattoo_names_t = std::map<std::string, name_to_tattoo_t>;
}
