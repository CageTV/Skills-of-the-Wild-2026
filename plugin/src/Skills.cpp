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

#include <random>

namespace Skills
{
	namespace
	{
		using Game::F;
		using Game::Val;
		using Settings::On;
		using Settings::V;

		bool atStation = false;   // the player is using a cooking station
		bool listening = false;

		std::mt19937 rng{ std::random_device{}() };

		float Roll() { return std::uniform_real_distribution<float>(0.0f, 100.0f)(rng); }

		void Say(const char* a_text)
		{
			if (On(V().notify)) {
				Game::Notify(a_text);
			}
		}

		std::string Name(const RE::TESForm* a_form)
		{
			if (!a_form) {
				return "?";
			}
			const char* n = a_form->GetName();
			return (n && n[0]) ? n : "an unnamed item";
		}

		bool HasKeyword(RE::TESForm* a_form, RE::BGSKeyword* a_kw)
		{
			if (!a_form || !a_kw) {
				return false;
			}
			auto* kf = a_form->As<RE::BGSKeywordForm>();
			return kf && kf->HasKeyword(a_kw);
		}

		std::string Lower(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

		// ---- the three trees' arithmetic (the same as the scripts', on the same globals) ----------------------------------------------------

		// Culinary Arts: progress is stored as a share of what the next point needs; each point makes the next one 9 experience dearer.
		void CookXp(float a_xp, const std::string& a_why)
		{
			const auto& f = F();
			a_xp *= V().xpCooking;
			if (a_xp <= 0.0f || Val(f.cookEarned) >= Val(f.cookTotal)) {
				return;
			}
			const float need = std::max(Val(f.cookXpToNext, 1.0f), 1.0f);
			const float progress = Val(f.cookProgress) * need;
			if (need <= progress + a_xp) {
				Game::Set(f.cookPoints, Val(f.cookPoints) + 1.0f);
				Game::Set(f.cookProgress, 0.0f);
				Game::Set(f.cookXpToNext, need + 9.0f);
				Game::Set(f.cookEarned, Val(f.cookEarned) + 1.0f);
				Say("You're getting the hang of this! New cooking perk available.");
				Journal::Add(std::format("Culinary Arts +{:.2f} ({}): a perk point", a_xp, a_why));
			} else {
				Game::Set(f.cookProgress, (progress + a_xp) / need);
				Journal::Add(std::format("Culinary Arts +{:.2f} ({})", a_xp, a_why));
			}
		}

		// Art of the Hunt: the first point needs 1 experience, every later one 6 more than the one before.
		void HuntXp(float a_xp, const std::string& a_why)
		{
			const auto& f = F();
			a_xp *= V().xpHunting;
			if (a_xp <= 0.0f || Val(f.huntEarned) >= Val(f.huntTotal)) {
				return;
			}
			const float need = 1.0f + 6.0f * Val(f.huntEarned);
			const float progress = Val(f.huntProgress) * need;
			if (need <= progress + a_xp) {
				Game::Set(f.huntPoints, Val(f.huntPoints) + 1.0f);
				Game::Set(f.huntProgress, 0.0f);
				Game::Set(f.huntEarned, Val(f.huntEarned) + 1.0f);
				Say("You reflect on your hunting experience. New perk available.");
				Journal::Add(std::format("Art of the Hunt +{:.2f} ({}): a perk point", a_xp, a_why));
				Synergy::OnHuntingPointEarned();
			} else {
				Game::Set(f.huntProgress, (progress + a_xp) / need);
				Journal::Add(std::format("Art of the Hunt +{:.2f} ({})", a_xp, a_why));
			}
		}

		// Wayfarer: experience is kept as a number against a limit that grows by 5 for every point.
		void WayXp(float a_xp, const std::string& a_why)
		{
			const auto& f = F();
			a_xp *= V().xpWayfarer;
			if (a_xp <= 0.0f || Val(f.wayEarned) >= Val(f.wayMax)) {
				return;
			}
			const float limit = std::max(Val(f.wayLimit, 1.0f), 1.0f);
			const float cur = Val(f.wayXp);
			if (cur + a_xp > limit) {
				Game::Set(f.wayLimit, limit + 5.0f);
				Game::Set(f.wayProgress, 0.0f);
				Game::Set(f.wayXp, 0.0f);
				Game::Set(f.wayPoints, Val(f.wayPoints) + 1.0f);
				Game::Set(f.wayEarned, Val(f.wayEarned) + 1.0f);
				Say("Your wayfaring skills have improved! New perk available.");
				Journal::Add(std::format("Wayfarer +{:.2f} ({}): a perk point", a_xp, a_why));
			} else {
				Game::Set(f.wayXp, cur + a_xp);
				Game::Set(f.wayProgress, (cur + a_xp) / limit);
				Journal::Add(std::format("Wayfarer +{:.2f} ({})", a_xp, a_why));
			}
		}

		// ---- cooking ------------------------------------------------------------------------------------------------------------------------

		bool IsCookingStation(RE::TESObjectREFR* a_furniture)
		{
			if (!a_furniture) {
				return false;
			}
			auto* base = a_furniture->GetBaseObject();
			auto* kf = base ? base->As<RE::BGSKeywordForm>() : nullptr;
			if (!kf) {
				return false;
			}
			const auto& f = F();
			if ((f.kwCookpot && kf->HasKeyword(f.kwCookpot)) || (f.kwOven && kf->HasKeyword(f.kwOven))) {
				return true;
			}
			if (!On(V().cookAnyStation)) {
				return false;
			}
			for (std::uint32_t i = 0; i < kf->numKeywords; ++i) {
				if (auto* kw = kf->keywords[i]) {
					const char* id = kw->GetFormEditorID();
					if (id && id[0] && Lower(id).find("cook") != std::string::npos) {
						return true;  // CraftingCookpot, UsesCookingSkill, and the cooking keywords of other mods
					}
				}
			}
			return false;
		}

		// The two Papyrus calls the mod's script made, made the same way: Actor.EquipItem(potion, true, true) uses a potion up silently without taking it from the
		// player, ObjectReference.AddItem(food, 1, true) adds a meal without a message.
		template <class... Args>
		void CallPlayer(const char* a_class, const char* a_method, Args... a_args)
		{
			SKSE::GetTaskInterface()->AddTask([=]() mutable {
				auto* player = Game::Player();
				auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
				if (!player || !vm) {
					return;
				}
				const auto handle = vm->GetObjectHandlePolicy()->GetHandleForObject(player->GetFormType(), player);
				RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
				vm->DispatchMethodCall2(handle, a_class, a_method, RE::MakeFunctionArguments(std::move(a_args)...), callback);
			});
		}

		void Drink(RE::AlchemyItem* a_potion)
		{
			if (a_potion) {
				CallPlayer("Actor", "EquipItem", static_cast<RE::TESForm*>(a_potion), true, true);
			}
		}

		std::map<RE::FormID, int> pendingBonus;  // bonus meals handed out whose arrival must not be counted as cooking

		void GivePerk(RE::PlayerCharacter* a_player, RE::BGSPerk* a_perk)
		{
			if (a_player && a_perk && !a_player->HasPerk(a_perk)) {
				a_player->AddPerk(a_perk);
			}
		}

		bool WearingChefHat()
		{
			auto* player = Game::Player();
			const auto* hat = F().chefHat;
			if (!player || !hat) {
				return false;
			}
			using Slot = RE::BGSBipedObjectForm::BipedObjectSlot;
			for (auto slot : { Slot::kHead, Slot::kHair, Slot::kCirclet, Slot::kLongHair }) {
				if (player->GetWornArmor(slot) == hat) {
					return true;
				}
			}
			return false;
		}

		void OnMealCooked(RE::TESBoundObject* a_food)
		{
			const auto& f = F();
			auto*       player = Game::Player();
			if (!player || !a_food) {
				return;
			}
			if (const int r = Game::Rank(f.cookBasic); r == 1) {
				Drink(f.lowHeat1);
			} else if (r == 2) {
				Drink(f.lowHeat2);
			}

			// Frugal Gourmet: a chance of a second meal without extra ingredients.
			const int gourmet = Game::Rank(f.cookGourmet);
			if ((gourmet == 2 && Roll() >= 66.0f) || (gourmet == 1 && Roll() >= 75.0f)) {
				++pendingBonus[a_food->GetFormID()];
				CallPlayer("ObjectReference", "AddItem", static_cast<RE::TESForm*>(a_food), 1, true);
				Say("You have managed to cook a second meal!");
			}

			const float hat = WearingChefHat() ? 0.03f : 0.0f;
			float       mult = 0.09f + hat;
			if (const int s = Game::Rank(f.cookSauce); s == 1) {
				if (f.spellInspired1) {
					player->AddSpell(f.spellInspired1);
				}
				mult = 0.12f + hat;
			} else if (s == 2) {
				if (f.spellInspired2) {
					player->AddSpell(f.spellInspired2);
				}
				mult = 0.14f + hat;
			}

			float xp = static_cast<float>(a_food->GetGoldValue()) * mult;
			if (f.knownRecipes && !f.knownRecipes->HasForm(a_food)) {
				xp *= 3.0f;  // the first time you make a dish is worth three times as much
			}
			if (f.knownRecipes) {
				f.knownRecipes->AddForm(a_food);
			}
			CookXp(xp, Name(a_food));
		}

		// ---- books: one read counts for the first tree whose reading list holds it --------------------------------------------------------

		void OnBookRead(RE::TESForm* a_book)
		{
			const auto& f = F();
			if (!a_book || !f.booksRead || f.booksRead->HasForm(a_book)) {
				return;
			}
			if (f.cookingBooks && f.cookingBooks->HasForm(a_book)) {
				f.booksRead->AddForm(a_book);
				Game::Later(3.0f, [name = Name(a_book)]() { Say("This book has taught me a lot! New cooking perk available."); });
				CookXp(4.0f, "read " + Name(a_book));
			} else if (f.huntingBooks && f.huntingBooks->HasForm(a_book)) {
				f.booksRead->AddForm(a_book);
				HuntXp(2.0f, "read " + Name(a_book));
			} else if (f.wayBooks && f.wayBooks->HasForm(a_book)) {
				f.booksRead->AddForm(a_book);
				WayXp(2.0f, "read " + Name(a_book));
			}
		}

		// ---- hunting ------------------------------------------------------------------------------------------------------------------------

		bool IsGame(const RE::Actor* a_actor)
		{
			auto* race = a_actor ? a_actor->GetRace() : nullptr;
			if (!race) {
				return false;
			}
			return race->HasKeywordString("ActorTypeAnimal") || race->HasKeywordString("ActorTypeCreature") || race->HasKeywordString("ActorTypeDragon");
		}

		bool IsPelt(RE::TESBoundObject* a_item)
		{
			const auto& f = F();
			return a_item && ((f.shoVanilla && f.shoVanilla->HasForm(a_item)) || (f.shoBruma && f.shoBruma->HasForm(a_item)) || (f.shoCustom && f.shoCustom->HasForm(a_item)));
		}

		// ---- the events ---------------------------------------------------------------------------------------------------------------------

		class Sink :
			public RE::BSTEventSink<RE::TESFurnitureEvent>,
			public RE::BSTEventSink<RE::TESContainerChangedEvent>,
			public RE::BSTEventSink<RE::TESEquipEvent>,
			public RE::BSTEventSink<RE::TESDeathEvent>,
			public RE::BSTEventSink<RE::LocationDiscovery::Event>
		{
		public:
			RE::BSEventNotifyControl ProcessEvent(const RE::TESFurnitureEvent* a_event, RE::BSTEventSource<RE::TESFurnitureEvent>*) override
			{
				if (Game::Ready() && a_event && a_event->actor && a_event->actor->IsPlayerRef() && a_event->targetFurniture) {
					const bool enter = a_event->type == RE::TESFurnitureEvent::FurnitureEventType::kEnter;
					auto*      furniture = a_event->targetFurniture.get();
					const bool station = IsCookingStation(furniture);
					if (enter && station) {
						atStation = true;
						GivePerk(Game::Player(), F().perkIsCooking);
					} else if (!enter && station) {
						atStation = false;
						if (auto* p = Game::Player()) {
							if (F().perkIsCooking) {
								p->RemovePerk(F().perkIsCooking);
							}
							if ((F().spellInspired1 && p->HasSpell(F().spellInspired1)) || (F().spellInspired2 && p->HasSpell(F().spellInspired2))) {
								Say("You're feeling inspired!");
							}
						}
					}
				}
				return RE::BSEventNotifyControl::kContinue;
			}

			RE::BSEventNotifyControl ProcessEvent(const RE::TESContainerChangedEvent* a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>*) override
			{
				if (!Game::Ready() || !a_event || a_event->newContainer != 0x14 || a_event->itemCount <= 0) {
					return RE::BSEventNotifyControl::kContinue;
				}
				if (auto it = pendingBonus.find(a_event->baseObj); it != pendingBonus.end() && it->second > 0) {
					if (--it->second <= 0) {
						pendingBonus.erase(it);
					}
					return RE::BSEventNotifyControl::kContinue;  // the second meal of Frugal Gourmet arriving
				}
				auto* item = RE::TESForm::LookupByID<RE::TESBoundObject>(a_event->baseObj);
				if (!item) {
					return RE::BSEventNotifyControl::kContinue;
				}
				const auto& f = F();
				// Cooking: something new appears in the inventory (nothing was moved from a container) while the player sits at a cooking station.
				if (atStation && a_event->oldContainer == 0 && HasKeyword(item, f.kwFood) && !HasKeyword(item, f.kwNoExp) && !(f.ignoreFood && f.ignoreFood->HasForm(item))) {
					OnMealCooked(item);
				}
				// Skinning past Simple Hunting Overhaul's own counter, which stops at 100: a pelt from a dead animal's body.
				else if (On(V().huntSkinBeyondCap) && f.shoXp && Val(f.shoXp) >= 100.0f && a_event->oldContainer != 0 && IsPelt(item)) {
					if (auto* body = RE::TESForm::LookupByID<RE::Actor>(a_event->oldContainer); body && body->IsDead() && !(f.shoBlacklist && f.shoBlacklist->HasForm(body->GetActorBase()))) {
						HuntXp(V().huntSkinXp, "skinned " + Name(body));
					}
				}
				return RE::BSEventNotifyControl::kContinue;
			}

			RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*) override
			{
				if (!Game::Ready() || !a_event || !a_event->equipped || !a_event->actor || !a_event->actor->IsPlayerRef()) {
					return RE::BSEventNotifyControl::kContinue;
				}
				auto* form = RE::TESForm::LookupByID(a_event->baseObject);
				if (!form) {
					return RE::BSEventNotifyControl::kContinue;
				}
				const auto& f = F();
				if (form == f.lowHeat1 || form == f.lowHeat2 || form == f.exotic1 || form == f.exotic2 || form == f.healthy1 || form == f.healthy2) {
					return RE::BSEventNotifyControl::kContinue;  // the plugin's own potions
				}
				if (HasKeyword(form, f.kwIngredient)) {
					if (const int r = Game::Rank(f.cookElaborate); r == 1) {
						Drink(f.exotic1);
					} else if (r == 2) {
						Drink(f.exotic2);
					}
				} else if (HasKeyword(form, f.kwFood)) {
					if (const int r = Game::Rank(f.cookExotic); r == 1) {
						Drink(f.healthy1);
					} else if (r == 2) {
						Drink(f.healthy2);
					}
					Synergy::OnMealEaten(form);
				} else {
					OnBookRead(form);
				}
				return RE::BSEventNotifyControl::kContinue;
			}

			RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*) override
			{
				if (Game::Ready() && a_event && a_event->dead && a_event->actorKiller && a_event->actorKiller->IsPlayerRef() && a_event->actorDying) {
					if (auto* dying = a_event->actorDying->As<RE::Actor>(); dying && !dying->IsPlayerRef() && IsGame(dying)) {
						Game::Set(F().huntKills, Val(F().huntKills) + 1.0f);
						HuntXp(V().huntKillXp, "killed " + Name(dying->GetActorBase()));
					}
				}
				return RE::BSEventNotifyControl::kContinue;
			}

			RE::BSEventNotifyControl ProcessEvent(const RE::LocationDiscovery::Event*, RE::BSTEventSource<RE::LocationDiscovery::Event>*) override
			{
				if (!Game::Ready()) {
					return RE::BSEventNotifyControl::kContinue;
				}
				WayXp(V().wayLocXp, "a new place");
				return RE::BSEventNotifyControl::kContinue;
			}
		};

		Sink sink;
	}

	void Init()
	{
		if (listening) {
			return;
		}
		auto* holder = RE::ScriptEventSourceHolder::GetSingleton();
		if (!holder) {
			return;
		}
		holder->AddEventSink<RE::TESFurnitureEvent>(&sink);
		holder->AddEventSink<RE::TESContainerChangedEvent>(&sink);
		holder->AddEventSink<RE::TESEquipEvent>(&sink);
		holder->AddEventSink<RE::TESDeathEvent>(&sink);
		if (auto* src = RE::LocationDiscovery::GetEventSource()) {
			src->AddEventSink(&sink);
		}
		listening = true;
		SKSE::log::info("Skill trees: listening for cooking, kills, skinning, books and discoveries");
	}

	void OnGameLoaded()
	{
		atStation = false;
		pendingBonus.clear();
		if (auto* p = Game::Player(); p && Game::Ready()) {
			GivePerk(p, F().perkThings);
			GivePerk(p, F().perkMortar);
			if (F().spellSitting && !p->HasSpell(F().spellSitting)) {
				p->AddSpell(F().spellSitting);
			}
		}
	}

	void Tick()
	{
		if (!Game::Ready() || !listening) {
			return;
		}
		auto& d = State::D();

		// Diagnostics: every change of a tree's points to spend is logged, with what has been earned, so a purchase that leaves a tree below
		// zero can be told from the plugin's own awards (each award is logged on its own line).
		{
			static float last[3] = { -1000.0f, -1000.0f, -1000.0f };
			const auto&  g = F();
			const struct { const char* name; const RE::TESGlobal* pts; const RE::TESGlobal* earned; } trees[3] = {
				{ "Culinary Arts", g.cookPoints, g.cookEarned }, { "Art of the Hunt", g.huntPoints, g.huntEarned }, { "Wayfarer", g.wayPoints, g.wayEarned }
			};
			for (int i = 0; i < 3; ++i) {
				const float now = Val(trees[i].pts);
				if (last[i] > -999.0f && now != last[i]) {
					SKSE::log::info("{}: points to spend {:.0f} -> {:.0f} (earned {:.0f})", trees[i].name, last[i], now, Val(trees[i].earned));
				}
				last[i] = now;
			}
		}

		// days: the Wayfarer tree
		if (auto* cal = RE::Calendar::GetSingleton()) {
			const int today = static_cast<int>(std::floor(cal->GetDaysPassed()));
			if (d.lastDay < 0) {
				d.lastDay = today;
			} else if (today > d.lastDay) {
				WayXp(static_cast<float>(today - d.lastDay) * V().wayDayXp, std::format("{} day(s) passed", today - d.lastDay));
				d.lastDay = today;
			}
		}

		// skinnings: Simple Hunting Overhaul raises its hunting experience by one for every pelt, and Skills of the Wild's own harvesting does the same
		if (const auto* sho = F().shoXp) {
			const float now = sho->value;
			if (d.lastCleans < 0.0f) {
				d.lastCleans = now;
			} else if (now > d.lastCleans) {
				HuntXp((now - d.lastCleans) * V().huntSkinXp, "skinning");
				d.lastCleans = now;
			} else if (now < d.lastCleans) {
				d.lastCleans = now;  // the player lowered it from the menu: start from there
			}
		}
	}

	Summary Cooking()
	{
		const auto& f = F();
		return { "Culinary Arts", Val(f.cookProgress), static_cast<int>(Val(f.cookPoints)), static_cast<int>(Val(f.cookEarned)), static_cast<int>(Val(f.cookTotal)) };
	}

	Summary Hunting()
	{
		const auto& f = F();
		return { "Art of the Hunt", Val(f.huntProgress), static_cast<int>(Val(f.huntPoints)), static_cast<int>(Val(f.huntEarned)), static_cast<int>(Val(f.huntTotal)) };
	}

	Summary Wayfarer()
	{
		const auto& f = F();
		return { "Wayfarer", Val(f.wayProgress), static_cast<int>(Val(f.wayPoints)), static_cast<int>(Val(f.wayEarned)), static_cast<int>(Val(f.wayMax)) };
	}

	bool AtCookingStation() { return atStation; }

	bool IsHotMeal(const RE::TESForm* a_food)
	{
		if (!a_food) {
			return false;
		}
		const std::string name = Lower(Name(a_food));
		if (name.find("cold") != std::string::npos || name.find("raw") != std::string::npos) {
			return false;
		}
		for (const char* word : { "stew", "soup", "chowder", "broth", "pie", "porridge", "roast", "tea", "grilled", "baked" }) {
			if (name.find(word) != std::string::npos) {
				return true;
			}
		}
		// Gourmet and similar mods mark their hot dishes with a keyword such as MAG_FoodTypeStewHot.
		if (const auto* kf = const_cast<RE::TESForm*>(a_food)->As<RE::BGSKeywordForm>()) {
			for (std::uint32_t i = 0; i < kf->numKeywords; ++i) {
				if (auto* kw = kf->keywords[i]) {
					const char* id = kw->GetFormEditorID();
					if (id && id[0] && Lower(id).find("hot") != std::string::npos) {
						return true;
					}
				}
			}
		}
		return false;
	}

	void AddWayfarerXp(float a_xp, const std::string& a_why) { WayXp(a_xp, a_why); }
}
