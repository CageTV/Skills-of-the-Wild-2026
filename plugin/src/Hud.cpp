/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Game.h"
#include "Hud.h"
#include "Settings.h"

namespace Hud
{
	namespace
	{
		using Game::F;
		using Game::Val;

		// The compass and the shout meter share one clip, which is how the mod's script has always hidden the compass.
		constexpr const char* kCompassAlpha = "_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha";
		constexpr const char* kEyeAlpha = "_root.HUDMovieBaseInstance.StealthMeterInstance._alpha";

		// what was last done: -1 nothing yet, 0 hidden, 1 shown
		int lastCompass = -1;
		int lastEye = -1;
		std::atomic<int> settleTicks{ 0 };  // ticks left in which every state is applied again (the HUD finishes loading a moment after the save)

		void SetAlpha(const char* a_path, double a_value)
		{
			auto* ui = RE::UI::GetSingleton();
			if (!ui) {
				return;
			}
			if (auto movie = ui->GetMovieView(RE::HUDMenu::MENU_NAME)) {
				movie->SetVariableDouble(a_path, a_value);
			}
		}

		void ApplyJump()
		{
			auto* player = Game::Player();
			auto* gmst = RE::GameSettingCollection::GetSingleton();
			auto* setting = gmst ? gmst->GetSetting("fJumpHeightMin") : nullptr;
			if (!player || !setting || !F().defaultJump || player->IsOnMount()) {
				return;
			}
			const float v = Val(F().defaultJump, 76.0f);
			if (v >= 20.0f && std::abs(setting->data.f - v) > 0.01f) {
				setting->data.f = v;  // the mounted jump perks change this setting; on foot it is the player's default again
			}
		}

		void Apply(bool a_force)
		{
			const auto& f = F();
			const int   mode = Settings::EffectiveCompassMode();
			auto*       player = Game::Player();
			if (!player) {
				return;
			}
			const bool cheat = Val(f.compassCheat) > 0.5f;
			const bool wantCompass = mode == 3 || cheat || Val(f.waySpatial) > 0.5f;
			const bool wantEye = mode == 3 || cheat || Val(f.waySixth) > 0.5f;

			if (mode == 2) {
				// Another mod owns the HUD: give back whatever Skills of the Wild's rule took, once, and stay out of it.
				if (lastCompass == 0) {
					SetAlpha(kCompassAlpha, 100.0);
				}
				if (lastEye == 0) {
					SetAlpha(kEyeAlpha, 100.0);
				}
				lastCompass = lastEye = -1;
				if (f.spellCrossHair && player->HasSpell(f.spellCrossHair)) {
					player->RemoveSpell(f.spellCrossHair);
				}
				return;
			}

			// hidden states are applied again every tick (cheap, and the HUD resets them); shown states only when they change or after a load
			const int c = wantCompass ? 1 : 0;
			const int e = wantEye ? 1 : 0;
			if (c == 0 || a_force || lastCompass != c) {
				SetAlpha(kCompassAlpha, c ? 100.0 : 0.0);
			}
			if (e == 0 || a_force || lastEye != e) {
				SetAlpha(kEyeAlpha, e ? 100.0 : 0.0);
			}
			lastCompass = c;
			lastEye = e;

			// Without the sneak eye the crosshair stays visible while sneaking, through the mod's own spell.
			if (f.spellCrossHair) {
				const bool has = player->HasSpell(f.spellCrossHair);
				if (!wantEye && !has) {
					player->AddSpell(f.spellCrossHair);
				} else if (wantEye && has) {
					player->RemoveSpell(f.spellCrossHair);
				}
			}
		}
	}

	void OnGameLoaded()
	{
		lastCompass = lastEye = -1;
		settleTicks = 4;
		Game::Later(2.5f, []() {
			ApplyJump();
			Apply(true);
		});
	}

	void Tick()
	{
		if (!Game::Ready() || Game::Paused()) {
			return;
		}
		const bool force = settleTicks.load() > 0;
		if (force) {
			--settleTicks;
		}
		Apply(force);
	}

	void Refresh()
	{
		lastCompass = lastEye = -1;
		SKSE::GetTaskInterface()->AddTask([]() {
			ApplyJump();
			Apply(true);
		});
	}

	const char* ModeText()
	{
		switch (Settings::EffectiveCompassMode()) {
		case 2:
			return "Another mod owns the HUD: the compass and sneak eye are left alone.";
		case 3:
			return "The compass and the sneak eye are always shown.";
		default:
			return "Skills of the Wild's rule: no compass or sneak eye until the Wayfarer perks that unlock them.";
		}
	}
}
