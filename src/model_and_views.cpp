#include "model_and_views.h"
#include "slavetatsng_interface.h"

namespace slavetats_ui {

	void refresh_models(actor_model_t* actor_model, applied_tattoos_model_t* applied_tattoos, nioverride_model_t* nioverride_model) {
		auto actor = actor_model->get_actor();
		if (actor) {
			JFormDB::setInt(actor, ".SlaveTats.updated", 1); // guess, add_and_get_tattoo already does this, just to be safe
			slavetats::synchronize_tattoos(actor);
			// Remark: The synchronize_tattoos function isn't synchron anymore, so the init's here could be too early and have no effect.
			// But, the data is also periodically re-read, so at least at the next refresh everything displayed in the gui should be 
			// up-to-date again
			applied_tattoos->init();  // reread data, and notify dependent models
			nioverride_model->init(); // reread data, and notify dependent models
		}
	}

	slavetats_model_t::slavetats_model_t() {
		init();
	}

	void slavetats_model_t::init() {
		last_refresh = std::chrono::steady_clock::now();
		set_dirty(interest_mask{ dirty_flags::periodic_refresh });
	}
	
	// It would be very dangerous to call 'set_dirty' automatically here periodically, because this would cause other models
	// to re-read their data, which would invalidate references in the midst of function executions (crash!), call refresh() instead,
	// when it is save to update the models
	void slavetats_model_t::update() {
	}

	void slavetats_model_t::refresh() {
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refresh) > 500ms) {
			init();
		}
	}

	//################################################################################################################
	
	installed_tattoos_model_t::installed_tattoos_model_t() {
		slavetats_model_registered = slavetats_model_t::GetSingleton()->interest_register(
			slavetats_model_t::interest_mask{
				slavetats_model_t::dirty_flags::periodic_refresh
			});
		init();
	}

	installed_tattoos_model_t::~installed_tattoos_model_t() {
		slavetats_model_t::GetSingleton()->interest_unregister(slavetats_model_registered);
	}

	void installed_tattoos_model_t::init() {
		_installed_tattoos.clear();
		read_slavetats_installed_tattoos(_installed_tattoos);
		_installed_tattoo_names.clear();
		read_slavetats_installed_tattoo_names(_installed_tattoos, _installed_tattoo_names);
		set_dirty(interest_mask{ dirty_flags::updated });
	}

	void installed_tattoos_model_t::update() {
		auto dirty = slavetats_model_t::GetSingleton()->test_and_clear_dirty(slavetats_model_registered);
		if (dirty) {
			init();
		}
	}

	const area_sections_t& installed_tattoos_model_t::installed_tattoos() {
		update();
		return _installed_tattoos;
	}

	const area_section_tattoo_names_t& installed_tattoos_model_t::installed_tattoo_names() {
		update();
		return _installed_tattoo_names;
	}

	//#########################################################################################################

	actor_scanner_model::actor_scanner_model() {
		// last_scan = std::chrono::steady_clock::now();
		init();
	}

	void actor_scanner_model::init() {
		_actors = find_actors();
		_actor_labels.clear();
		for (auto actor : _actors) {
			const auto base = actor->GetActorBase();
			_actor_labels.push_back(std::format("{:#010x} ({})", actor->GetFormID(), base->GetName()));
		}
	}

	void actor_scanner_model::update() {
		/*auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_scan) > 1000ms) {
			last_scan = std::chrono::steady_clock::now();
			_actors = find_actors();
			_actor_labels.clear();
			for (auto actor : _actors) {
				const auto base = actor->GetActorBase();
				_actor_labels.push_back(std::format("{:#010x} ({})", actor->GetFormID(), base->GetName()));
			}
		}*/
	}

	const std::vector<RE::Actor*>& actor_scanner_model::actors() {
		update();
		return _actors;
	}

	const std::vector<std::string>& actor_scanner_model::actor_labels() {
		update();
		return _actor_labels;
	}

	//#########################################################################################################

	RE::Actor* actor_model_t::get_actor() {
		return actor;
	}

	const std::string& actor_model_t::get_actor_label() {
		return actor_label;
	}

	void actor_model_t::set_actor(RE::Actor* a_actor) {
		if (a_actor != actor) {
			actor = a_actor;
			if (actor) {
				actor_label = std::format("{:#010x} ({})", actor->GetFormID(), actor->GetActorBase()->GetName());
			}
			else actor_label.clear();
			set_dirty(interest_mask{ dirty_flags::selected_actor });
		}
	}

	//#########################################################################################################

	area_model_t::area_model_t(): _area(tattoo_area::no_value) {

	}

	void area_model_t::update() {

	}
	
	void area_model_t::init() {

	}


	tattoo_area area_model_t::get_area() {
		return _area;
	}
	
	
	void area_model_t::set_area(tattoo_area a_area) {
		if (a_area != _area) {
			_area = a_area;
			set_dirty(interest_mask{ dirty_flags::selected_area });
		}
	}


	//#########################################################################################################

	nioverride_model_t::nioverride_model_t(actor_model_t* a_actor_model): actor_model(a_actor_model) {
		actor_model_registered = actor_model->interest_register(
			actor_model_t::interest_mask{
				actor_model_t::dirty_flags::selected_actor
			});
		slavetats_model_registered = slavetats_model_t::GetSingleton()->interest_register(
			slavetats_model_t::interest_mask{
				slavetats_model_t::dirty_flags::periodic_refresh
			});
		init();
	}

	void nioverride_model_t::init() {
		// last_scan = std::chrono::steady_clock::now();
		auto actor = actor_model->get_actor();
		if (actor) {
			actor_overrides_t tmp;
			read_actor_overrides(actor, tmp);
			_overrides.emplace(std::move(tmp));
		}
		else {
			_overrides.reset();
		}
		set_dirty(interest_mask{ dirty_flags::updated });
	}

	nioverride_model_t::~nioverride_model_t() {
		actor_model->interest_unregister(actor_model_registered);
		slavetats_model_t::GetSingleton()->interest_unregister(slavetats_model_registered);
	}

	void nioverride_model_t::update() {
		bool dirty_st = slavetats_model_t::GetSingleton()->test_and_clear_dirty(slavetats_model_registered);
		bool dirty_a = actor_model->test_and_clear_dirty(actor_model_registered);
		
		bool refresh = dirty_st || dirty_a;
	
		if (refresh) {
			init();
		}
	}

	const std::optional<actor_overrides_t>& nioverride_model_t::overrides() {
		update();
		return _overrides;
	}

	//#########################################################################################################

	applied_tattoos_model_t::applied_tattoos_model_t(actor_model_t* a_actor_model) : actor_model(a_actor_model) {
		actor_model_registered = actor_model->interest_register(
			actor_model_t::interest_mask{
				actor_model_t::dirty_flags::selected_actor
			});
		slavetats_model_registered = slavetats_model_t::GetSingleton()->interest_register(
			slavetats_model_t::interest_mask{
				slavetats_model_t::dirty_flags::periodic_refresh
			});
		init();
	}

	applied_tattoos_model_t::~applied_tattoos_model_t() {
		actor_model->interest_unregister(actor_model_registered);
		slavetats_model_t::GetSingleton()->interest_unregister(slavetats_model_registered);
	}

	void applied_tattoos_model_t::init() {
		// last_scan = std::chrono::steady_clock::now();
		auto actor = actor_model->get_actor();
		if (actor) {
			jactor_tattoos_t jtattoos_by_id;
			jread_actor_tattoos(actor, jtattoos_by_id);

			jid_by_area_slot_t jtattoos_by_key;
			jtattoos_by_area_slot(jtattoos_by_id, jtattoos_by_key);
			_tattoo_ids.emplace(std::move(jtattoos_by_key));

			_tattoos.emplace(std::move(jtattoos_by_id));

			_tattoo_version = JFormDB::getStr(actor, ".SlaveTats.version");
		}
		else {
			_tattoos.reset();
			_tattoo_ids.reset();
			_tattoo_version.clear();
		}
		set_dirty(interest_mask{ dirty_flags::updated });
	}

	void applied_tattoos_model_t::update() {

		bool dirty_st = slavetats_model_t::GetSingleton()->test_and_clear_dirty(slavetats_model_registered);
		bool dirty_a = actor_model->test_and_clear_dirty(actor_model_registered);

		bool refresh = dirty_st || dirty_a;

		if (refresh) {
			init();
		}
	}

	const std::optional<jactor_tattoos_t>& applied_tattoos_model_t::tattoos() {
		update();
		return _tattoos;
	}
	
	const std::optional<jid_by_area_slot_t>&  applied_tattoos_model_t::tattoo_ids() {
		update();
		return _tattoo_ids;
	}

	const std::string& applied_tattoos_model_t::tattoo_version() {
		update();
		return _tattoo_version;
	}

	//#########################################################################################################

	slots_model_t::slots_model_t(applied_tattoos_model_t* applied_tattoos_model, nioverride_model_t* nioverride_model)
		: _applied_tattoos_model(applied_tattoos_model), _nioverride_model(nioverride_model) {
		applied_tattoos_model_registered = applied_tattoos_model->interest_register(
			applied_tattoos_model_t::interest_mask{
				applied_tattoos_model_t::dirty_flags::updated
			});
		nioverride_model_registered = nioverride_model->interest_register(
			nioverride_model_t::interest_mask{
				nioverride_model_t::dirty_flags::updated
			});
		init();
	}

	slots_model_t::~slots_model_t() {
		_applied_tattoos_model->interest_unregister(applied_tattoos_model_registered);
		_nioverride_model->interest_unregister(nioverride_model_registered);
	}

	void slots_model_t::update() {
		// Models must be up-to-date, before checking if they are dirty, otherwise status is inconsistent. And, even worse, I suspect,
		// if the models would depend on each other, they would have to be updated in a certain order, or multiple times, before the dirty flags would be right.
		//_applied_tattoos_model->update();
		//_nioverride_model->update();

		bool dirty_a = _applied_tattoos_model->test_and_clear_dirty(applied_tattoos_model_registered);
		bool dirty_n = _nioverride_model->test_and_clear_dirty(nioverride_model_registered);
		
		bool refresh = dirty_a || dirty_n;
			
		if (refresh) {
			init();
		}
	}

	void slots_model_t::init() {
		_slot_infos.clear();
		if (_applied_tattoos_model->tattoo_ids().has_value() && _applied_tattoos_model->tattoos().has_value()
			&& _nioverride_model->overrides().has_value()) {

			auto& tattoos = _applied_tattoos_model->tattoos().value();
			auto& overrides = _nioverride_model->overrides().value();
			auto& tattoo_ids = _applied_tattoos_model->tattoo_ids().value();

			read_slot_infos(tattoos, tattoo_ids, overrides, _slot_infos);
		}
		set_dirty(interest_mask{ dirty_flags::updated });
	}

	const area_slots_t& slots_model_t::slot_infos() {
		update();
		return _slot_infos;
	}

	//#########################################################################################################

	selected_slot_model_t::selected_slot_model_t(area_model_t* area_model, slots_model_t* slots_model) : _area_model(area_model), _slots_model(slots_model) {
		area_model_registered = area_model->interest_register(
			area_model_t::interest_mask{
				area_model_t::dirty_flags::selected_area
			});
		slots_model_registered = _slots_model->interest_register(
			slots_model_t::interest_mask{
				slots_model_t::dirty_flags::updated
			});
		init();
	}

	selected_slot_model_t::~selected_slot_model_t() {
		_area_model->interest_unregister(area_model_registered);
		_slots_model->interest_unregister(slots_model_registered);
	}

	void selected_slot_model_t::init() {
		// check, that selected_slot is still valid
		if (_selected_slot.has_value()) {
			auto& slot_infos = _slots_model->slot_infos();
			auto area = _area_model->get_area();
			auto& selected = _selected_slot.value();
			if (selected.area == area) {
				auto area_it = slot_infos.find(selected.area);
				if (selected.slot_num >= 0 && selected.slot_num < area_it->second.size()) {
					auto info = area_it->second.at(selected.slot_num);
					if (selected != info) {
						_selected_slot.reset();
						set_dirty(interest_mask{ dirty_flags::updated });
					}
				}
				else {
					_selected_slot.reset();
					set_dirty(interest_mask{ dirty_flags::updated });
				}
			}
			else {
				_selected_slot.reset();
				set_dirty(interest_mask{ dirty_flags::updated });
			}
		}
	}

	void selected_slot_model_t::update() {
		bool dirty_a = _area_model->test_and_clear_dirty(area_model_registered);
		bool dirty_s = _slots_model->test_and_clear_dirty(slots_model_registered);

		bool refresh = dirty_a || dirty_s;

		if (refresh) {
			init();
		}
	}

	const std::optional<slot_info_t>& selected_slot_model_t::selected_slot() {
		update();
		return _selected_slot;
	}

	void selected_slot_model_t::set_selected_slot(const std::optional<slot_info_t>& slot_info) {
		_selected_slot = slot_info;
		set_dirty(interest_mask{ dirty_flags::updated });
	}

	//#########################################################################################################

	selected_section_model_t::selected_section_model_t(area_model_t* area_model) //, installed_tattoos_model_t* installed_tattoos)
	: _area_model(area_model) {  //, _installed_tattoos_model(installed_tattoos) {
		area_model_registered = area_model->interest_register(
			area_model_t::interest_mask{
				area_model_t::dirty_flags::selected_area
			});
		installed_tattoos_model_registered = installed_tattoos_model_t::GetSingleton()->interest_register(
			installed_tattoos_model_t::interest_mask {
				installed_tattoos_model_t::dirty_flags::updated
			});
		init();
	}
	
	selected_section_model_t::~selected_section_model_t() {
		_area_model->interest_unregister(area_model_registered);
		installed_tattoos_model_t::GetSingleton()->interest_unregister(installed_tattoos_model_registered);
	}
	
	void selected_section_model_t::init() {
		if (_selected_section.has_value()) {
			if (_area_model->get_area() != _selected_section.value().area) {
				_selected_section.reset();
				set_dirty(interest_mask{ dirty_flags::updated });
			}
			else {
				auto area_it = installed_tattoos_model_t::GetSingleton()->installed_tattoo_names().find(_selected_section.value().area);
				if (area_it == installed_tattoos_model_t::GetSingleton()->installed_tattoo_names().end()) {
					_selected_section.reset();
					set_dirty(interest_mask{ dirty_flags::updated });
				}
				else {
					auto section_it = area_it->second.find(_selected_section.value().section);
					if (section_it == area_it->second.end()) {
						_selected_section.reset();
						set_dirty(interest_mask{ dirty_flags::updated });
					}
				}
			}
		}
	}

	void selected_section_model_t::update() {
		bool dirty_a = _area_model->test_and_clear_dirty(area_model_registered);
		bool dirty_i = installed_tattoos_model_t::GetSingleton()->test_and_clear_dirty(installed_tattoos_model_registered);

		bool refresh = dirty_a || dirty_i;

		if (refresh) {
			init();
		}
	}

	const std::optional<selected_section_model_t::selection>& selected_section_model_t::get_selected_section() {
		update();
		return _selected_section;
	}
	
	void selected_section_model_t::set_selected_section(const std::optional<selected_section_model_t::selection>& section) {
		_selected_section = section;
		set_dirty(interest_mask{ dirty_flags::updated });
	}

	//#########################################################################################################

	tattoo_in_section_model_t::tattoo_in_section_model_t(area_model_t* area_model, selected_section_model_t* selected_section)
		: _area_model(area_model), _selected_section_model(selected_section) {
		area_model_registered = area_model->interest_register(
			area_model_t::interest_mask{
				area_model_t::dirty_flags::selected_area
			});
		installed_tattoos_model_registered = installed_tattoos_model_t::GetSingleton()->interest_register(
			installed_tattoos_model_t::interest_mask{
				installed_tattoos_model_t::dirty_flags::updated
			});
		selected_section_model_registered = _selected_section_model->interest_register(
			selected_section_model_t::interest_mask{
				selected_section_model_t::dirty_flags::updated
			});
		init();
	}

	tattoo_in_section_model_t::~tattoo_in_section_model_t() {
		_area_model->interest_unregister(area_model_registered);
		installed_tattoos_model_t::GetSingleton()->interest_unregister(installed_tattoos_model_registered);
		_selected_section_model->interest_unregister(selected_section_model_registered);
	}

	void tattoo_in_section_model_t::init() {
		if (_selected_tattoo.has_value()) {
			if (_area_model->get_area() != _selected_tattoo.value().area)
				_selected_tattoo.reset();
			else {
				auto area_it = installed_tattoos_model_t::GetSingleton()->installed_tattoo_names().find(_selected_tattoo.value().area);
				if (area_it == installed_tattoos_model_t::GetSingleton()->installed_tattoo_names().end())
					_selected_tattoo.reset();
				else {
					auto section_it = area_it->second.find(_selected_tattoo.value().section);
					if (section_it == area_it->second.end())
						_selected_tattoo.reset();
					else {
						auto tattoo_it = section_it->second.find(_selected_tattoo.value().name);
						if (tattoo_it == section_it->second.end() || tattoo_it->second != _selected_tattoo.value().tattoo_id)
							_selected_tattoo.reset();
					}
				}
			}
		}
	}

	void tattoo_in_section_model_t::update() {
		bool dirty_a = _area_model->test_and_clear_dirty(area_model_registered);
		bool dirty_i = installed_tattoos_model_t::GetSingleton()->test_and_clear_dirty(installed_tattoos_model_registered);
		bool dirty_s = _selected_section_model->test_and_clear_dirty(selected_section_model_registered);

		bool refresh = dirty_a || dirty_i || dirty_s;

		if (refresh) {
			init();
		}
	}

	const std::optional<tattoo_in_section_model_t::selection>& tattoo_in_section_model_t::get_selected_tattoo() {
		update();
		return _selected_tattoo;
	}

	void tattoo_in_section_model_t::set_selected_tattoo(const std::optional<tattoo_in_section_model_t::selection>& selected_tattoo) {
		_selected_tattoo = selected_tattoo;
	}

	//#########################################################################################################

	field_in_tattoo_model_t::field_in_tattoo_model_t(applied_tattoos_model_t* applied_tattoos_model, selected_slot_model_t* selected_slot_model)
		: _applied_tattoos_model(applied_tattoos_model), _selected_slot_model(selected_slot_model) {
		applied_tattoos_model_registered = _applied_tattoos_model->interest_register(
			applied_tattoos_model_t::interest_mask{
				applied_tattoos_model_t::dirty_flags::updated
			});
		selected_slot_model_registered = _selected_slot_model->interest_register(
			selected_slot_model_t::interest_mask{
				selected_slot_model_t::dirty_flags::updated
			});
		init();
	}

	field_in_tattoo_model_t::~field_in_tattoo_model_t() {
		_applied_tattoos_model->interest_unregister(applied_tattoos_model_registered);
		_selected_slot_model->interest_unregister(selected_slot_model_registered);
	}

	void field_in_tattoo_model_t::init() {
		bool dirty = false;
		if (!_selected_slot_model->selected_slot().has_value()) {
			if (_selected_field.has_value()) {
				dirty = true;
			}
		}
		else {
			if (_selected_field.has_value()) {
				if (_selected_field.value().tattoo_id != _selected_slot_model->selected_slot().value().tattoo_id) {
					dirty = true;
				}
				else {
					auto& tattoos = _applied_tattoos_model->tattoos().value();
					auto tattoo_id = _selected_slot_model->selected_slot().value().tattoo_id;
					auto tattoo_it = tattoos.find(tattoo_id);
					if (tattoo_it == tattoos.end()) {
						dirty = true;
					}
					else {
						if (_selected_field.value().field_name != "[new]") { // 'new field' selection is always valid
							auto& fields = tattoo_it->second;
							auto field_it = fields.find(_selected_field.value().field_name);
							if (field_it == fields.end()) {  // tattoo doesn't contain the field anymore
								dirty = true;
							}
							else { // check, if the value of the field has changed
								auto& old = _selected_field.value();
								if (field_it->second != old.field_value) {
									_selected_field = std::optional(selection{ old.field_name, field_it->second, old.tattoo_id, old.field_definition });
									set_dirty(interest_mask{ dirty_flags::field_value_changed });
								}
							}
						}
					}
				}
			}
		}
		if (dirty) {
			_selected_field.reset();
			_selected_field_definition.reset();
			set_dirty(interest_mask{ dirty_flags::selection_changed,  dirty_flags::field_value_changed });
		}
	}

	void field_in_tattoo_model_t::update() {
		bool dirty_a = _applied_tattoos_model->test_and_clear_dirty(applied_tattoos_model_registered);
		bool dirty_s = _selected_slot_model->test_and_clear_dirty(selected_slot_model_registered);

		bool refresh = dirty_a || dirty_s;

		if (refresh) {
			init();
		}
	}

	const std::optional<field_in_tattoo_model_t::selection>& field_in_tattoo_model_t::get_selected_field() {
		update();
		return _selected_field;
	}
	
	void field_in_tattoo_model_t::set_selected_field(const std::optional<field_in_tattoo_model_t::selection>& field) {
		_selected_field = field;
		set_dirty(interest_mask{ dirty_flags::selection_changed,  dirty_flags::field_value_changed });
	}

	const std::optional<field_db::field_definition>& field_in_tattoo_model_t::get_selected_definition() {
		update();
		return _selected_field_definition;
	}

	void field_in_tattoo_model_t::set_selected_definition(const std::optional<field_db::field_definition>& field_def) {
		_selected_field_definition = field_def;
		set_dirty(interest_mask{ dirty_flags::selection_changed,  dirty_flags::field_value_changed });
	}

	//#########################################################################################################

	field_editor_int_model_t::field_editor_int_model_t(field_in_tattoo_model_t* field_in_tattoo_model)
	: _field_in_tattoo_model(field_in_tattoo_model), _private_buf(0) {
		field_in_tattoo_model_registered = _field_in_tattoo_model->interest_register(
			field_in_tattoo_model_t::interest_mask{
				field_in_tattoo_model_t::dirty_flags::selection_changed
			});
	}
	
	field_editor_int_model_t::~field_editor_int_model_t() {
		_field_in_tattoo_model->interest_unregister(field_in_tattoo_model_registered);
	}

	void field_editor_int_model_t::init() {
		if (_field_in_tattoo_model->get_selected_field().has_value() && _field_in_tattoo_model->get_selected_definition().has_value()) {
			
			auto& field = _field_in_tattoo_model->get_selected_field().value();
			auto& field_def = _field_in_tattoo_model->get_selected_definition().value();

			if (field_def.type == jvalue_type::j_int) {
				// change selected input method, if necessary
				if (_input_method.has_value()) {
					auto method = _input_method.value();
					if ((method == input_method_int::color_pick_wheel || method == input_method_int::color_pick_palette || method == input_method_int::color_pick_bar)
						&& field_def.range != field_db::value_range::rgb) {
						_input_method = std::optional(input_method_int::dec_fmt);
					}
					else {
						if ((method == input_method_int::dec_fmt || method == input_method_int::hex_fmt) && field_def.range == field_db::value_range::rgb)
							_input_method = std::optional(input_method_int::color_pick_palette);

					}
				}
				else {
					if (field_def.range == field_db::value_range::rgb) {
						_input_method = std::optional(input_method_int::color_pick_palette);
					}
					else {
						_input_method = std::optional(input_method_int::dec_fmt);
					}
				}

				// set initial value
				if (field.field_value.type == jvalue_type::j_int) {  // existing field (not "[new]")
					_private_buf = std::get<int>(field.field_value.value);
				}
				else {
					_private_buf = 0;   // default value 0
				}
				_buffer_address = std::optional(&_private_buf);
			}
			else
				_buffer_address.reset();
		}
		else {
			_buffer_address.reset();
		}
	}

	void field_editor_int_model_t::update() {
		bool dirty_f = _field_in_tattoo_model->test_and_clear_dirty(field_in_tattoo_model_registered);

		bool refresh = dirty_f;

		if (refresh) {
			init();
		}
	}
	
	const std::optional<input_method_int>& field_editor_int_model_t::get_input_method() {
		return _input_method;
	}
	
	void field_editor_int_model_t::set_input_method(const std::optional<input_method_int>& method) {
		_input_method = method;
	}

	const std::optional<int*>& field_editor_int_model_t::get_buffer_address() {
		update();
		return _buffer_address;
	}

	//#########################################################################################################

	field_editor_float_model_t::field_editor_float_model_t(field_in_tattoo_model_t* field_in_tattoo_model)
		: _field_in_tattoo_model(field_in_tattoo_model), _private_buf(0.f) {
		field_in_tattoo_model_registered = _field_in_tattoo_model->interest_register(
			field_in_tattoo_model_t::interest_mask{
				field_in_tattoo_model_t::dirty_flags::selection_changed
			});
	}

	field_editor_float_model_t::~field_editor_float_model_t() {
		_field_in_tattoo_model->interest_unregister(field_in_tattoo_model_registered);
	}

	void field_editor_float_model_t::init() {
		if (_field_in_tattoo_model->get_selected_field().has_value() && _field_in_tattoo_model->get_selected_definition().has_value()) {

			auto& field = _field_in_tattoo_model->get_selected_field().value();
			auto& field_def = _field_in_tattoo_model->get_selected_definition().value();

			if (field_def.type == jvalue_type::j_float) {
				// change selected input method, if necessary
				if (_input_method.has_value()) {
					auto method = _input_method.value();
					if ((method == input_method_float::slider_0_to_1)
						&& field_def.range != field_db::value_range::between_0_and_1) {
						_input_method = std::optional(input_method_float::dec_fmt);
					}
					else {
						if ((method == input_method_float::dec_fmt || method == input_method_float::hex_fmt) && field_def.range == field_db::value_range::between_0_and_1)
							_input_method = std::optional(input_method_float::slider_0_to_1);
					}
				}
				else {
					if (field_def.range == field_db::value_range::between_0_and_1) {
						_input_method = std::optional(input_method_float::slider_0_to_1);
					}
					else {
						_input_method = std::optional(input_method_float::dec_fmt);
					}
				}

				// set initial value
				if (field.field_value.type == jvalue_type::j_float) {  // existing field (not "[new]")
					_private_buf = std::get<float>(field.field_value.value);
				}
				else {
					_private_buf = 0.f;   // default value 0
				}
				_buffer_address = std::optional(&_private_buf);
			}
			else
				_buffer_address.reset();
		}
		else {
			_buffer_address.reset();
		}
	}

	void field_editor_float_model_t::update() {
		bool dirty_f = _field_in_tattoo_model->test_and_clear_dirty(field_in_tattoo_model_registered);

		bool refresh = dirty_f;

		if (refresh) {
			init();
		}
	}

	const std::optional<input_method_float>& field_editor_float_model_t::get_input_method() {
		return _input_method;
	}

	void field_editor_float_model_t::set_input_method(const std::optional<input_method_float>& method) {
		_input_method = method;
	}

	const std::optional<float*>& field_editor_float_model_t::get_buffer_address() {
		update();
		return _buffer_address;
	}



	//#########################################################################################################

	field_editor_string_model_t::field_editor_string_model_t(field_in_tattoo_model_t* field_in_tattoo_model)
		: _field_in_tattoo_model(field_in_tattoo_model), _private_buf("") {
		field_in_tattoo_model_registered = _field_in_tattoo_model->interest_register(
			field_in_tattoo_model_t::interest_mask{
				field_in_tattoo_model_t::dirty_flags::selection_changed
			});
	}

	field_editor_string_model_t::~field_editor_string_model_t() {
		_field_in_tattoo_model->interest_unregister(field_in_tattoo_model_registered);
	}

	void field_editor_string_model_t::init() {
		if (_field_in_tattoo_model->get_selected_field().has_value() && _field_in_tattoo_model->get_selected_definition().has_value()) {

			auto& field = _field_in_tattoo_model->get_selected_field().value();
			auto& field_def = _field_in_tattoo_model->get_selected_definition().value();

			if (field_def.type == jvalue_type::j_string) {
				// set initial value
				if (field.field_value.type == jvalue_type::j_string) {  // existing field (not "[new]")
					auto& value = std::get<std::string>(field.field_value.value);
					int count = value.size() < _private_buf.size() ? value.size() : _private_buf.size() - 1;
					memcpy(_private_buf.data(), value.data(), count);
					_private_buf[count] = '\0';
				}
				else {
					_private_buf[0] = '\0';   // default value empty string
				}
				_buffer_address = std::optional(&_private_buf);
			}
			else
				_buffer_address.reset();
		}
		else {
			_buffer_address.reset();
		}
	}

	void field_editor_string_model_t::update() {
		bool dirty_f = _field_in_tattoo_model->test_and_clear_dirty(field_in_tattoo_model_registered);

		bool refresh = dirty_f;

		if (refresh) {
			init();
		}
	}

	const std::optional<std::array<char, 512>*>& field_editor_string_model_t::get_buffer_address() {
		update();
		return _buffer_address;
	}



}
