/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// Local FormIDs of the records SkillsOfTheWild.dll reads or writes. Read from SkillsOfTheWild.esp 2.18 (Nexus 37693; an ESL-flagged plugin, so these are
// the local ids), Simple Hunting Overhaul.esp 1.16 and Skyrim.esm on 2026-10-17 (Spriggit export + houseCARL).
namespace ids
{
	inline constexpr const char* kSotw = "SkillsOfTheWild.esp";
	inline constexpr const char* kSho = "Simple Hunting Overhaul.esp";
	inline constexpr const char* kSkyrim = "Skyrim.esm";
	inline constexpr const char* kHearth = "HearthFires.esm";
	inline constexpr const char* kFrostfall = "Frostfall.esp";
	inline constexpr const char* kLastSeed = "LastSeed.esp";

	// ---- SkillsOfTheWild.esp: Culinary Arts ----
	inline constexpr unsigned CookProgress = 0x000807;     // _CCCooking_PerkPointProgress (0..1, the share of the next perk point)
	inline constexpr unsigned CookPoints = 0x000808;       // _CCCooking_PerkPoints (unspent)
	inline constexpr unsigned CookEarned = 0x000809;       // _CCCooking_PerkPointsEarned
	inline constexpr unsigned CookTotal = 0x00080A;        // _CCCooking_PerkPointsTotal (the most you can earn)
	inline constexpr unsigned CookXpToNext = 0x00080B;     // _CCCooking_XPtoNewPerk (+9 for every point earned)
	inline constexpr unsigned CookBasicMeals = 0x00080C;   // _CCC_PerkRank_BasicMeals   (Low Heat)
	inline constexpr unsigned CookElaborate = 0x00080E;    // _CCC_PerkRank_ElaborateMeals (Exotic Taste)
	inline constexpr unsigned CookExotic = 0x000810;       // _CCC_PerkRank_ExoticMeals  (Healthy Diet)
	inline constexpr unsigned CookSpecialSauce = 0x000812; // _CCC_PerkRank_SpecialSauce (Inspired Cook)
	inline constexpr unsigned CookGourmet = 0x000814;      // _CCC_PerkRank_Gourmet      (Frugal Gourmet)
	inline constexpr unsigned PerkIsCooking = 0x000806;    // _CCC_iscooking: an empty perk other patches may look for while the player cooks
	inline constexpr unsigned PerkThings = 0x000840;       // _AAA_PerkThings
	inline constexpr unsigned PerkMortar = 0x0009A2;       // _MortarPerk_
	inline constexpr unsigned SpellInspired1 = 0x000838;   // _CCC_InspiredCook
	inline constexpr unsigned SpellInspired2 = 0x00083D;   // _CCC_InspiredCook2
	inline constexpr unsigned SpellSitting = 0x000843;     // _SittingAlways_Spell
	inline constexpr unsigned SpellCrossHair = 0x000BA3;   // _CrossHairSneak
	inline constexpr unsigned PotionLowHeat1 = 0x000844;
	inline constexpr unsigned PotionLowHeat2 = 0x000845;
	inline constexpr unsigned PotionExotic1 = 0x000846;
	inline constexpr unsigned PotionExotic2 = 0x000847;
	inline constexpr unsigned PotionHealthy1 = 0x000848;
	inline constexpr unsigned PotionHealthy2 = 0x000849;
	inline constexpr unsigned ListCookingBooks = 0x00099C; // _CCC_CookingBooks
	inline constexpr unsigned ListIgnoreFood = 0x00099F;   // _CCC_IgnoreListFood
	inline constexpr unsigned ListKnownRecipes = 0x0009A0; // _CCC_AlreadyKnownRecipes

	// ---- SkillsOfTheWild.esp: Art of the Hunt ----
	inline constexpr unsigned HuntProgress = 0x00084E;     // _HHH_PerkPointProgress
	inline constexpr unsigned HuntPoints = 0x00084F;       // _HHH_PerkPoints
	inline constexpr unsigned HuntEarned = 0x000850;       // _HHH_PerkPointsEarned
	inline constexpr unsigned HuntTotal = 0x000851;        // _HHH_PerkPointsTotal
	inline constexpr unsigned HuntArtRank = 0x000852;      // _HHH_PerkRank_ArtoftheHunt
	inline constexpr unsigned HuntKills = 0x00085F;        // HHH_KillsTotal
	inline constexpr unsigned ListHuntingBooks = 0x00099D; // _HHH_HuntingBooks

	// ---- SkillsOfTheWild.esp: Wayfarer ----
	inline constexpr unsigned WayProgress = 0x000930;      // _WWW_Progress
	inline constexpr unsigned WayEarned = 0x000931;        // _WWW_EarnedPoints
	inline constexpr unsigned WayMax = 0x000932;           // _WWW_MaxPerkPoints
	inline constexpr unsigned WayPoints = 0x000933;        // _WWW_PerkPoints
	inline constexpr unsigned WayLimit = 0x000934;         // _WWW_MovingXPLimit (+5 for every point earned)
	inline constexpr unsigned WayXp = 0x000935;            // _WWW_CurrentXP
	inline constexpr unsigned WaySixthSense = 0x000956;    // _WWW_PerkRank_SixthSense: unlocks the sneak eye
	inline constexpr unsigned WaySpatial = 0x000958;       // _WWW_PerkRank_SpatialAwareness: unlocks the compass
	inline constexpr unsigned ListWayBooks = 0x00099B;     // _WWW_WayfaringBooks
	inline constexpr unsigned ListBooksRead = 0x00099E;    // _WWW_WayfaringBooksREAD (shared by all three trees)

	// ---- SkillsOfTheWild.esp: settings that lived only in the console ----
	inline constexpr unsigned CompassCheat = 0x000863;     // CompassCheat (1 = compass and sneak eye from the start)
	inline constexpr unsigned DefaultJump = 0x000FAB;      // DefaultJumpHeightValue (76 in vanilla)

	// ---- Skyrim.esm / HearthFires.esm ----
	inline constexpr unsigned KwVendorFood = 0x08CDEA;
	inline constexpr unsigned KwVendorIngredient = 0x08CDEB;
	inline constexpr unsigned KwNoExp = 0x08CDED;          // the "no experience" keyword Skills of the Wild's own scripts honour
	inline constexpr unsigned KwCookpot = 0x0A5CB3;        // CraftingCookpot
	inline constexpr unsigned KwBYOHOven = 0x0117F7;       // BYOHCraftingOven (HearthFires.esm)
	inline constexpr unsigned ChefHat = 0x01BCA7;          // ClothesChefHat

	// ---- Simple Hunting Overhaul.esp ----
	inline constexpr unsigned ShoXp = 0x000822;            // _HuntingXP (0..100; Skills of the Wild's script calls it _DS_Hunterborn_TotalCleans)
	inline constexpr unsigned ShoVanillaPelts = 0x00081F;
	inline constexpr unsigned ShoBrumaPelts = 0x000820;
	inline constexpr unsigned ShoCustomPelts = 0x000BF2;
	inline constexpr unsigned ShoBlacklist = 0x0008E5;     // _SHO_Blacklist

	// ---- Frostfall.esp (normal build / ESL build) ----
	inline constexpr unsigned FfRunning = 0x06DCFB;
	inline constexpr unsigned FfRunningEsl = 0x000A9A;
	inline constexpr unsigned FfExposure = 0x08827A;       // _Frost_AttributeExposureMeter (higher = colder)
	inline constexpr unsigned FfExposureEsl = 0x000B96;
	inline constexpr unsigned FfExposureMax = 0x07CAA4;
	inline constexpr unsigned FfExposureMaxEsl = 0x000AE7;

	// ---- LastSeed.esp (normal build / ESL build) ----
	inline constexpr unsigned LsRunning = 0x00B162;
	inline constexpr unsigned LsRunningEsl = 0x0008C2;
	inline constexpr unsigned LsHunger = 0x0029CD;         // _Seed_AttributeHunger (higher = hungrier)
	inline constexpr unsigned LsHungerEsl = 0x000819;
	inline constexpr unsigned LsHungerMax = 0x0102A9;
	inline constexpr unsigned LsHungerMaxEsl = 0x000939;
}
