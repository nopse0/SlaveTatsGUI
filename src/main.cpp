#include <SlaveTatsNG_Interface.h>
#include "jcontainers_wrapper.h"
#include "nioverride_wrapper.h"
#include "config.h"
#include "UI.h"

namespace
{
	std::string get_jcontainers_dllname()
	{
		auto patchVersion = REL::Module::get().version().patch();
		std::string pluginName{ "JContainers64" };
		if (REL::Module::IsVR()) {
			pluginName = "JContainersVR";
		}
		else if (patchVersion == 659 || patchVersion == 1179) {
			pluginName = "JContainersGOG";
		}
		return pluginName;
	}

	void initialize_logging()
	{
		auto path = logger::log_directory();
		if (!path) {
			SKSE::stl::report_and_fail("Failed to find standard logging directory"sv);
		}

		auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
		*path /= fmt::format(FMT_STRING("{}.log"), pluginName);

		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

		auto pluginVersion = SKSE::PluginDeclaration::GetSingleton()->GetVersion();
		logger::info(FMT_STRING("{} v{}"), pluginName, pluginVersion.string());
	}

	void messaging_hook(SKSE::MessagingInterface::Message* a_message)
	{
		switch (a_message->type) {
		case SKSE::MessagingInterface::kPostLoad:
		{
			std::string pluginName = get_jcontainers_dllname();
			logger::info("JContainers Plugin Name seems to be: {}", pluginName);
			SKSE::GetMessagingInterface()->RegisterListener(pluginName.c_str(), [](SKSE::MessagingInterface::Message* a_msg) {
				logger::info("a_msg={}, msgtype={}, message_root_interface={}", (void*)a_msg, a_msg ? a_msg->type : -1, (int)jc::message_root_interface);
				if (a_msg && a_msg->type == jc::message_root_interface) {
					const jc::root_interface* root = jc::root_interface::from_void(a_msg->data);
					logger::info("root_interface={}", (void*)root);
					if (root)
						// Seems to be a deadlock or so here, so we defer the actual JContainers initialization
						slavetats_ng::jcwrapper::JCWrapper::GetSingleton()->PreInit(root);
				}
				});
		
			const char* slavetatsPlugin = "SlaveTatsNG";
			SKSE::GetMessagingInterface()->RegisterListener(slavetatsPlugin, [](SKSE::MessagingInterface::Message* a_msg) {
				// logger::info("a_msg={}, msgtype={}, message_root_interface={}", (void*)a_msg, a_msg ? a_msg->type : -1, (int)jc::message_root_interface);
				if (a_msg && a_msg->type == slavetats::interface::MessageType::Interface) {
					const slavetats::Addresses* iface = slavetats::Addresses::from_void(a_msg->data);
					slavetats::interface::singleton::get()->iface = iface;
					logger::info("SlaveTatsNG Interface found: address = {}", (void*)iface);
					logger::info("simple_add_tattoo: address = {}", (void*)iface->simple_add_tattoo);
				}
				});
		
		
		}
			break;

		case SKSE::MessagingInterface::kPostPostLoad:
			break;

		case SKSE::MessagingInterface::kDataLoaded:
		{
			slavetats_ng::jcwrapper::JCWrapper::GetSingleton()->Init();
			slavetats_ng::skee_wrapper::NiOverride::Init();
		}
			break;

		case SKSE::MessagingInterface::kSaveGame:
			break;

		case SKSE::MessagingInterface::kPostLoadGame:
			break;

		default:
			break;
		}
	}
}

namespace slavetats_ng
{
	const char* const config_file = "data/skse/plugins/SlaveTatsNG/SlaveTatsNG.ini";
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	initialize_logging();

	logger::info("Game version : {}", a_skse->RuntimeVersion().string());

	// Duplicated from SlaveTatsNG, not nice
	CSimpleIniA config;
	SI_Error    rc = config.LoadFile(slavetats_ng::config_file);
	logger::info("Load {}, result = {}", slavetats_ng::config_file, rc);
	auto ini = slavetats_ng::config::Config::GetSingleton();
	logger::info("blank_texture_name = {}", ini->blank_texture_name);
	if (rc >= 0) {
		clib_util::ini::get_value(config, ini->skee_dll_name, "Config", "skeeDllName", ";");
		logger::info("skee_dll_name = {}", ini->skee_dll_name);
		clib_util::ini::get_value(config, ini->blank_texture_name, "Config", "blankTextureName", ";");
		logger::info("blank_texture_name = {}", ini->blank_texture_name);
		clib_util::ini::get_value(config, ini->use_vmhook, "Config", "vmHook", ";");
		logger::info("blank_texture_name = {}", ini->blank_texture_name);
	}

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(messaging_hook);

	slavetats_ui::SlaveTatsUI::initialize();
	
	return true;
}
