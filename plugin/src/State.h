/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// What the plugin has to remember between game sessions that the skill trees' globals do not hold. It lives in the SKSE co-save of each save game.
namespace State
{
	struct Data
	{
		int   lastDay = -1;        // the last "days passed" the Wayfarer tree counted
		float lastCleans = -1.0f;  // the last value of Simple Hunting Overhaul's hunting experience the Art of the Hunt tree counted
	};

	Data& D();
	void  Register();  // hooks the co-save callbacks
}

// A short log of what the skill trees earned lately, for the Overview page ("Culinary Arts +4.2 (Beef Stew)").
namespace Journal
{
	void                     Add(const std::string& a_line);
	std::vector<std::string> Recent();
}
