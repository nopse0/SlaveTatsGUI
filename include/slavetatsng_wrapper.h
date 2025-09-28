#if 0
#pragma once

#include <ClibUtil/singleton.hpp>
#include "SlaveTatsNG_Interface.h"

namespace slavetats
{
	namespace interface_impl
	{
		class SlaveTats : public clib_util::singleton::ISingleton<SlaveTats>, public SlaveTatsNG::Wrapper {
		public:
			//SlaveTatsNG::Wrapper* wrapper = new SlaveTatsNG::Wrapper(nullptr);
			inline SlaveTats() : Wrapper(nullptr) {}
		};
	}

	// Wrapper of the wrapper :)

	using namespace interface_impl;

	inline bool simple_add_tattoo(RE::Actor* target, RE::BSFixedString section, RE::BSFixedString name, int color = 0, bool last = true, bool silent = false, float alpha = 1.0)
	{
		return SlaveTats::GetSingleton()->simple_add_tattoo(target, section, name, color, last, silent, alpha);
	}
	inline bool simple_remove_tattoo(RE::Actor* target, RE::BSFixedString section, RE::BSFixedString name, bool last = true, bool silent = false)
	{
		return SlaveTats::GetSingleton()->simple_remove_tattoo(target, section, name, last, silent);
	}
	inline bool synchronize_tattoos(RE::Actor* target, bool silent = false)
	{
		return SlaveTats::GetSingleton()->synchronize_tattoos(target, silent);
	}
	inline void log_tattoo(RE::BSFixedString message, int tattoo)
	{
		return SlaveTats::GetSingleton()->log_tattoo(message, tattoo);
	}
	inline bool tattoo_matches(int a_template, int tattoo, bool include_configurable = false)
	{
		return SlaveTats::GetSingleton()->tattoo_matches(a_template, tattoo, include_configurable);
	}
	inline void _append(int dest, int val)
	{
		SlaveTats::GetSingleton()->_append(dest, val);
	}
	inline void _extend_matching(int dest, int src, int a_template, int applied = 0, RE::BSFixedString domain = "default")
	{
		SlaveTats::GetSingleton()->_extend_matching(dest, src, a_template, applied, domain);
	}
	inline bool query_available_tattoos(int a_template, int matches, int applied = 0, RE::BSFixedString domain = "default")
	{
		return SlaveTats::GetSingleton()->query_available_tattoos(a_template, matches, applied, domain);
	}
	inline bool query_applied_tattoos(RE::Actor* target, int a_template, int matches, RE::BSFixedString except_area = "", int except_slot = -1)
	{
		return SlaveTats::GetSingleton()->query_applied_tattoos(target, a_template, matches, except_area, except_slot);
	}
	inline bool query_applied_tattoos_with_attribute(RE::Actor* target, RE::BSFixedString attrib, int matches, RE::BSFixedString except_area = "", int except_slot = -1)
	{
		return SlaveTats::GetSingleton()->query_applied_tattoos_with_attribute(target, attrib, matches, except_area, except_slot);
	}
	inline bool has_applied_tattoos_with_attribute(RE::Actor* target, RE::BSFixedString attrib, RE::BSFixedString except_area = "", int except_slot = -1)
	{
		return SlaveTats::GetSingleton()->has_applied_tattoos_with_attribute(target, attrib, except_area, except_slot);
	}
	inline bool remove_tattoos(RE::Actor* target, int a_template, bool ignore_lock = false, bool silent = false)
	{
		return SlaveTats::GetSingleton()->remove_tattoos(target, a_template, ignore_lock, silent);
	}
	inline bool remove_tattoo_from_slot(RE::Actor* target, RE::BSFixedString area, int slot, bool ignore_lock = false, bool silent = false)
	{
		return SlaveTats::GetSingleton()->remove_tattoo_from_slot(target, area, slot, ignore_lock, silent);
	}
	inline int get_applied_tattoo_in_slot(RE::Actor* target, RE::BSFixedString area, int slot)
	{
		return SlaveTats::GetSingleton()->get_applied_tattoo_in_slot(target, area, slot);
	}
	inline bool get_applied_tattoos_by_area(RE::Actor* target, int on_body, int on_face, int on_hands, int on_feet)
	{
		return SlaveTats::GetSingleton()->get_applied_tattoos_by_area(target, on_body, on_face, on_hands, on_feet);
	}
	inline bool external_slots(RE::Actor* target, RE::BSFixedString area, int matches)
	{
		return SlaveTats::GetSingleton()->external_slots(target, area, matches);
	}
	inline bool add_tattoo(RE::Actor* target, int tattoo, int slot = -1, bool ignore_lock = false, bool silent = false)
	{
		return SlaveTats::GetSingleton()->add_tattoo(target, tattoo, slot, ignore_lock, silent);
	}
	inline int add_and_get_tattoo(RE::Actor* target, int tattoo, int slot = -1, bool ignore_lock = false, bool silent = false,
		bool try_upgrade = true)
	{
		return SlaveTats::GetSingleton()->add_and_get_tattoo(target, tattoo, slot, ignore_lock, silent, try_upgrade);
	}
	inline RE::TESForm* get_form(int tattoo, RE::BSFixedString plugin_field, RE::BSFixedString formid_field, RE::TESForm* a_default = nullptr)
	{
		return SlaveTats::GetSingleton()->get_form(tattoo, plugin_field, formid_field, a_default);
	}
	inline void _log_jcontainer(int jc, RE::BSFixedString indent)
	{
		return SlaveTats::GetSingleton()->_log_jcontainer(jc, indent);
	}


}
#endif

