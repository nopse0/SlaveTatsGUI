#include "utility.h"
#include "jcontainers_wrapper.h"
#include "slavetats_util.h"
#include "tattoo_field_definitions.h"
#include "slavetatsng_wrapper.h"

namespace slavetats_ui {

    using namespace slavetats_ng::jcwrapper;

    static void find_npcs(std::vector<RE::Actor*>& actors) {
        const auto processLists = RE::ProcessLists::GetSingleton();
        if (!processLists) { return; }
        static const auto* ActorTypeNPC = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("ActorTypeNPC");
        for (auto& targetHandle : processLists->highActorHandles) {
            const auto actorPtr = targetHandle.get();
            if (!actorPtr) { continue; }
            // Check if actor is NPC (leave out if you want all actors)
            const auto base = actorPtr->GetActorBase();
            if (base && base->GetRace()->HasKeyword(ActorTypeNPC)) {
                actors.push_back(actorPtr.get());
            }
        }
        return;
    }

    std::vector<RE::Actor*> find_actors() {
        std::vector<RE::Actor*> actors;
        RE::Actor* playerActor = RE::PlayerCharacter::GetSingleton();
        actors.push_back(playerActor);
        find_npcs(actors);
        return actors;
    }

    void jread_tattoo_fields(int tattoo, jfields_t& fields) {
        std::string key(JMap::nextKey(tattoo).c_str());
        std::transform(key.begin(), key.end(), key.begin(), tolower);  // lower case
        while (!key.empty()) {
            auto jtype = JMap::valueType(tattoo, key);
            auto type = magic_enum::enum_cast<jvalue_type>(jtype);
            // We are using the same enumeration values as Jcontainers, so this should always be true
            if (type.has_value()) {
                // Special case is key = "area", because the area value is needed for lookups and std::string is case sensitive
                // => convert the value to upper case
                if (key == "area" && type.value() == jvalue_type::j_string) {
                    std::string value(JMap::getStr(tattoo, key).c_str());
                    std::transform(value.begin(), value.end(), value.begin(), toupper);
                    fields.emplace(std::make_pair(key, jvalue_t{ jvalue_type::j_string, jvalue_variant_t(value) }));
                }
                else {
                    switch (type.value()) {
                    case jvalue_type::j_none:
                        fields.emplace(std::make_pair(key, jvalue_t{ jvalue_type::j_none }));
                        break;
                    case jvalue_type::j_int:
                        fields.emplace(std::make_pair(key, jvalue_t{ jvalue_type::j_int, jvalue_variant_t(JMap::getInt(tattoo, key)) }));
                        break;
                    case jvalue_type::j_float:
                        fields.emplace(std::make_pair(key, jvalue_t{ jvalue_type::j_float, jvalue_variant_t(JMap::getFlt(tattoo, key)) }));
                        break;
                    case jvalue_type::j_form:
                        fields.emplace(std::make_pair(key, jvalue_t{ jvalue_type::j_form, jvalue_variant_t(JMap::getForm(tattoo, key)) }));
                        break;
                    case jvalue_type::j_complex:
                        fields.emplace(std::make_pair(key, jvalue_t{ jvalue_type::j_complex, jvalue_variant_t(JMap::getObj(tattoo, key)) }));
                        break;
                    case jvalue_type::j_string:
                        fields.emplace(std::make_pair(key, jvalue_t{ jvalue_type::j_string, jvalue_variant_t(std::string(JMap::getStr(tattoo, key).c_str())) }));
                        break;
                    }
                }    
            }
            key = JMap::nextKey(tattoo, key);
            std::transform(key.begin(), key.end(), key.begin(), tolower);  // lower case
        }
    }

    void jread_tattoo_array(int tattoos, jactor_tattoos_t& jtattoos) {
        if (tattoos && JValue::isArray(tattoos)) {
            auto count = JArray::count(tattoos);
            while (count > 0) {
                count--;
                auto tattoo = JArray::getObj(tattoos, count);
                if (tattoo) {
                    jfields_t fields;
                    jread_tattoo_fields(tattoo, fields);
                    jtattoos.emplace(tattoo, std::move(fields));
                }
            }
        }
    }

    void jtattoos_by_area_slot(const jactor_tattoos_t& tattoos, jid_by_area_slot_t& tattoos_by_area_slot) {
        for (auto& tattoo : tattoos) {
            auto& fields = tattoo.second;
            tattoo_area area = tattoo_area::no_value;
            {
                auto it = fields.find("area");
                if (it != fields.end() && it->second.type == jvalue_type::j_string) {
                    auto area_opt = magic_enum::enum_cast<tattoo_area>(std::get<std::string>(it->second.value));
                    if (area_opt.has_value())
                        area = area_opt.value();
                }
            }
            int slot = -1;
            {
                auto it = fields.find("slot");
                if (it != fields.end() && it->second.type == jvalue_type::j_int) {
                    slot = std::get<int>(it->second.value);
                }
            }
            tattoos_by_area_slot.emplace(tattoo_key{ area, slot }, tattoo.first);  // fields are copied
        }
    }

    std::string jvalue_as_string(const jvalue_t& value) {
        switch (value.type) {
        case jvalue_type::j_none:
            return std::string("??? none ???");
        case jvalue_type::j_int:
            return std::format("{}", std::get<int>(value.value));
        case jvalue_type::j_float:
            return std::format("{}", std::get<float>(value.value));
        case jvalue_type::j_form:
            return std::string("??? form ???");
        case jvalue_type::j_complex:
            return std::string("??? complex object ???");
        case jvalue_type::j_string:
            return std::format("{}", (std::get<std::string>(value.value)).c_str());
        default:
            return std::string("??? undefined ???");
        }
    }

    //static void jfields_to_strings(const jfields_t& jfields, std::map<std::string, std::string>& sfields) {
    //    for (auto& kv : jfields) {
    //        sfields.emplace(kv.first, jvalue_as_string(kv.second));
    //    }
    //}

    void read_slavetats_cache(area_sections_t& area_sections) {
        int cache = JDB::solveObj(".SlaveTatsNG.cache");
        if (cache) {
            int domain = JMap::getObj(cache, "default");
            if (domain) {
                for (auto area : { tattoo_area::BODY, tattoo_area::FACE, tattoo_area::FEET, tattoo_area::HANDS }) {
                    int sections = JMap::getObj(domain, magic_enum::enum_name(area));
                    if (sections) {
                        section_tattoos_t section_tattoos;
                        for (auto section_name = JMap::nextKey(sections); !section_name.empty(); section_name = JMap::nextKey(sections, section_name)) {
                            int tattoos_id = JMap::getObj(sections, section_name);
                            if (tattoos_id) {
                                jactor_tattoos_t tattoos;
                                jread_tattoo_array(tattoos_id, tattoos);
                                if (!tattoos.empty()) {
                                    section_tattoos.emplace(section_name, std::move(tattoos));
                                }
                            }
                        }
                        area_sections.emplace(area, std::move(section_tattoos));
                    }
                }
            }
        }
    }


}
