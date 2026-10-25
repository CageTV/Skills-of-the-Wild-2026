/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// The compass, the sneak eye and the jump height, which Skills of the Wild's Wayfarer tree takes away until the perks that give them back. The mod's
// script did this once on every load with calls into the HUD; the plugin does it from the game's HUD itself, keeps it that way when the HUD reloads, and
// can leave a mod that draws its own compass alone.
namespace Hud
{
	void OnGameLoaded();  // a save was loaded or a new game started: put everything right a little later
	void Tick();          // every few seconds on the game thread
	void Refresh();       // a setting changed
	const char* ModeText();  // what is being done now, for the menu
}
