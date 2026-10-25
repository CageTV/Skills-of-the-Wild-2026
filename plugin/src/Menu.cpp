/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Game.h"
#include "Hud.h"
#include "Menu.h"
#include "Settings.h"
#include "Skills.h"
#include "State.h"
#include "Synergy.h"

#include "SKSEMenuFramework.h"

namespace Menu
{
	namespace
	{
		using namespace ImGuiMCP;
		using Settings::Def;
		using Settings::Kind;

		bool registered = false;

		void Tip(const Def& a_d)
		{
			if (a_d.tip && a_d.tip[0] && IsItemHovered()) {
				BeginTooltip();
				PushTextWrapPos(420.0f);
				TextUnformatted(a_d.tip);
				PopTextWrapPos();
				EndTooltip();
			}
		}

		const char* NeedName(const char* a_plugin)
		{
			const std::string_view p = a_plugin ? a_plugin : "";
			if (p == "Frostfall.esp") return "Frostfall";
			if (p == "LastSeed.esp") return "Last Seed";
			if (p == "Simple Hunting Overhaul.esp") return "Simple Hunting Overhaul";
			return a_plugin ? a_plugin : "";
		}

		void DrawDef(const Def& a_d, int a_id)
		{
			if (Settings::IsHeader(a_d)) {
				Spacing();
				SeparatorText(a_d.label);
				return;
			}
			if (Settings::IsColumn(a_d)) {
				Spacing();
				Separator();
				return;
			}
			PushID(a_id);
			const bool available = Settings::Available(a_d);
			if (!available) {
				BeginDisabled();
			}
			bool changed = false;
			switch (a_d.kind) {
			case Kind::Toggle:
				{
					bool on = Settings::Get(a_d) > 0.5f;
					if (Checkbox(a_d.label, &on)) {
						Settings::Set(a_d, on ? 1.0f : 0.0f);
						changed = true;
					}
					Tip(a_d);
				}
				break;
			case Kind::Slider:
				{
					const float shown = a_d.shown > 0.0f ? a_d.shown : 1.0f;
					float       v = Settings::Get(a_d) * shown;
					if (SliderFloat(a_d.label, &v, a_d.min * shown, a_d.max * shown, a_d.fmt)) {
						Settings::Set(a_d, v / shown);
						changed = true;
					}
					Tip(a_d);
				}
				break;
			case Kind::Combo:
				{
					int index = std::clamp(static_cast<int>(Settings::Get(a_d) + 0.5f), 0, std::max(a_d.optionCount - 1, 0));
					if (Combo(a_d.label, &index, a_d.options, a_d.optionCount)) {
						Settings::Set(a_d, static_cast<float>(index));
						changed = true;
					}
					Tip(a_d);
				}
				break;
			}
			if (changed && a_d.link != Settings::Link::None) {
				Hud::Refresh();
			} else if (changed && std::string_view(a_d.id) == "compass_mode") {
				Hud::Refresh();
			}
			if (!available) {
				EndDisabled();
				if (a_d.needs && IsItemHovered()) {
					BeginTooltip();
					Text("%s is not installed.", NeedName(a_d.needs));
					EndTooltip();
				}
			}
			PopID();
		}

		void DrawPage(int a_page)
		{
			if (!Game::Ready()) {
				TextColored(ImVec4(1.0f, 0.45f, 0.4f, 1.0f), "Skills of the Wild is not loaded, so these settings do nothing.");
				return;
			}
			const auto& page = Settings::kPages[a_page];
			for (int i = 0; i < page.count; ++i) {
				DrawDef(page.defs[i], i);
			}
		}

		void Status(const char* a_label, bool a_installed, const char* a_note = "")
		{
			if (a_installed) {
				TextColored(ImVec4(0.65f, 0.9f, 0.5f, 1.0f), "%s: installed%s", a_label, a_note);
			} else {
				TextDisabled("%s: not installed", a_label);
			}
		}

		void Tree(const Skills::Summary& a_s)
		{
			Text("%s", a_s.name);
			SameLine(220.0f);
			TextDisabled("%d point(s) to spend, %d of %d earned", a_s.points, a_s.earned, a_s.total);
			ProgressBar(std::clamp(a_s.progress, 0.0f, 1.0f), ImVec2(-1.0f, 0.0f));
		}

		void __stdcall RenderOverview()
		{
			if (!Game::Ready()) {
				TextColored(ImVec4(1.0f, 0.45f, 0.4f, 1.0f), "Skills of the Wild 2026 is not active.");
				Spacing();
				TextWrapped(Game::Loaded("SkillsOfTheWild.esp")
					? "SkillsOfTheWild.esp is loaded but some of its records are missing, so the skill trees keep running on the mod's own scripts."
					: "SkillsOfTheWild.esp is not in the load order.");
				return;
			}
			TextColored(ImVec4(0.65f, 0.9f, 0.5f, 1.0f), "Skills of the Wild 2026 is active.");
			Spacing();
			TextWrapped("The skill trees now run on the game's own events instead of the scripts' sit-down checks: experience arrives when you earn it, "
						"at any cooking station, and works together with the other survival mods below when they are installed.");

			Spacing();
			SeparatorText("Skill trees");
			Tree(Skills::Cooking());
			Tree(Skills::Hunting());
			Tree(Skills::Wayfarer());
			Spacing();
			TextDisabled("Beast Handler is still run by the mod's own scripts.");

			Spacing();
			SeparatorText("Working together with");
			Status("Campfire", Game::Loaded("Campfire.esm"), Game::ProbeDll("Campfire.dll") ? " (Campfire 2026)" : "");
			Status("Simple Hunting Overhaul", Game::Loaded("Simple Hunting Overhaul.esp"), Game::ProbeDll("SimpleHunting.dll") ? " (Simple Hunting Overhaul 2026)" : "");
			Status("Frostfall", Game::Loaded("Frostfall.esp"), Game::ProbeDll("Frostfall.dll") ? " (Frostfall 2026)" : "");
			Status("Last Seed", Game::Loaded("LastSeed.esp"), Game::ProbeDll("LastSeed.dll") ? " (Last Seed 2026)" : "");
			Status("Gourmet", Game::Loaded("Gourmet.esp"));
			Status("Dynamic Activation Key", Game::Loaded("Dynamic Activation Key.esp"));
			Status("Compass Navigation Overhaul", Game::ProbeDll("CompassNavigationOverhaul.dll"));
			Spacing();
			TextWrapped("%s", Hud::ModeText());
			if (Synergy::LastSeedActive()) {
				TextColored(ImVec4(0.65f, 0.9f, 0.5f, 1.0f), "Last Seed is running: cooked meals fill you more.");
			}
			if (Synergy::FrostfallActive()) {
				TextColored(ImVec4(0.65f, 0.9f, 0.5f, 1.0f), "Frostfall is running: hot meals warm you, the cold teaches.");
			}

			Spacing();
			SeparatorText("Lately");
			const auto lines = Journal::Recent();
			if (lines.empty()) {
				TextDisabled("Nothing yet. Cook a meal, kill or skin an animal, discover a place...");
			}
			for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
				Text("%s", it->c_str());
			}
			Spacing();
			TextDisabled("Skills of the Wild by JaySerpa. The plugin and these pages by CageTV.");
		}

		void __stdcall RenderSkills() { DrawPage(0); }
		void __stdcall RenderSight() { DrawPage(1); }

		void __stdcall RenderSynergies()
		{
			if (Game::Ready()) {
				TextWrapped("Each link works only when its partner mod is installed and running, and each can be turned off here.");
			}
			DrawPage(2);
		}

		void __stdcall RenderProfile()
		{
			static int   confirmReset = 0;
			static float confirmTimer = 0.0f;
			confirmTimer -= GetIO()->DeltaTime;
			if (confirmTimer <= 0.0f) {
				confirmReset = 0;
			}
			SeparatorText("Settings file");
			TextDisabled("%s", Settings::FilePath());
			TextWrapped("The pages save themselves. The two settings that live in the game's globals (the compass cheat and the default jump height) follow you into "
						"every save once you have changed them.");
			Spacing();
			if (Button("Write the file now")) {
				Settings::SaveNow();
			}
			SameLine();
			if (Button("Read the file now")) {
				Settings::LoadNow();
				Hud::Refresh();
			}
			Spacing();
			SeparatorText("Defaults");
			if (confirmReset == 1) {
				TextWrapped("Put every setting on the pages back to its default?");
				if (Button("Yes, reset")) {
					Settings::ResetDefaults();
					Hud::Refresh();
					confirmReset = 0;
				}
				SameLine();
				if (Button("Cancel")) {
					confirmReset = 0;
				}
			} else if (Button("Reset all settings to default")) {
				confirmReset = 1;
				confirmTimer = 10.0f;
			}
		}

		// Drawn every frame whether or not the menu is open: writes the settings file a moment after the last change.
		void __stdcall Frame()
		{
			Settings::Tick(GetIO()->DeltaTime);
		}
	}

	bool Registered() { return registered; }

	void Register()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			SKSE::log::warn("SKSE Menu Framework is not installed: the Skills of the Wild settings pages are not available");
			return;
		}
		SKSEMenuFramework::SetSection("Skills of the Wild");
		SKSEMenuFramework::AddSectionItem("Overview", RenderOverview);
		SKSEMenuFramework::AddSectionItem("Skills", RenderSkills);
		SKSEMenuFramework::AddSectionItem("Sight and Movement", RenderSight);
		SKSEMenuFramework::AddSectionItem("Synergies", RenderSynergies);
		SKSEMenuFramework::AddSectionItem("Profile", RenderProfile);
		SKSEMenuFramework::AddHudElement(Frame);
		registered = true;
		SKSE::log::info("Registered with SKSE Menu Framework {}", SKSEMenuFramework::GetMenuFrameworkVersion());
	}
}
