/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// The settings of Skills of the Wild 2026. Most are plugin values kept in Data/SKSE/Plugins/SkillsOfTheWild/settings.ini; two of them
// (the compass cheat and the default jump height) are globals of Skills of the Wild itself that used to be reachable only from the console, so the
// pages read and write those globals directly and the ini keeps a copy that is applied again on every load.
namespace Settings
{
	enum class Kind { Toggle, Slider, Combo };
	enum class Link { None, CompassCheat, DefaultJump };

	// Every value is a float so one table can describe them all (a toggle is 0 or 1, a combo is the index of its entry).
	struct Values
	{
		// general
		float notify = 1;             // show the skill tree messages at all
		float compassMode = 0;        // 0 = automatic, 1 = Skills of the Wild's rule (no compass or sneak eye until the Wayfarer perks), 2 = leave the HUD alone, 3 = always show
		float compassCheat = 0;       // linked: CompassCheat
		float defaultJump = 76;       // linked: DefaultJumpHeightValue
		// experience
		float xpCooking = 1;          // multiplier of the experience the Culinary Arts tree earns
		float xpHunting = 1;
		float xpWayfarer = 1;
		float cookAnyStation = 1;     // any furniture with a cooking keyword counts as a cooking station, not only the vanilla pot and the Hearthfires oven
		float cookFoodOnly = 1;       // only food (not potions) added while cooking earns experience
		float huntKillXp = 0.2f;      // experience per animal or creature killed
		float huntSkinXp = 1.1f;      // experience per skinning
		float huntSkinBeyondCap = 1;  // keep counting skinnings after Simple Hunting Overhaul's own experience has reached 100
		float wayDayXp = 0.9f;        // per day that passes
		float wayLocXp = 0.6f;        // per location discovered
		// synergies with the other 2026 survival mods
		float synMealHunger = 1;      // Last Seed: a meal you cooked fills you more, by the Culinary perks
		float synMealHungerPct = 10;  // percent of the meal's own relief per Healthy Diet rank
		float synWarmMeal = 1;        // Frostfall: a hot meal warms you
		float synWarmMealPts = 6;     // exposure points a hot meal takes away (plus 3 per Healthy Diet rank)
		float synColdXp = 1;          // Frostfall: hours spent cold outdoors earn Wayfarer experience
		float synColdXpHour = 0.05f;  // Wayfarer experience per game hour in the cold
		float synHuntShare = 1;       // Simple Hunting Overhaul: every Art of the Hunt point earned raises your hunting experience
		float synHuntSharePts = 3;    // hunting experience points per Art of the Hunt point
	};

	struct Def
	{
		const char*       id;      // key in settings.ini
		const char*       label;
		const char*       tip;
		Kind              kind;
		float Values::*   member;
		float             def, min, max, step;
		const char*       fmt;     // slider display format (printf, one float)
		float             shown;   // slider display factor (100 shows 0.2 as 20)
		const char* const* options;
		int               optionCount;
		Link              link;
		const char*       needs;   // plugin that must be loaded for the setting to do anything; nullptr = none
	};

	struct Page
	{
		const char* title;
		const Def*  defs;
		int         count;       // headers (id null, label set) and column breaks (id null, label empty) are defs too
	};

	inline constexpr int kPageTotal = 3;
	extern const Page kPages[kPageTotal];

	bool IsHeader(const Def& a_d);
	bool IsColumn(const Def& a_d);

	const Values& V();               // the current values
	float         Get(const Def& a_d);
	void          Set(const Def& a_d, float a_value);
	bool          Available(const Def& a_d);

	inline bool On(float a_v) { return a_v > 0.5f; }

	// 1 = Skills of the Wild's rule, 2 = leave the HUD alone, 3 = always show: what compassMode means after the automatic choice (a compass overhaul mod
	// that draws its own compass gets 2, everything else 1).
	int EffectiveCompassMode();

	void OnDataLoaded();             // reads settings.ini
	void OnGameLoaded();             // puts the linked globals right
	void SaveNow();
	void LoadNow();
	void ResetDefaults();
	void Tick(float a_dt);           // writes the file a moment after the last change
	const char* FilePath();
}
