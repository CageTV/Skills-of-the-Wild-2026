/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Game.h"
#include "Settings.h"
#include "Skills.h"
#include "State.h"
#include "Synergy.h"

namespace Synergy
{
	namespace
	{
		using Game::F;
		using Game::Val;
		using Settings::On;
		using Settings::V;

		// Frostfall's exposure meter runs from 0 (warm) to 120; Last Seed's hunger from 0 (full) to 120. From this exposure on the character counts as cold.
		constexpr float kColdFrom = 40.0f;
		// Cold-hour experience is paid out in lumps so the journal and the progress bar do not move every few seconds.
		constexpr float kColdLump = 0.25f;

		float coldCarry = 0.0f;
	}

	bool LastSeedActive() { return Game::LastSeedRunning(); }
	bool FrostfallActive() { return Game::FrostfallRunning(); }
	bool HuntingActive() { return F().shoXp != nullptr; }

	void OnMealEaten(RE::TESForm* a_food)
	{
		const auto& f = F();
		const int   healthy = Game::Rank(f.cookExotic);  // Healthy Diet

		// Last Seed: Healthy Diet makes a meal fill you more. Last Seed lowers the hunger with its own script a moment after the meal, so the
		// plugin measures how much it took off and adds a share of that.
		if (On(V().synMealHunger) && healthy > 0 && Game::LastSeedRunning()) {
			const float before = Val(f.lsHunger);
			Game::Later(3.0f, [before, healthy]() {
				const auto& g = F();
				const float after = Val(g.lsHunger);
				const float relief = before - after;
				if (relief > 0.5f) {
					const float extra = relief * (V().synMealHungerPct / 100.0f) * static_cast<float>(healthy);
					Game::Set(g.lsHunger, std::max(0.0f, after - extra));
					Journal::Add(std::format("Last Seed: the meal filled you {:.0f} points more", extra));
				}
			});
		}

		// Frostfall: a hot dish takes some of the cold away.
		if (On(V().synWarmMeal) && Game::FrostfallRunning() && Skills::IsHotMeal(a_food)) {
			const float points = V().synWarmMealPts + 3.0f * static_cast<float>(healthy);
			Game::Later(1.0f, [points]() {
				const auto& g = F();
				const float now = Val(g.ffExposure);
				if (now > 0.0f && points > 0.0f) {
					Game::Set(g.ffExposure, std::max(0.0f, now - points));
					Journal::Add(std::format("Frostfall: the hot meal took {:.0f} points of cold away", std::min(now, points)));
				}
			});
		}
	}

	void OnHuntingPointEarned()
	{
		const auto& f = F();
		if (!On(V().synHuntShare) || !f.shoXp || V().synHuntSharePts <= 0.0f) {
			return;
		}
		const float before = f.shoXp->value;
		const float after = std::min(100.0f, before + V().synHuntSharePts);
		if (after <= before) {
			return;
		}
		Game::Set(f.shoXp, after);
		// the Art of the Hunt tree must not count its own gift as skinnings
		auto& d = State::D();
		if (d.lastCleans >= 0.0f) {
			d.lastCleans += after - before;
		}
		Journal::Add(std::format("Simple Hunting Overhaul: hunting experience {:.0f} -> {:.0f}", before, after));
	}

	void Tick(float a_gameHours)
	{
		if (a_gameHours <= 0.0f || !On(V().synColdXp) || !Game::FrostfallRunning()) {
			return;
		}
		auto* player = Game::Player();
		auto* cell = player ? player->GetParentCell() : nullptr;
		if (!cell || cell->IsInteriorCell() || player->IsInCombat()) {
			return;  // only a character that is outdoors and not fighting learns from the cold
		}
		if (Val(F().ffExposure) < kColdFrom) {
			return;
		}
		coldCarry += std::min(a_gameHours, 1.0f) * V().synColdXpHour;
		if (coldCarry >= kColdLump) {
			Skills::AddWayfarerXp(coldCarry, "the cold");
			coldCarry = 0.0f;
		}
	}
}
