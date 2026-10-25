/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// The three experience-driven skill trees of Skills of the Wild (Culinary Arts, Art of the Hunt, Wayfarer), done by the plugin instead of by the
// mod's scripts: the scripts looked at kills, days and places only when the player sat down, and recognised cooking by a sit-down trick that only
// knew two kinds of cooking station. The plugin works from the game's own events, so experience arrives when it is earned, at any cooking station.
// It writes the same globals the scripts did, so saves, the Campfire skill menu and other mods' patches see no difference.
namespace Skills
{
	struct Summary
	{
		const char* name;
		float       progress;  // 0..1, the share of the next point
		int         points;    // unspent
		int         earned;
		int         total;
	};

	void Init();            // after the game data is loaded: starts listening
	void OnGameLoaded();    // a save was loaded or a new game started
	void Tick();            // every few seconds on the game thread (days, hunting experience, the cold)

	Summary Cooking();
	Summary Hunting();
	Summary Wayfarer();

	bool AtCookingStation();
	bool IsHotMeal(const RE::TESForm* a_food);

	// experience from other parts of the plugin (the synergies); the tree's experience multiplier applies
	void AddWayfarerXp(float a_xp, const std::string& a_why);
}
