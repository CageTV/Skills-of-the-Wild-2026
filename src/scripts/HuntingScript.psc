Scriptname HuntingScript extends ReferenceAlias  

GlobalVariable Property _HHH_PerkPointProgress  Auto  
GlobalVariable Property _HHH_PerkPoints  Auto  
GlobalVariable Property _HHH_PerkPointsEarned  Auto  
GlobalVariable Property _HHH_PerkPointsTotal  Auto  
GlobalVariable Property _DS_Hunterborn_TotalCleans  Auto  
GlobalVariable Property HHH_KillsTotal Auto 
GlobalVariable Property _HHH_PerkRank_ArtoftheHunt Auto 
Float Property NewXP Auto
Float Property RecordedHunterbornXP Auto
Float Property XPfromHunterborn Auto
Float Property XPtoNewPerk Auto
Float Property progressforscript Auto
Spell Property _Camp_CreateItemSpell Auto
Spell Property _Camp_SurvivalVisionPower Auto
Actor property PlayerRef Auto

Event OnSit(ObjectReference akFurniture)

If _HHH_PerkRank_ArtoftheHunt.GetValue() == 0
PlayerRef.RemoveSpell(_Camp_SurvivalVisionPower)
Endif
; Skills of the Wild 2026: the plugin counts kills and skinnings itself, when they happen
If SOTW2026.NativeActive()
	Return
EndIf
Utility.Wait(6.0)
NewXP = 0
XPFromHunterborn = 0
KillCounter()

EndEvent

Function KillCounter()
If _HHH_PerkPointsEarned.GetValueInt() < _HHH_PerkPointsTotal.GetValueInt()
int UpdatedCount = game.QueryStat("Animals Killed") + game.QueryStat("Creatures Killed")

If UpdatedCount > HHH_KillsTotal.GetValueInt() 
int NewAnimalstoGetXPfrom = UpdatedCount - HHH_KillsTotal.GetValueInt()
NewXP = NewAnimalstoGetXPfrom * 0.2
HHH_KillsTotal.SetValueInt(UpdatedCount)
endif

if RecordedHunterbornXP < _DS_Hunterborn_TotalCleans.GetValueInt() && _DS_Hunterborn_TotalCleans.GetValueInt() > 1
XPfromHunterborn = (_DS_Hunterborn_TotalCleans.GetValueInt() - RecordedHunterbornXP) * 1.1
RecordedHunterbornXP = _DS_Hunterborn_TotalCleans.GetValueInt()
endif

if XPtoNewPerk <= (progressforscript + NewXP + XPfromHunterborn)

		_HHH_PerkPoints.SetValueInt(_HHH_PerkPoints.GetValueInt() + 1)
		_HHH_PerkPointProgress.SetValue(0)
		progressforscript = 0
		XPtoNewPerk = XPtoNewPerk + 6
		_HHH_PerkPointsEarned.SetValueInt(_HHH_PerkPointsEarned.GetValueInt() + 1)
		Debug.Notification("You reflect on your hunting experience. New perk available.")

		else 
			progressforscript = progressforscript + NewxP + XPfromHunterborn
			_HHH_PerkPointProgress.SetValue(progressforscript / XPtoNewPerk)
			endif
Endif
EndFunction


Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
If SOTW2026.NativeActive()
	Return
EndIf
if _HHH_HuntingBooks.HasForm(akBaseObject) && !_WWW_WayfaringBooksREAD.HasForm(akBaseObject)
Book Book1 = akBaseObject as book 
_WWW_WayfaringBooksREAD.Addform(Book1)

if (progressforscript + 2) > XPtoNewPerk
Utility.Wait(5)
Debug.notification("This book has taught me a lot! New hunting perk available.")
progressforscript = 0
XPtoNewPerk = XPtoNewPerk + 6
_HHH_PerkPointProgress.SetValue(0)
_HHH_PerkPoints.SetValue(_HHH_PerkPoints.GetValue() + 1)
_HHH_PerkPointsEarned.SetValue(_HHH_PerkPointsEarned.GetValue() + 1)

else 

progressforscript = progressforscript + 2
_HHH_PerkPointProgress.SetValue(progressforscript / XPtoNewPerk)
endif
endif
endevent

FormList Property _HHH_HuntingBooks Auto
FormList Property _WWW_WayfaringBooksREAD Auto