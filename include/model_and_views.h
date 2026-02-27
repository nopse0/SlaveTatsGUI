#pragma once

#include <CLibUtil/singleton.hpp>
#include "widget_area_selector.h"
#include <magic_enum_containers.hpp>
#include "nioverride_utils.h"
#include <tattoo_field_db.h>
#include <editors.h>

// Note: model and views are intended to be used only by the thread, which does the ImGui rendering,
// and their is only one instance of each of the three views 'apply_remove', 'inspector' and 'field_editor',
// so no synchronization or classes necessary 

namespace slavetats_ui {

	class actor_model_t;
	class applied_tattoos_model_t;
	class nioverride_model_t;
	void refresh_models(actor_model_t* actor_model, applied_tattoos_model_t* applied_tattoos, nioverride_model_t* nioverride_model);

	template<class T>
	class evaluable {
	public:
		void reevaluate() {
			static_cast<T*>(this)->update();
		}
	};

	template<class E, class T>
	class interests_manager : public evaluable<T>
	{
	public:
		typedef magic_enum::containers::bitset<E> interest_mask;
		typedef int interest_handle;
	
	private:
		interest_handle last_handle = 0;
		std::map<interest_handle, std::pair<interest_mask, interest_mask>> interests;   // ._1 = dirty flags, ._2 = dirty flag mask
	
	public:
		interest_handle interest_register(interest_mask mask) {
			last_handle += 1;
			interests.emplace(std::make_pair(last_handle, std::make_pair(interest_mask{}, mask)));
			return last_handle;
		}

		void interest_unregister(interest_handle handle) {
			interests.erase(handle);
		}

		void set_dirty(interest_mask flags) {
			for (auto it = interests.begin(); it != interests.end(); ++it) {
				it->second = std::make_pair(flags & it->second.second, it->second.second);
			}
		}

		std::pair<interest_mask, interest_mask> get_dirty(interest_handle handle) {
			this->evaluable<T>::reevaluate();
			auto it = interests.find(handle);
			if (it != interests.end()) {
				return it->second;
			}
			else {
				return std::pair(interest_mask{}, interest_mask{});
			}
		}

		bool test_and_clear_dirty(interest_handle handle) {
			auto flags = get_dirty(handle);
			bool dirty = (flags.first & flags.second).any();
			clear_dirty(handle);
			return dirty;
		}
		
		void clear_dirty(interest_handle handle) {
			auto it = interests.find(handle);
			if (it != interests.end()) {
				it->second.first.reset();     // reset all dirty flags
			}
		}
	};

	namespace _slavetats_model {
		enum class dirty_flags {
			periodic_refresh = 1,
		};
	}
	class slavetats_model_t : public clib_util::singleton::ISingleton<slavetats_model_t>, 
		public interests_manager< _slavetats_model::dirty_flags, slavetats_model_t> {
	private:
		std::chrono::time_point<std::chrono::steady_clock> last_refresh;
	public:
		typedef _slavetats_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, slavetats_model_t>::interest_mask;
		using interests_manager<dirty_flags, slavetats_model_t>::interest_handle;

		slavetats_model_t();
		void init();
		void update();
		void refresh();
	};


	namespace _installed_tattoos_model {
		enum class dirty_flags {
			updated = 1,
		};
	}
	class installed_tattoos_model_t : public clib_util::singleton::ISingleton<installed_tattoos_model_t>,
		public interests_manager< _installed_tattoos_model::dirty_flags, installed_tattoos_model_t>
	{
	public:
		typedef _installed_tattoos_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, installed_tattoos_model_t>::interest_mask;
		using interests_manager<dirty_flags, installed_tattoos_model_t>::interest_handle;
	private:
		slavetats_model_t::interest_handle slavetats_model_registered = 0;
		area_sections_t             _installed_tattoos;       // area -> (section -> (tattoo_id -> tattoo_fields))
		area_section_tattoo_names_t _installed_tattoo_names;  // area -> (section -> (tattoo_name -> tattoo_id))

	public:
		installed_tattoos_model_t();
		~installed_tattoos_model_t();
		void init();
		void update();

		// Returns the installed slavetats tattoos, ordered by area and section
		const area_sections_t&             installed_tattoos();
		const area_section_tattoo_names_t& installed_tattoo_names();
	};

	class actor_scanner_model : public clib_util::singleton::ISingleton<actor_scanner_model>
	{
	private:
		std::vector<RE::Actor*> _actors;
		std::vector<std::string> _actor_labels;
		// std::chrono::time_point<std::chrono::steady_clock> last_scan;

	public:
		actor_scanner_model();
		void init();
		void update();

		const std::vector<RE::Actor*>& actors();
		const std::vector<std::string>& actor_labels();
	};


	namespace _actor_model {
		enum class dirty_flags {
			selected_actor = 1,
		};
	}
	class actor_model_t : public interests_manager< _actor_model::dirty_flags, actor_model_t>
	{
	private:
		RE::Actor*  actor = nullptr;          // null, if no actor is selected
		std::string actor_label;
	public:
		typedef _actor_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, actor_model_t>::interest_mask;
		using interests_manager<dirty_flags, actor_model_t>::interest_handle;

		void update() {}
		RE::Actor* get_actor();               // null, if no actor is selected
		const std::string& get_actor_label();
		void set_actor(RE::Actor* a_actor);
	};


	namespace _area_model {
		enum class dirty_flags {
			selected_area = 1,
		};
	}
	class area_model_t : public interests_manager< _area_model::dirty_flags, area_model_t>
	{
	private:
		tattoo_area _area;
	public:
		typedef _area_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, area_model_t>::interest_mask;
		using interests_manager<dirty_flags, area_model_t>::interest_handle;
		area_model_t();
		void update();
		void init();

		tattoo_area get_area();
		void set_area(tattoo_area a_area);
	};


	namespace _nioverride_model {
		enum class dirty_flags {
			updated = 1,
		};
	}
	class nioverride_model_t : public interests_manager< _nioverride_model::dirty_flags, nioverride_model_t>
	{
	public:
		typedef _nioverride_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, nioverride_model_t>::interest_mask;
		using interests_manager<dirty_flags, nioverride_model_t>::interest_handle;

	private:
		actor_model_t::interest_handle     actor_model_registered = 0;
		slavetats_model_t::interest_handle slavetats_model_registered = 0;
		// std::chrono::time_point<std::chrono::steady_clock> last_scan;

		actor_model_t*                   actor_model;
		std::optional<actor_overrides_t> _overrides;

	public:
		nioverride_model_t(actor_model_t* a_actor_model);
		~nioverride_model_t();
		void update();
		void init();
		
		const std::optional<actor_overrides_t>& overrides();
	};

	namespace _applied_tattoos_model {
		enum class dirty_flags {
			updated = 1,
		};
	}
	class applied_tattoos_model_t : public interests_manager< _applied_tattoos_model::dirty_flags, applied_tattoos_model_t>
	{
	public:
		typedef _applied_tattoos_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, applied_tattoos_model_t>::interest_mask;
		using interests_manager<dirty_flags, applied_tattoos_model_t>::interest_handle;
	private:
		actor_model_t::interest_handle     actor_model_registered = 0;
		slavetats_model_t::interest_handle slavetats_model_registered = 0;
		// std::chrono::time_point<std::chrono::steady_clock> last_scan;

		actor_model_t*                    actor_model;
		std::optional<jactor_tattoos_t>   _tattoos;
		std::optional<jid_by_area_slot_t> _tattoo_ids;
		std::string                       _tattoo_version;

	public:
		applied_tattoos_model_t(actor_model_t* a_actor_model);
		~applied_tattoos_model_t();
		void update();
		void init();

		const std::optional<jactor_tattoos_t>&   tattoos();
		const std::optional<jid_by_area_slot_t>& tattoo_ids();
		const std::string& tattoo_version();

	};

	namespace _slots_model {
		enum class dirty_flags {
			updated = 1,
		};
	}
	class slots_model_t : public interests_manager< _slots_model::dirty_flags, slots_model_t> {
	public:
		typedef _slots_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, slots_model_t>::interest_mask;
		using interests_manager<dirty_flags, slots_model_t>::interest_handle;
	private:
		applied_tattoos_model_t::interest_handle applied_tattoos_model_registered = 0;
		nioverride_model_t::interest_handle      nioverride_model_registered = 0;
		// std::chrono::time_point<std::chrono::steady_clock> last_scan;

		applied_tattoos_model_t* _applied_tattoos_model;
		nioverride_model_t*      _nioverride_model;
		
		area_slots_t _slot_infos;
		// std::optional<slot_info_t> _selected_slot;

	public:
		slots_model_t(applied_tattoos_model_t* applied_model, nioverride_model_t* nioverride_model);
		~slots_model_t();
		void init();
		void update();

		const area_slots_t& slot_infos();
	};


	namespace _selected_slot_model {
		enum class dirty_flags {
			updated = 1,
		};
	}
	class selected_slot_model_t : public interests_manager< _selected_slot_model::dirty_flags, selected_slot_model_t> {
	public:
		typedef _selected_slot_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, selected_slot_model_t>::interest_mask;
		using interests_manager<dirty_flags, selected_slot_model_t>::interest_handle;
	private:
		area_model_t::interest_handle area_model_registered = 0;
		slots_model_t::interest_handle slots_model_registered = 0;
	
		area_model_t* _area_model;
		slots_model_t* _slots_model;

		std::optional<slot_info_t> _selected_slot;

	public:
		selected_slot_model_t(area_model_t* area_model, slots_model_t* slots_model);
		~selected_slot_model_t();
		void update();
		void init();

		const std::optional<slot_info_t>& selected_slot();
		void set_selected_slot(const std::optional<slot_info_t>& slot_info);
	};


	namespace _selected_section_model {
		enum class dirty_flags {
			updated = 1,
		};
	}
	class selected_section_model_t : public interests_manager< _selected_section_model::dirty_flags, selected_section_model_t> {
	public:
		typedef _selected_section_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, selected_section_model_t>::interest_mask;
		using interests_manager<dirty_flags, selected_section_model_t>::interest_handle;
		struct selection {
			tattoo_area area;
			std::string section;
		};
	private:
		area_model_t::interest_handle area_model_registered = 0;
		installed_tattoos_model_t::interest_handle installed_tattoos_model_registered = 0;

		area_model_t* _area_model;
		std::optional<selection> _selected_section;
	public:
		selected_section_model_t(area_model_t* area_model);   // installed_tattoos_model_t* installed_tattoos);
		~selected_section_model_t();
		void init();
		void update();

		const std::optional<selection>& get_selected_section();
		void set_selected_section(const std::optional<selection>& section);
	};


	class tattoo_in_section_model_t {
	public:
		struct selection {
			tattoo_area area;
			std::string section;
			std::string name;
			int         tattoo_id;
		};
	private:
		area_model_t::interest_handle area_model_registered = 0;
		installed_tattoos_model_t::interest_handle installed_tattoos_model_registered = 0;
		selected_section_model_t::interest_handle selected_section_model_registered = 0;

		area_model_t* _area_model;
		selected_section_model_t* _selected_section_model;
		std::optional<selection>  _selected_tattoo;
	public:
		tattoo_in_section_model_t(area_model_t* area_model, selected_section_model_t* selected_section_model);
		~tattoo_in_section_model_t();
		void init();
		void update();

		const std::optional<selection>& get_selected_tattoo();
		void set_selected_tattoo(const std::optional<selection>& section);
	};

	namespace _field_in_tattoo_model {
		enum class dirty_flags {
			selection_changed = 1,
			field_value_changed = 2
		};
	}
	class field_in_tattoo_model_t : public interests_manager< _field_in_tattoo_model::dirty_flags, field_in_tattoo_model_t> {
	public:
	public:
		typedef _field_in_tattoo_model::dirty_flags dirty_flags;
		using interests_manager<dirty_flags, field_in_tattoo_model_t>::interest_mask;
		using interests_manager<dirty_flags, field_in_tattoo_model_t>::interest_handle;
		struct selection {
			std::string field_name;
			jvalue_t    field_value;
			int         tattoo_id;
			field_db::field_definition field_definition;
		};
	private:
		applied_tattoos_model_t::interest_handle applied_tattoos_model_registered = 0;
		selected_slot_model_t::interest_handle selected_slot_model_registered = 0;

		applied_tattoos_model_t*                  _applied_tattoos_model;
		selected_slot_model_t*                    _selected_slot_model;
		std::optional<selection>				  _selected_field;
		std::optional<field_db::field_definition> _selected_field_definition;
	public:
		field_in_tattoo_model_t(applied_tattoos_model_t* applied_tattoos_model, selected_slot_model_t* selected_slot_model);
		~field_in_tattoo_model_t();
		void init();
		void update();

		const std::optional<selection>&  get_selected_field();
		void set_selected_field(const std::optional<selection>& field);
		const std::optional<field_db::field_definition>& get_selected_definition();
		void set_selected_definition(const std::optional<field_db::field_definition>& field);
	};

	class field_editor_int_model_t {
	private:
		field_in_tattoo_model_t::interest_handle field_in_tattoo_model_registered = 0;

		field_in_tattoo_model_t*        _field_in_tattoo_model;
		std::optional<input_method_int> _input_method;
		int                             _private_buf;
		std::optional<int*>             _buffer_address;
	public:
		field_editor_int_model_t(field_in_tattoo_model_t* field_in_tattoo_model);
		~field_editor_int_model_t();
		void init();
		void update();

		const std::optional<input_method_int>& get_input_method();
		void set_input_method(const std::optional<input_method_int>& method);
		const std::optional<int*>& get_buffer_address();
	};

	class field_editor_float_model_t {
	private:
		field_in_tattoo_model_t::interest_handle field_in_tattoo_model_registered = 0;

		field_in_tattoo_model_t*          _field_in_tattoo_model;
		std::optional<input_method_float> _input_method;
		float                             _private_buf;
		std::optional<float*>             _buffer_address;
	public:
		field_editor_float_model_t(field_in_tattoo_model_t* field_in_tattoo_model);
		~field_editor_float_model_t();
		void init();
		void update();

		const std::optional<input_method_float>& get_input_method();
		void set_input_method(const std::optional<input_method_float>& method);
		const std::optional<float*>& get_buffer_address();
	};

	class field_editor_string_model_t {
	private:
		field_in_tattoo_model_t::interest_handle field_in_tattoo_model_registered = 0;

		field_in_tattoo_model_t*              _field_in_tattoo_model;
		std::array<char, 512>                 _private_buf;
		std::optional<std::array<char, 512>*> _buffer_address;
	public:
		field_editor_string_model_t(field_in_tattoo_model_t* field_in_tattoo_model);
		~field_editor_string_model_t();
		void init();
		void update();

		const std::optional<std::array<char, 512>*>& get_buffer_address();
	};


}
