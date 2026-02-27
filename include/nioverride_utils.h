#pragma once
//#include "tattoo_field_definitions.h"
#include "frozen/map.h"
#include "frozen/unordered_map.h"
// #include "slavetats_util.h"
#include "nioverride_wrapper.h"
#include "magic_enum.hpp"


namespace slavetats_ui {

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
	enum class ni_override_type {
		ni_undefined = 0,
		ni_int = 1,
		ni_float = 2,
		ni_string = 3,
		ni_texture_set = 4
	};

	struct ni_override_key {
		int key = -1;
		int index = -1;
	
		constexpr inline bool operator < (const ni_override_key& rhs) const {
			if (key < rhs.key)
				return true;
			else if (key > rhs.key)
				return false;
			else
				return index < rhs.index;
		}
	
	};

	struct ni_override_definition {
		ni_override_type type = ni_override_type::ni_undefined;
		const char* name = nullptr;
	};

	constexpr inline frozen::map< ni_override_key, ni_override_definition, 23 > ni_override_definitions = {
		{{ 0, -1 }, { ni_override_type::ni_int, "EmissiveColor" }},
		{{ 1, -1 }, { ni_override_type::ni_float, "EmissiveMultiple"}},
		{{ 2, -1 }, { ni_override_type::ni_float, "Glossiness" }},
		{{ 3, -1 }, { ni_override_type::ni_float, "SpecularStrength" }},
		{{ 4, -1 }, { ni_override_type::ni_float, "LightingEffect1" }},
		{{ 5, -1 }, { ni_override_type::ni_float, "LightingEffect2" }},
		{{ 6, -1 }, { ni_override_type::ni_texture_set, "TextureSet" }},
		{{ 7, -1 }, { ni_override_type::ni_int, "TintColor" }},
		{{ 8, -1 }, { ni_override_type::ni_float, "Alpha" }},

		{{ 9, 0 }, { ni_override_type::ni_string, "Texture[Diffuse]" }},
		{{ 9, 1 }, { ni_override_type::ni_string, "Texture[Normal]" }},
		{{ 9, 2 }, { ni_override_type::ni_string, "Texture[EnvironmentMask/SubsurfaceTint]" }},
		{{ 9, 3 }, { ni_override_type::ni_string, "Texture[Glow/Detail]" }},
		{{ 9, 4 }, { ni_override_type::ni_string, "Texture[Height]" }},
		{{ 9, 5 }, { ni_override_type::ni_string, "Texture[Environment]" }},
		{{ 9, 6 }, { ni_override_type::ni_string, "Texture[Multilayer]" }},
		{{ 9, 7 }, { ni_override_type::ni_string, "Texture[BacklightMask/Specular]" }},
		{{ 9, 8 }, { ni_override_type::ni_string, "Texture[???]" }},

		{{ 20, -1 }, { ni_override_type::ni_float, "ControllerStartStop" }},
		{{ 21, -1 }, { ni_override_type::ni_float, "ControllerStartTime" }},
		{{ 22, -1 }, { ni_override_type::ni_float, "ControllerStopTime" }},
		{{ 23, -1 }, { ni_override_type::ni_float, "ControllerFrequency" }},
		{{ 24, -1 }, { ni_override_type::ni_float, "ControllerPhase" }}
	};
	
	using ni_override_value = std::variant<int, float, std::string>;

	using ni_override_values = std::map< ni_override_key, ni_override_value >;

	void read_slot_overrides(RE::Actor* actor, bool is_female, tattoo_area area, int slot, ni_override_values& values);

	using actor_overrides_t = std::map<tattoo_key, ni_override_values>;
	void read_actor_overrides(RE::Actor* actor, actor_overrides_t& overrides);

}
