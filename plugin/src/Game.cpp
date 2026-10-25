/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Game.h"
#include "Ids.h"

namespace Game
{
	namespace
	{
		Forms f;
		bool  dataOk = false;
		bool  scriptsOk = false;
		bool  ready = false;

		template <class T>
		T* Look(unsigned a_local, const char* a_plugin)
		{
			auto* dh = RE::TESDataHandler::GetSingleton();
			return dh ? dh->LookupForm<T>(a_local, a_plugin) : nullptr;
		}

		// The two companion mods come as a normal plugin and as an ESL build with every id renumbered.
		bool IsLight(const char* a_plugin)
		{
			auto* dh = RE::TESDataHandler::GetSingleton();
			const auto* file = dh ? dh->LookupModByName(a_plugin) : nullptr;
			return file && file->IsLight();
		}

		RE::TESGlobal* Companion(const char* a_plugin, unsigned a_normal, unsigned a_esl)
		{
			if (!Loaded(a_plugin)) {
				return nullptr;
			}
			return Look<RE::TESGlobal>(IsLight(a_plugin) ? a_esl : a_normal, a_plugin);
		}
	}

	bool Init()
	{
		using namespace ids;
		ready = false;
		dataOk = false;
		f = {};
		if (!Loaded(kSotw)) {
			return false;
		}
		auto G = [](unsigned id) { return Look<RE::TESGlobal>(id, kSotw); };
		f.cookProgress = G(CookProgress);
		f.cookPoints = G(CookPoints);
		f.cookEarned = G(CookEarned);
		f.cookTotal = G(CookTotal);
		f.cookXpToNext = G(CookXpToNext);
		f.cookBasic = G(CookBasicMeals);
		f.cookElaborate = G(CookElaborate);
		f.cookExotic = G(CookExotic);
		f.cookSauce = G(CookSpecialSauce);
		f.cookGourmet = G(CookGourmet);
		f.perkIsCooking = Look<RE::BGSPerk>(PerkIsCooking, kSotw);
		f.perkThings = Look<RE::BGSPerk>(PerkThings, kSotw);
		f.perkMortar = Look<RE::BGSPerk>(PerkMortar, kSotw);
		f.spellInspired1 = Look<RE::SpellItem>(SpellInspired1, kSotw);
		f.spellInspired2 = Look<RE::SpellItem>(SpellInspired2, kSotw);
		f.spellSitting = Look<RE::SpellItem>(SpellSitting, kSotw);
		f.spellCrossHair = Look<RE::SpellItem>(SpellCrossHair, kSotw);
		f.lowHeat1 = Look<RE::AlchemyItem>(PotionLowHeat1, kSotw);
		f.lowHeat2 = Look<RE::AlchemyItem>(PotionLowHeat2, kSotw);
		f.exotic1 = Look<RE::AlchemyItem>(PotionExotic1, kSotw);
		f.exotic2 = Look<RE::AlchemyItem>(PotionExotic2, kSotw);
		f.healthy1 = Look<RE::AlchemyItem>(PotionHealthy1, kSotw);
		f.healthy2 = Look<RE::AlchemyItem>(PotionHealthy2, kSotw);
		f.cookingBooks = Look<RE::BGSListForm>(ListCookingBooks, kSotw);
		f.ignoreFood = Look<RE::BGSListForm>(ListIgnoreFood, kSotw);
		f.knownRecipes = Look<RE::BGSListForm>(ListKnownRecipes, kSotw);

		f.huntProgress = G(HuntProgress);
		f.huntPoints = G(HuntPoints);
		f.huntEarned = G(HuntEarned);
		f.huntTotal = G(HuntTotal);
		f.huntArt = G(HuntArtRank);
		f.huntKills = G(HuntKills);
		f.huntingBooks = Look<RE::BGSListForm>(ListHuntingBooks, kSotw);

		f.wayProgress = G(WayProgress);
		f.wayEarned = G(WayEarned);
		f.wayMax = G(WayMax);
		f.wayPoints = G(WayPoints);
		f.wayLimit = G(WayLimit);
		f.wayXp = G(WayXp);
		f.waySixth = G(WaySixthSense);
		f.waySpatial = G(WaySpatial);
		f.wayBooks = Look<RE::BGSListForm>(ListWayBooks, kSotw);
		f.booksRead = Look<RE::BGSListForm>(ListBooksRead, kSotw);

		f.compassCheat = G(CompassCheat);
		f.defaultJump = G(DefaultJump);

		f.kwFood = Look<RE::BGSKeyword>(KwVendorFood, kSkyrim);
		f.kwIngredient = Look<RE::BGSKeyword>(KwVendorIngredient, kSkyrim);
		f.kwNoExp = Look<RE::BGSKeyword>(KwNoExp, kSkyrim);
		f.kwCookpot = Look<RE::BGSKeyword>(KwCookpot, kSkyrim);
		f.kwOven = Look<RE::BGSKeyword>(KwBYOHOven, kHearth);
		f.chefHat = Look<RE::TESObjectARMO>(ChefHat, kSkyrim);

		if (Loaded(kSho)) {
			f.shoXp = Look<RE::TESGlobal>(ShoXp, kSho);
			f.shoVanilla = Look<RE::BGSListForm>(ShoVanillaPelts, kSho);
			f.shoBruma = Look<RE::BGSListForm>(ShoBrumaPelts, kSho);
			f.shoCustom = Look<RE::BGSListForm>(ShoCustomPelts, kSho);
			f.shoBlacklist = Look<RE::BGSListForm>(ShoBlacklist, kSho);
		}
		f.ffRunning = Companion(kFrostfall, FfRunning, FfRunningEsl);
		f.ffExposure = Companion(kFrostfall, FfExposure, FfExposureEsl);
		f.ffExposureMax = Companion(kFrostfall, FfExposureMax, FfExposureMaxEsl);
		f.lsRunning = Companion(kLastSeed, LsRunning, LsRunningEsl);
		f.lsHunger = Companion(kLastSeed, LsHunger, LsHungerEsl);
		f.lsHungerMax = Companion(kLastSeed, LsHungerMax, LsHungerMaxEsl);

		// The three skill trees and the settings are what the plugin cannot work without; the rest degrades.
		dataOk = f.cookProgress && f.cookPoints && f.cookEarned && f.cookTotal && f.cookXpToNext && f.huntProgress && f.huntPoints && f.huntEarned && f.huntTotal && f.wayProgress &&
		        f.wayXp && f.wayLimit && f.wayPoints && f.wayEarned && f.wayMax && f.kwFood && f.kwIngredient && f.booksRead;
		ready = dataOk && scriptsOk;
		SKSE::log::info("Skills of the Wild data {}; Simple Hunting Overhaul {}, Frostfall {}, Last Seed {}", dataOk ? "found" : "INCOMPLETE - the native skill trees are off",
			f.shoXp ? "found" : "not installed", f.ffExposure ? "found" : "not installed", f.lsHunger ? "found" : "not installed");
		return dataOk;
	}

	const Forms& F() { return f; }
	bool         Ready() { return ready; }
	bool         ScriptsInstalled() { return scriptsOk; }
	bool         DataOk() { return dataOk; }

	namespace
	{
		// Does this compiled script mention SOTW2026? (the three patched scripts call it; the original ones, and the old "Re-enable Compass" patch's copy, do not)
		bool ScriptMentions(const char* a_file)
		{
			std::ifstream in(std::filesystem::path("Data") / "Scripts" / a_file, std::ios::binary);
			if (!in) {
				return false;
			}
			std::string bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
			for (auto& c : bytes) {
				c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));  // the compiler stores names in lower case
			}
			return bytes.find("sotw2026") != std::string::npos;
		}
	}

	bool CheckScripts()
	{
		// The scripts the game will really load are looked at, whichever mod they come from: all three have to be this layer's, or the original scripts
		// would count the same experience a second time.
		const bool found = ScriptMentions("SOTW2026.pex") && ScriptMentions("CookingScript.pex") && ScriptMentions("HuntingScript.pex") && ScriptMentions("_WWW_HUD.pex");
		if (found != scriptsOk) {
			if (found) {
				SKSE::log::info("The Skills of the Wild 2026 scripts are installed: the plugin runs the skill trees");
			} else {
				SKSE::log::info("The Skills of the Wild 2026 scripts (SOTW2026.pex and the three patched scripts) are NOT the ones the game loads: the plugin keeps out of the skill trees so nothing is counted twice");
			}
		}
		scriptsOk = found;
		ready = dataOk && scriptsOk;
		return found;
	}

	bool Loaded(const char* a_plugin)
	{
		auto* dh = RE::TESDataHandler::GetSingleton();
		return a_plugin && dh && dh->LookupModByName(a_plugin);
	}

	bool ProbeDll(const char* a_name)
	{
		return GetModuleHandleA(a_name) != nullptr;
	}

	float Val(const RE::TESGlobal* a_g, float a_default) { return a_g ? a_g->value : a_default; }

	void Set(RE::TESGlobal* a_g, float a_value)
	{
		if (a_g) {
			a_g->value = a_value;
		}
	}

	int Rank(const RE::TESGlobal* a_g) { return a_g ? static_cast<int>(a_g->value + 0.5f) : 0; }

	bool FrostfallRunning() { return f.ffRunning && static_cast<int>(f.ffRunning->value) == 2 && f.ffExposure; }
	bool LastSeedRunning() { return f.lsRunning && static_cast<int>(f.lsRunning->value) == 2 && f.lsHunger; }

	void Notify(const std::string& a_text)
	{
		SKSE::GetTaskInterface()->AddTask([a_text]() {
#if __has_include(<RE/S/SendHUDMessage.h>)
			RE::SendHUDMessage::ShowHUDMessage(a_text.c_str());
#else
			RE::DebugNotification(a_text.c_str());
#endif
		});
	}

	void Later(float a_seconds, std::function<void()> a_fn)
	{
		std::thread([a_seconds, fn = std::move(a_fn)]() mutable {
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(a_seconds * 1000.0f)));
			SKSE::GetTaskInterface()->AddTask(std::move(fn));
		}).detach();
	}

	RE::PlayerCharacter* Player() { return RE::PlayerCharacter::GetSingleton(); }

	bool Paused()
	{
		auto* ui = RE::UI::GetSingleton();
		return !ui || ui->GameIsPaused();
	}

	bool InCombat()
	{
		auto* p = RE::PlayerCharacter::GetSingleton();
		return p && p->IsInCombat();
	}

	float GameHours()
	{
		auto* cal = RE::Calendar::GetSingleton();
		return cal ? cal->GetDaysPassed() * 24.0f : 0.0f;
	}
}
