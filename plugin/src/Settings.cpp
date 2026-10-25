/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Game.h"
#include "Ids.h"
#include "Settings.h"

namespace Settings
{
	namespace
	{
		constexpr const char* kFile = "Data/SKSE/Plugins/SkillsOfTheWild/settings.ini";
		constexpr float       kWriteDelay = 0.6f;

#define HEADER(l) { nullptr, l, "", Kind::Slider, nullptr, 0, 0, 0, 0, "", 1, nullptr, 0, Link::None, nullptr }
#define COLUMN { nullptr, "", "", Kind::Slider, nullptr, 0, 0, 0, 0, "", 1, nullptr, 0, Link::None, nullptr }
#define TOG(id, l, tip, mem, d, needs) { id, l, tip, Kind::Toggle, &Values::mem, d, 0, 1, 1, "", 1, nullptr, 0, Link::None, needs }
#define SLD(id, l, tip, mem, d, mn, mx, st, f, sh, needs) { id, l, tip, Kind::Slider, &Values::mem, d, mn, mx, st, f, sh, nullptr, 0, Link::None, needs }
#define CMB(id, l, tip, mem, d, opts) { id, l, tip, Kind::Combo, &Values::mem, d, 0, 0, 1, "", 1, opts, static_cast<int>(std::size(opts)), Link::None, nullptr }
#define LTOG(id, l, tip, mem, d, lk) { id, l, tip, Kind::Toggle, &Values::mem, d, 0, 1, 1, "", 1, nullptr, 0, lk, nullptr }
#define LSLD(id, l, tip, mem, d, mn, mx, st, f, lk) { id, l, tip, Kind::Slider, &Values::mem, d, mn, mx, st, f, 1, nullptr, 0, lk, nullptr }

		constexpr const char* kCompass[] = { "Automatic", "Skills of the Wild's rule", "Leave the HUD alone", "Always show" };

		constexpr const char* kFf = ids::kFrostfall;
		constexpr const char* kLs = ids::kLastSeed;
		constexpr const char* kSho = ids::kSho;

		constexpr Def kSkills[] = {
			HEADER("Experience"),
			SLD("xp_cooking", "Culinary Arts", "Multiplies the experience the Culinary Arts tree earns (cooking and reading about cooking).", xpCooking, 1.0f, 0.1f, 5.0f, 0.05f, "x %.2f", 1, nullptr),
			SLD("xp_hunting", "Art of the Hunt", "Multiplies the experience the Art of the Hunt tree earns (kills, skinnings, hunting books).", xpHunting, 1.0f, 0.1f, 5.0f, 0.05f, "x %.2f", 1, nullptr),
			SLD("xp_wayfarer", "Wayfarer", "Multiplies the experience the Wayfarer tree earns (days, places, books, the cold).", xpWayfarer, 1.0f, 0.1f, 5.0f, 0.05f, "x %.2f", 1, nullptr),
			HEADER("Messages"),
			TOG("notify", "Skill Tree Messages", "Shows \"New perk available\", \"You managed to cook a second meal\" and the other messages of the skill trees.", notify, 1, nullptr),
			COLUMN,
			HEADER("Culinary Arts"),
			TOG("cook_any_station", "Any Cooking Station", "Counts any furniture with a cooking keyword as a place to cook (campfire pots, ovens and kitchens of other mods), not only the vanilla cooking pot and the Hearthfires oven.", cookAnyStation, 1, nullptr),
			TOG("cook_food_only", "Only Food Earns Experience", "Only food counts as a cooked meal; a potion or an ingredient added at the same time does not.", cookFoodOnly, 1, nullptr),
			HEADER("Art of the Hunt"),
			SLD("hunt_kill_xp", "Experience per Kill", "Experience for every animal or creature you kill.", huntKillXp, 0.2f, 0.0f, 3.0f, 0.05f, "%.2f", 1, nullptr),
			SLD("hunt_skin_xp", "Experience per Skinning", "Experience for every carcass you skin with Simple Hunting Overhaul.", huntSkinXp, 1.1f, 0.0f, 5.0f, 0.05f, "%.2f", 1, kSho),
			TOG("hunt_skin_beyond_cap", "Keep Counting Past 100", "Simple Hunting Overhaul stops counting your hunting experience at 100, so the old tree stopped growing there. On: every skinning still earns experience.", huntSkinBeyondCap, 1, kSho),
			HEADER("Wayfarer"),
			SLD("way_day_xp", "Experience per Day", "Experience for every day that passes.", wayDayXp, 0.9f, 0.0f, 5.0f, 0.05f, "%.2f", 1, nullptr),
			SLD("way_loc_xp", "Experience per Place", "Experience for every location you discover.", wayLocXp, 0.6f, 0.0f, 5.0f, 0.05f, "%.2f", 1, nullptr),
		};

		constexpr Def kSight[] = {
			HEADER("Compass and Sneak Eye"),
			CMB("compass_mode", "Compass and Sneak Eye", "Skills of the Wild's rule hides the compass and the sneak eye until the Wayfarer perks that unlock them. Leave the HUD alone is for a mod that draws its own compass and replaces the \"Re-enable Compass\" patch. Automatic picks that when Compass Navigation Overhaul is installed.", compassMode, 0, kCompass),
			LTOG("compass_cheat", "Always Show Them (cheat)", "Shows the compass and the sneak eye from the start, as the console command \"Set CompassCheat to 1\" did.", compassCheat, 0, Link::CompassCheat),
			COLUMN,
			HEADER("Movement"),
			LSLD("default_jump", "Default Jump Height", "The jump height of your character without a mount. 76 in vanilla; set it to what your other mods use, or the Wayfarer jump perks put it back to the wrong value.", defaultJump, 76, 20, 300, 1, "%.0f", Link::DefaultJump),
		};

		constexpr Def kSynergy[] = {
			HEADER("Last Seed"),
			TOG("syn_meal_hunger", "Cooked Meals Fill You More", "After you eat, the Culinary Arts perk Healthy Diet makes the meal fill you more: its own relief of hunger plus a share per perk rank.", synMealHunger, 1, kLs),
			SLD("syn_meal_hunger_pct", "Extra Relief per Rank", "Share of the meal's own relief of hunger added for every rank of Healthy Diet.", synMealHungerPct, 10, 0, 50, 1, "%.0f %%", 1, kLs),
			HEADER("Frostfall"),
			TOG("syn_warm_meal", "Hot Meals Warm You", "Eating a hot dish (stew, soup, chowder, broth, pie) takes some of the cold out of you.", synWarmMeal, 1, kFf),
			SLD("syn_warm_meal_pts", "Warmth of a Hot Meal", "Exposure points a hot dish takes away (plus 3 for every rank of Healthy Diet).", synWarmMealPts, 6, 0, 40, 1, "%.0f", 1, kFf),
			TOG("syn_cold_xp", "The Cold Teaches", "Hours you spend outdoors while Frostfall has you cold earn Wayfarer experience.", synColdXp, 1, kFf),
			SLD("syn_cold_xp_hour", "Experience per Cold Hour", "Wayfarer experience for every game hour spent cold outdoors.", synColdXpHour, 0.05f, 0.0f, 1.0f, 0.01f, "%.2f", 1, kFf),
			COLUMN,
			HEADER("Simple Hunting Overhaul"),
			TOG("syn_hunt_share", "Skill Points Teach the Knife", "Every Art of the Hunt point you earn also raises your hunting experience, so skinning gets faster.", synHuntShare, 1, kSho),
			SLD("syn_hunt_share_pts", "Hunting Experience per Point", "Hunting experience (0 to 100) added for every Art of the Hunt point earned.", synHuntSharePts, 3, 0, 20, 1, "%.0f", 1, kSho),
		};
	}

	const Page kPages[kPageTotal] = {
		{ "Skills", kSkills, static_cast<int>(std::size(kSkills)) },
		{ "Sight and Movement", kSight, static_cast<int>(std::size(kSight)) },
		{ "Synergies", kSynergy, static_cast<int>(std::size(kSynergy)) },
	};

	bool IsHeader(const Def& a_d) { return a_d.id == nullptr && a_d.label && a_d.label[0]; }
	bool IsColumn(const Def& a_d) { return a_d.id == nullptr && !(a_d.label && a_d.label[0]); }

	namespace
	{
		Values            values;
		std::map<std::string, float> fileKeys;  // what settings.ini held when it was read, to know which linked values the player has chosen
		std::mutex        mu;
		bool              dirty = false;
		float             timer = -1.0f;

		float Clamp(const Def& a_d, float a_v)
		{
			if (a_d.kind == Kind::Slider) {
				if (a_d.step > 0.0f) {
					a_v = a_d.min + std::round((a_v - a_d.min) / a_d.step) * a_d.step;
				}
				return std::clamp(a_v, a_d.min, a_d.max);
			}
			if (a_d.kind == Kind::Combo) {
				return std::clamp(std::round(a_v), 0.0f, static_cast<float>(std::max(a_d.optionCount - 1, 0)));
			}
			return a_v > 0.5f ? 1.0f : 0.0f;
		}

		RE::TESGlobal* Linked(const Def& a_d)
		{
			switch (a_d.link) {
			case Link::CompassCheat:
				return Game::F().compassCheat;
			case Link::DefaultJump:
				return Game::F().defaultJump;
			default:
				return nullptr;
			}
		}

		template <class Fn>
		void ForEach(Fn&& a_fn)
		{
			for (const auto& page : kPages) {
				for (int i = 0; i < page.count; ++i) {
					if (page.defs[i].id) {
						a_fn(page.defs[i]);
					}
				}
			}
		}

		void WriteFile()
		{
			std::error_code ec;
			std::filesystem::create_directories(std::filesystem::path(kFile).parent_path(), ec);
			std::ofstream out(kFile, std::ios::trunc);
			if (!out) {
				SKSE::log::warn("Cannot write {}", kFile);
				return;
			}
			out << "# Skills of the Wild 2026 settings. Edited by the SKSE Menu Framework pages. Delete a line to put that setting back to its default.\n";
			int n = 0;
			ForEach([&](const Def& d) {
				// A linked value is only written once it differs from the default or the player has chosen it, so a save keeps its own value until then.
				const float v = values.*(d.member);
				if (d.link != Link::None && !fileKeys.contains(d.id) && std::abs(v - d.def) < 1e-6f) {
					return;
				}
				char buf[48];
				std::snprintf(buf, sizeof(buf), "%g", static_cast<double>(v));
				out << d.id << "=" << buf << "\n";
				++n;
			});
			SKSE::log::info("Wrote {} ({} settings)", kFile, n);
		}

		void Touch()
		{
			dirty = true;
			timer = kWriteDelay;
		}

		void ReadFile()
		{
			fileKeys.clear();
			std::ifstream in(kFile);
			std::string   line;
			while (std::getline(in, line)) {
				if (line.empty() || line[0] == '#' || line[0] == ';') {
					continue;
				}
				const auto eq = line.find('=');
				if (eq == std::string::npos) {
					continue;
				}
				std::string key = line.substr(0, eq);
				std::string val = line.substr(eq + 1);
				while (!val.empty() && (val.back() == '\r' || val.back() == ' ')) {
					val.pop_back();
				}
				fileKeys[key] = std::strtof(val.c_str(), nullptr);
			}
		}
	}

	const Values& V() { return values; }

	float Get(const Def& a_d)
	{
		std::lock_guard lock(mu);
		if (auto* g = Linked(a_d)) {
			return g->value;
		}
		return values.*(a_d.member);
	}

	void Set(const Def& a_d, float a_value)
	{
		a_value = Clamp(a_d, a_value);
		std::lock_guard lock(mu);
		values.*(a_d.member) = a_value;
		if (auto* g = Linked(a_d)) {
			g->value = a_value;
		}
		if (a_d.id) {
			fileKeys[a_d.id] = a_value;
		}
		Touch();
	}

	bool Available(const Def& a_d)
	{
		if (a_d.link != Link::None && !Linked(a_d)) {
			return false;
		}
		return !a_d.needs || Game::Loaded(a_d.needs);
	}

	int EffectiveCompassMode()
	{
		const int m = static_cast<int>(values.compassMode + 0.5f);
		if (m >= 1 && m <= 3) {
			return m;
		}
		return Game::ProbeDll("CompassNavigationOverhaul.dll") ? 2 : 1;
	}

	void OnDataLoaded()
	{
		std::lock_guard lock(mu);
		values = {};
		ReadFile();
		int n = 0;
		ForEach([&](const Def& d) {
			const auto it = fileKeys.find(d.id);
			if (it != fileKeys.end()) {
				values.*(d.member) = Clamp(d, it->second);
				++n;
			}
		});
		SKSE::log::info("Settings: {} value(s) read from {}", n, kFile);
	}

	void OnGameLoaded()
	{
		std::lock_guard lock(mu);
		int applied = 0, adopted = 0;
		ForEach([&](const Def& d) {
			auto* g = Linked(d);
			if (!g) {
				return;
			}
			const auto it = fileKeys.find(d.id);
			if (it != fileKeys.end()) {
				g->value = Clamp(d, it->second);  // the player's choice follows them into every save
				++applied;
			} else {
				values.*(d.member) = g->value;     // not chosen: this save's own value stays
				++adopted;
			}
		});
		SKSE::log::info("Game loaded: {} linked value(s) applied from the file, {} taken from the save", applied, adopted);
	}

	void SaveNow()
	{
		std::lock_guard lock(mu);
		ForEach([&](const Def& d) {
			if (auto* g = Linked(d)) {
				values.*(d.member) = g->value;
			}
			fileKeys[d.id] = values.*(d.member);
		});
		WriteFile();
		dirty = false;
		timer = -1.0f;
	}

	void LoadNow()
	{
		{
			std::lock_guard lock(mu);
			ReadFile();
			ForEach([&](const Def& d) {
				const auto it = fileKeys.find(d.id);
				if (it != fileKeys.end()) {
					values.*(d.member) = Clamp(d, it->second);
					if (auto* g = Linked(d)) {
						g->value = values.*(d.member);
					}
				}
			});
		}
	}

	void ResetDefaults()
	{
		std::lock_guard lock(mu);
		fileKeys.clear();
		ForEach([&](const Def& d) {
			values.*(d.member) = d.def;
			if (auto* g = Linked(d)) {
				g->value = d.def;
			}
		});
		Touch();
	}

	void Tick(float a_dt)
	{
		if (timer < 0.0f) {
			return;
		}
		timer -= a_dt;
		if (timer <= 0.0f) {
			std::lock_guard lock(mu);
			timer = -1.0f;
			if (dirty) {
				WriteFile();
				dirty = false;
			}
		}
	}

	const char* FilePath() { return kFile; }
}
