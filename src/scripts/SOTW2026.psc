Scriptname SOTW2026 Hidden

; Skills of the Wild 2026: what SkillsOfTheWild.dll gives the scripts. When the plugin is loaded and has found Skills of the Wild's records it runs the
; three experience trees (Culinary Arts, Art of the Hunt, Wayfarer) and the compass rules itself, and the scripts of the original mod step aside so
; nothing is counted twice. Without the plugin the scripts work exactly as before.

; True when the plugin has taken over.
Bool Function Active() Global Native

; The same, but safe to call whether or not the plugin is installed.
Bool Function NativeActive() Global
	return SKSE.GetPluginVersion("SkillsOfTheWild") > 0 && Active()
EndFunction
