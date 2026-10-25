/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// How Skills of the Wild 2026 works together with the other 2026 survival mods when they are installed: Last Seed (hunger), Frostfall (cold) and
// Simple Hunting Overhaul (skinning). Each link switches itself off when its partner is missing and can be turned off on the Synergies page.
namespace Synergy
{
	void OnMealEaten(RE::TESForm* a_food);   // the player ate or drank something
	void OnHuntingPointEarned();             // an Art of the Hunt point was earned
	void Tick(float a_gameHours);            // game hours since the last tick

	// What the menu shows: which partners are active.
	bool LastSeedActive();
	bool FrostfallActive();
	bool HuntingActive();
}
