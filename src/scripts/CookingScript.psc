Scriptname CookingScript extends ReferenceAlias  

Keyword Property CraftingCookpot  Auto  
Keyword Property BYOHCraftingOven  Auto  
Keyword Property UsesCookingSkill  Auto  
ObjectReference Property furn  Auto  
Actor Property PlayerRef  Auto  
Float Property XPMult  Auto  
Float Property PlayerXP  Auto  
Float Property NewPerk  Auto  
Perk Property _CCC_iscooking  Auto  
GlobalVariable Property _CCCooking_PerkPointProgress  Auto  
GlobalVariable Property _CCCooking_PerkPoints  Auto  
GlobalVariable Property _CCCooking_PerkPointsEarned  Auto  
GlobalVariable Property _CCCooking_PerkPointsTotal  Auto  
GlobalVariable Property _CCCooking_XPtoNewPerk  Auto  
GlobalVariable Property _CCC_PerkRank_Gourmet  Auto  
GlobalVariable Property _WWW_PerkRank_NorthFinder Auto
GlobalVariable Property CompassCheat Auto
Float Property HatXP  Auto
GlobalVariable Property _CCC_PerkRank_SpecialSauce  Auto  
GlobalVariable Property _HHH_PerkRank_ArtofTheHunt  Auto  
GlobalVariable Property _CCC_PerkRank_BasicMeals Auto
GlobalVariable Property _CCC_PerkRank_ElaborateMeals Auto
GlobalVariable Property _CCC_PerkRank_ExoticMeals Auto
SPELL Property _CCC_InspiredCook  Auto  
SPELL Property _CCC_InspiredCook2  Auto  
SPELL Property _Camp_CreateItemSpell Auto
SPELL Property _Camp_SurvivalVisionPower auto
Spell Property _CrossHairSneak Auto
Spell Property _Camp_HarvestWoodSpell Auto
Armor Property ClothesChefHat  Auto  
Float Property progressforscript Auto
GlobalVariable Property _HHH_PerkRank_Handcrafting Auto  
Keyword Property NoEXP Auto
Furniture Property _Camp_MortarAndPestleFurniture Auto
Perk Property _MortarPerk_ Auto
Perk Property _AAA_PerkThings Auto
Spell Property _SittingAlways_Spell Auto
Keyword Property VendorItemIngredient Auto
Keyword Property VendorItemFood Auto
Potion Property _LowHeat_Potion1 Auto
Potion Property _LowHeat_Potion2 Auto
Potion Property _ExoticTaste_Potion1 Auto
Potion Property _ExoticTaste_Potion2 Auto
Potion Property _HealthyDiet_Potion1 Auto
Potion Property _HealthyDiet_Potion2 Auto

GlobalVariable Property SneakEyePerk Auto
GlobalVariable Property CompassPerk Auto
GlobalVariable Property DefaultJumpHeightValue Auto

FormList Property _CCC_CookingBooks Auto
FormList Property _WWW_WayfaringBooksREAD Auto
FormList Property _CCC_IgnoreListFood Auto
FormList Property _CCC_AlreadyKnownRecipes Auto

Event OnInit()
	Maintenance()
	PlayerRef.AddPerk(_AAA_PerkThings)
	PlayerRef.AddPerk(_MortarPerk_)
	PlayerRef.AddSpell(_SittingAlways_Spell, false)
	; Skills of the Wild 2026: SkillsOfTheWild.dll hides the compass and the sneak eye itself when it is installed
	If !SOTW2026.NativeActive()
		UI.SetFloat("HUD Menu", "_root.HUDMovieBaseInstance.StealthMeterInstance._alpha", 0)
		UI.SetFloat("HUD Menu", "_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha", 0)
	EndIf
EndEvent

Event OnPlayerLoadGame() 
	Maintenance()
EndEvent

Function Maintenance()
	Utility.Wait(2.0)
	If _HHH_PerkRank_ArtofTheHunt.GetValue() == 0
		PlayerRef.RemoveSpell(_Camp_SurvivalVisionPower)
	Endif	
	
	If _HHH_PerkRank_Handcrafting.GetValue() == 0
		PlayerRef.RemoveSpell(_Camp_CreateItemSpell)
	Endif
	
	If _WWW_PerkRank_NorthFinder.GetValue() == 0
		PlayerRef.RemoveSpell(_Camp_HarvestWoodSpell)
	endif

	; Skills of the Wild 2026: the compass, the sneak eye and the jump height are the plugin's job when it is installed
	If SOTW2026.NativeActive()
		Return
	EndIf

	If SneakEyePerk.GetValue() == 0 && CompassCheat.GetValue() == 0
		PlayerRef.AddSpell(_CrossHairSneak, false)
		UI.SetFloat("HUD Menu", "_root.HUDMovieBaseInstance.StealthMeterInstance._alpha", 0)
	else
		UI.SetFloat("HUD Menu", "_root.HUDMovieBaseInstance.StealthMeterInstance._alpha", 100)
		PlayerRef.RemoveSpell(_CrossHairSneak)
	endif

	If CompassPerk.GetValue() == 0 && CompassCheat.GetValue() == 0
		UI.SetFloat("HUD Menu", "_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha", 0)
	else
		UI.SetFloat("HUD Menu", "_root.HUDMovieBaseInstance.StealthMeterInstance._alpha", 100)
		PlayerRef.Removespell(_CrossHairSneak)
	endif

	If PlayerRef.IsOnMount() == False
		Game.SetGameSettingFloat("fJumpHeightMin", DefaultJumpHeightValue.GetValue())
	endif

EndFunction

Event OnSit(ObjectReference akFurniture)
	; Skills of the Wild 2026: with the plugin, cooking is recognised from the game's own furniture events and the "Cooking" state is never entered
	if !SOTW2026.NativeActive() && (akFurniture.HasKeyWord(CraftingCookpot) || akFurniture.HasKeyWord(BYOHCraftingOven) || akFurniture.HasKeyWord(UsesCookingSkill))
		GoToState("Cooking")
   		PlayerRef.AddPerk(_CCC_iscooking) 
	elseif akFurniture.GetBaseObject() == _Camp_MortarAndPestleFurniture
		Debug.notification("Using a mortar will result in less effective potions!")
	endIf
EndEvent

Event OnGetUp(ObjectReference akFurniture)
	if !SOTW2026.NativeActive() && (akFurniture.HasKeyWord(CraftingCookpot) || akFurniture.HasKeyWord(BYOHCraftingOven) || akFurniture.HasKeyWord(UsesCookingSkill))
		PlayerRef.RemovePerk(_CCC_iscooking) 
		GoToState("")
		
		if PlayerRef.HasSpell(_CCC_InspiredCook) ||  PlayerRef.HasSpell(_CCC_InspiredCook2)
			Debug.Notification("You're feeling inspired!")
		endif
	endIf
EndEvent

State Cooking
	Event OnItemAdded(Form akBaseItem, Int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)
		If PlayerRef.GetSitState() != 3 ; Player is not sitting (cooking), exit cooking mode
			PlayerRef.RemovePerk(_CCC_iscooking) 
			GoToState("")
		Endif
		
		if PlayerRef.HasPerk(_CCC_iscooking) && !_CCC_IgnoreListFood.HasForm(akBaseItem) && !akBaseItem.HasKeyWord(NoEXP) && akBaseItem.haskeyword(VendorItemFood)
			akItemReference == none && akSourceContainer == none && aiItemCount >= 1 
			Potion NewPotion = akBaseItem as potion
			float random = Utility.RandomFloat(0, 100)

			if _CCC_PerkRank_BasicMeals.GetValue() == 1
				PlayerRef.EquipItem(_LowHeat_Potion1, true, true)
			elseif _CCC_PerkRank_BasicMeals.GetValue() == 2
				PlayerRef.EquipItem(_LowHeat_Potion2, true, true)
			endif

			if _CCC_PerkRank_Gourmet.GetValue() == 2 &&  random >= 66
				PlayerRef.RemovePerk(_CCC_iscooking) 
				Utility.Wait(0.2)
				PlayerRef.AddItem(akBaseItem, 1, true)
				Debug.Notification("You have managed to cook a second meal!")
				Utility.Wait(0.2)
				PlayerRef.AddPerk(_CCC_iscooking) 
			endif

			if _CCC_PerkRank_Gourmet.GetValue() == 1 && random >= 75
				PlayerRef.RemovePerk(_CCC_iscooking) 
				Utility.Wait(0.2)
				PlayerRef.AddItem(akBaseItem, 1, true)
				Debug.Notification("You have managed to cook a second meal!")
				Utility.Wait(0.2)
				PlayerRef.AddPerk(_CCC_iscooking) 
			endif

			if PlayerRef.IsEquipped(ClothesChefHat)
				HatXP = 0.03
			Else
				HatXP = 0
			endif

				XPMult = 0.09 + HatXP

			if _CCC_PerkRank_SpecialSauce.GetValue() == 1
				PlayerRef.Addspell(_CCC_InspiredCook, false)
				XPMult = 0.12 + HatXP
			endif

			if _CCC_PerkRank_SpecialSauce.GetValue() == 2 
				PlayerRef.Addspell(_CCC_InspiredCook2, false)
				XPMult = 0.14 + HatXP
			endif

			if _CCCooking_PerkPointsEarned.GetValueInt() < _CCCooking_PerkPointsTotal.GetValueInt()
				float NewXP = (akBaseItem.GetGoldValue()) * XPMult

				If !_CCC_AlreadyKnownRecipes.HasForm(akBaseItem)
					NewXP = (akBaseItem.GetGoldValue() * 3) * XPMult
				endif

				_CCC_AlreadyKnownRecipes.Addform(akBaseItem)

				if _CCCooking_XPtoNewPerk.GetValue() <= (progressforscript + NewXP)
					_CCCooking_PerkPoints.SetValueInt(_CCCooking_PerkPoints.GetValueInt() + 1)
					_CCCooking_PerkPointProgress.SetValue(0)
					progressforscript = 0
					NewXP = 0
					_CCCooking_XPtoNewPerk.SetValueInt(_CCCooking_XPtoNewPerk.GetValueInt() + 9)
					_CCCooking_PerkPointsEarned.SetValueInt(_CCCooking_PerkPointsEarned.GetValueInt() + 1)
					Debug.Notification("You're getting the hang of this! New cooking perk available.")
				else 
					progressforscript = progressforscript + NewxP
					_CCCooking_PerkPointProgress.SetValue(progressforscript / _CCCooking_XPtoNewPerk.GetValueInt())
				endif
			endif
		endif
	EndEvent
EndState

Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
	; Skills of the Wild 2026: the plugin handles eating, ingredients and cooking books
	If SOTW2026.NativeActive()
		Return
	EndIf
	if akBaseObject.HasKeyword(VendorItemIngredient) && _CCC_PerkRank_ElaborateMeals.GetValue() == 1
		PlayerRef.EquipItem(_ExoticTaste_Potion1, true, true)
	elseif akBaseObject.HasKeyword(VendorItemIngredient) && _CCC_PerkRank_ElaborateMeals.GetValue() == 2
		PlayerRef.EquipItem(_ExoticTaste_Potion2, true, true)
	elseif akBaseObject.HasKeyword(VendorItemFood) && _CCC_PerkRank_ExoticMeals.GetValue() == 1
		PlayerRef.EquipItem(_HealthyDiet_Potion1, true, true)
	elseif akBaseObject.HasKeyword(VendorItemFood) && _CCC_PerkRank_ExoticMeals.GetValue() == 2
		PlayerRef.EquipItem(_HealthyDiet_Potion2, true, true)
	elseif _CCC_CookingBooks.HasForm(akBaseObject) && !_WWW_WayfaringBooksREAD.HasForm(akBaseObject)
		Book Book1 = akBaseObject as book 
		_WWW_WayfaringBooksREAD.Addform(Book1)

		if (progressforscript + 4) > _CCCooking_XPtoNewPerk.GetValue()
			Utility.Wait(3)
			Debug.notification("This book has taught me a lot! New cooking perk available.")
			progressforscript = 0
			_CCCooking_PerkPoints.SetValue(_CCCooking_PerkPoints.GetValue() + 1)
			_CCCooking_PerkPointsEarned.SetValue(_CCCooking_PerkPointsEarned.GetValue() + 1)
			_CCCooking_XPtoNewPerk.SetValueInt(_CCCooking_XPtoNewPerk.GetValueInt() + 9)
		else 
			progressforscript = progressforscript + 4
			_CCCooking_PerkPointProgress.SetValue(progressforscript / _CCCooking_XPtoNewPerk.GetValueInt())
		endif
	endif
endevent
