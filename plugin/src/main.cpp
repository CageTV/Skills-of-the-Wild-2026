/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Game.h"
#include "Hud.h"
#include "Menu.h"
#include "Settings.h"
#include "Skills.h"
#include "State.h"
#include "Synergy.h"

namespace
{
	void SetupLog()
	{
		const auto dir = SKSE::log::log_directory();
		if (!dir) {
			return;
		}
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>((*dir / "SkillsOfTheWild.log").string(), true);
		auto log = std::make_shared<spdlog::logger>("SkillsOfTheWild", std::move(sink));
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
		log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
		spdlog::set_default_logger(std::move(log));
	}

	// Papyrus asks this before running the original skill tree code: when the plugin does the work, the scripts step aside.
	bool PapyrusActive(RE::StaticFunctionTag*) { return Game::Ready(); }

	bool RegisterPapyrus(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("Active", "SOTW2026", PapyrusActive);
		return true;
	}

	std::atomic<int> scriptChecks{ 0 };

	void TickAll()
	{
		if (Game::DataOk() && !Game::ScriptsInstalled() && scriptChecks.load() < 20 && !Game::Paused()) {
			// the script system may not have loaded the script layer yet when the save finished loading: ask again for a minute
			++scriptChecks;
			Game::CheckScripts();
		}
		if (!Game::Ready() || Game::Paused()) {
			return;
		}
		static float lastHours = -1.0f;
		const float  now = Game::GameHours();
		float        delta = lastHours < 0.0f ? 0.0f : now - lastHours;
		lastHours = now;
		if (delta < 0.0f || delta > 72.0f) {
			delta = 0.0f;  // a different save was loaded
		}
		Skills::Tick();
		Synergy::Tick(delta);
		Hud::Tick();
	}

	void StartTicker()
	{
		static std::atomic<bool> started{ false };
		if (started.exchange(true)) {
			return;
		}
		std::thread([]() {
			for (;;) {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				SKSE::GetTaskInterface()->AddTask(TickAll);
			}
		}).detach();
	}

	void OnMessage(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kPostLoad:
			Menu::Register();  // SKSE Menu Framework loads after us alphabetically, so register once everything is loaded
			break;
		case SKSE::MessagingInterface::kDataLoaded:
			Game::Init();
			Game::CheckScripts();
			Settings::OnDataLoaded();
			Skills::Init();
			StartTicker();
			break;
		case SKSE::MessagingInterface::kPostLoadGame:
		case SKSE::MessagingInterface::kNewGame:
			scriptChecks = 0;
			Game::CheckScripts();
			Settings::OnGameLoaded();
			Skills::OnGameLoaded();
			Hud::OnGameLoaded();
			break;
		default:
			break;
		}
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	SetupLog();
	SKSE::log::info("SkillsOfTheWild.dll 1.0.1, game {}", REL::Module::get().version().string());
	State::Register();
	SKSE::GetPapyrusInterface()->Register(RegisterPapyrus);
	SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
	return true;
}
