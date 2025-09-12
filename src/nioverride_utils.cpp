#include "nioverride_utils.h"
#include "nioverride_wrapper.h"
#include "imgui.h"
#include "slavetats_util.h"
#include "magic_enum.hpp"

namespace slavetats_ui
{
	using namespace slavetats_ng::skee_wrapper;

	void read_slot_overrides(RE::Actor* actor, bool is_female, tattoo_area area, int slot, ni_override_values& values) {
		auto node_name = std::format("{} [ovl{}]", magic_enum::enum_name(area).data(), slot);
		for (auto d : ni_override_definitions) {
			if (NiOverride::HasNodeOverride(actor, is_female, node_name.c_str(), d.first.key, d.first.index)) {
				switch (d.second.type) {
				case ni_override_type::ni_int:
					values.emplace(d.first, ni_override_value(NiOverride::GetNodeOverrideInt(actor, is_female, node_name.c_str(), d.first.key, d.first.index)));
					break;
				case ni_override_type::ni_float:
					values.emplace(d.first, ni_override_value(NiOverride::GetNodeOverrideFloat(actor, is_female, node_name.c_str(), d.first.key, d.first.index)));
					break;
				case ni_override_type::ni_string:
					values.emplace(d.first, ni_override_value(std::string(NiOverride::GetNodeOverrideString(actor, is_female, node_name.c_str(), d.first.key, d.first.index))));
					break;
				default:
					values.emplace(d.first, ni_override_value());
				}
			}
		}
	}

	void read_actor_overrides(RE::Actor* actor, actor_overrides_t& result) {
		bool is_female = actor->GetActorBase()->IsFemale();
		for (auto area : { tattoo_area::BODY, tattoo_area::FACE, tattoo_area::FEET, tattoo_area::HANDS }) {
			for (int slot = 0; slot < get_num_slots(area); slot++) {
				tattoo_key key{ area, slot };
				ni_override_values values;
				read_slot_overrides(actor, is_female, area, slot, values);
				if (!values.empty()) {
					result.emplace(key, std::move(values));
				}
			}
		}
	}

}
