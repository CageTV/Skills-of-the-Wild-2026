Scriptname _WWW_HUD extends ReferenceAlias  
Import Utility
Actor Property PlayerRef Auto
Int Property _WWW_StoredLocations Auto
Int Property _WWW_StoredDays Auto
GlobalVariable Property _WWW_CurrentXP Auto
GlobalVariable Property _WWW_MovingXPLimit Auto
GlobalVariable Property _WWW_PerkPoints Auto
GlobalVariable Property _WWW_EarnedPoints Auto
GlobalVariable Property _WWW_Progress Auto
GlobalVariable Property _WWW_MaxPerkPoints Auto
float property XPfromdays auto
float property XPfromlocations auto

Event OnSit(ObjectReference akFurniture)

; Skills of the Wild 2026: the plugin counts days and places itself, when they pass
If SOTW2026.NativeActive()
	Return
EndIf

If _WWW_EarnedPoints.GetValue() < _WWW_MaxPerkPoints.GetValue()

;Checking that we don't already have maxed all XP.

if Game.QueryStat("Days Passed") > _WWW_StoredDays
XPfromdays = (Game.QueryStat("Days Passed") - _WWW_StoredDays) * 0.9
_WWW_StoredDays =  Game.QueryStat("Days Passed")
else 
XPfromdays = 0
endif

if Game.QueryStat("Locations Discovered") > _WWW_StoredLocations
XPfromlocations = (Game.QueryStat("Locations Discovered") - _WWW_StoredLocations) * 0.6
_WWW_StoredLocations =  Game.QueryStat("Locations Discovered")
else
XPfromlocations = 0
endif

if (_WWW_CurrentXP.GetValue() + XPfromlocations + XPFromDays) > _WWW_MovingXPLimit.GetValue()
Debug.notification("Your wayfaring skills have improved! New perk available.")

_WWW_MovingXPLimit.SetValue(_WWW_MovingXPLimit.GetValue() + 5)
_WWW_Progress.SetValue(0)
_WWW_CurrentXP.SetValue(0)
_WWW_PerkPoints.SetValue(_WWW_PerkPoints.GetValue() + 1)
_WWW_EarnedPoints.SetValue(_WWW_EarnedPoints.GetValue() + 1)

else 

_WWW_CurrentXP.SetValue(_WWW_CurrentXP.GetValue()+ XPfromlocations + XPfromdays)
_WWW_Progress.SetValue(_WWW_CurrentXP.GetValue() / _WWW_MovingXPLimit.GetValue())
endif

endif
EndEvent

Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
If SOTW2026.NativeActive()
	Return
EndIf
if _WWW_WayfaringBooks.HasForm(akBaseObject) && !_WWW_WayfaringBooksREAD.HasForm(akBaseObject)
Book Book1 = akBaseObject as book 
_WWW_WayfaringBooksREAD.Addform(Book1)

if (_WWW_CurrentXP.GetValue() + 2) > _WWW_MovingXPLimit.GetValue()
Utility.Wait(5)
Debug.notification("This book has taught me a lot! New wayfaring perk available.")
_WWW_Progress.SetValue(0)
_WWW_CurrentXP.SetValue(0)
_WWW_PerkPoints.SetValue(_WWW_PerkPoints.GetValue() + 1)
_WWW_EarnedPoints.SetValue(_WWW_EarnedPoints.GetValue() + 1)

else 

_WWW_CurrentXP.SetValue(_WWW_CurrentXP.GetValue()+ 2)
_WWW_Progress.SetValue(_WWW_CurrentXP.GetValue() / _WWW_MovingXPLimit.GetValue())
endif
endif
endevent

FormList Property _WWW_WayfaringBooks Auto
FormList Property _WWW_WayfaringBooksREAD Auto
