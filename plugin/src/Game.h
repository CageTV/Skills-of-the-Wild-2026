/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

#include <functional>

// The game data the plugin works with, looked up once the data is loaded, and a few helpers shared by the parts of the plugin.
namespace Game
{
	struct Forms
	{
		// Culinary Arts
		RE::TESGlobal *cookProgress{}, *cookPoints{}, *cookEarned{}, *cookTotal{}, *cookXpToNext{};
		RE::TESGlobal *cookBasic{}, *cookElaborate{}, *cookExotic{}, *cookSauce{}, *cookGourmet{};
		RE::BGSPerk *  perkIsCooking{}, *perkThings{}, *perkMortar{};
		RE::SpellItem *spellInspired1{}, *spellInspired2{}, *spellSitting{}, *spellCrossHair{};
		RE::AlchemyItem *lowHeat1{}, *lowHeat2{}, *exotic1{}, *exotic2{}, *healthy1{}, *healthy2{};
		RE::BGSListForm *cookingBooks{}, *ignoreFood{}, *knownRecipes{};
		// Art of the Hunt
		RE::TESGlobal *huntProgress{}, *huntPoints{}, *huntEarned{}, *huntTotal{}, *huntArt{}, *huntKills{};
		RE::BGSListForm *huntingBooks{};
		// Wayfarer
		RE::TESGlobal *wayProgress{}, *wayEarned{}, *wayMax{}, *wayPoints{}, *wayLimit{}, *wayXp{}, *waySixth{}, *waySpatial{};
		RE::BGSListForm *wayBooks{}, *booksRead{};
		// settings that lived in the console
		RE::TESGlobal *compassCheat{}, *defaultJump{};
		// keywords and items of the base game
		RE::BGSKeyword *kwFood{}, *kwIngredient{}, *kwNoExp{}, *kwCookpot{}, *kwOven{};
		RE::TESObjectARMO* chefHat{};
		// Simple Hunting Overhaul
		RE::TESGlobal*   shoXp{};
		RE::BGSListForm *shoVanilla{}, *shoBruma{}, *shoCustom{}, *shoBlacklist{};
		// Frostfall and Last Seed (null when not installed)
		RE::TESGlobal *ffRunning{}, *ffExposure{}, *ffExposureMax{};
		RE::TESGlobal *lsRunning{}, *lsHunger{}, *lsHungerMax{};
	};

	bool          Init();  // after the data is loaded; false when Skills of the Wild itself is not in the load order
	// After a save is loaded (the script system is up): are this layer's scripts installed? They are what stop the original scripts counting too.
	// Without them the plugin keeps out of the skill trees (Ready() stays false) so nothing is counted twice.
	bool          CheckScripts();
	bool          ScriptsInstalled();
	bool          DataOk();  // Skills of the Wild's records were all found
	const Forms&  F();
	bool          Ready();
	bool          Loaded(const char* a_plugin);
	bool          ProbeDll(const char* a_name);  // is that SKSE plugin DLL loaded in this game

	float Val(const RE::TESGlobal* a_g, float a_default = 0.0f);
	void  Set(RE::TESGlobal* a_g, float a_value);
	int   Rank(const RE::TESGlobal* a_g);

	bool FrostfallRunning();
	bool LastSeedRunning();

	void Notify(const std::string& a_text);                 // top-left message, from any thread
	void Later(float a_seconds, std::function<void()> a_fn);  // runs on the game thread after a delay

	RE::PlayerCharacter* Player();
	bool                 Paused();     // a menu is open and the game is paused
	bool                 InCombat();
	float                GameHours();  // total game time in hours
}
