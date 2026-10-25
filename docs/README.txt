Skills of the Wild 2026
Based on Skills of the Wild (Nexus 37693) by JaySerpa. Needs the original mod. Part of the 2026 series with Campfire 2026, Frostfall 2026, Last Seed 2026
and Simple Hunting Overhaul 2026, and it works with all of them when they are installed.

WHAT IT DOES
------------
Skills of the Wild's three experience trees (Culinary Arts, Art of the Hunt, Wayfarer) are now run by an SKSE plugin on the game's own events instead of
by scripts that looked at the world only when you sat down. It writes the same global variables the scripts did, so your saves, the Campfire skill menu
and every other mod's patch see no difference.

  Cooking      Recognised from the game's furniture events at ANY cooking station: the vanilla pot, the Hearthfires oven, and any furniture with a cooking
               keyword (campfire pots, kitchens, Gourmet and Last Seed stations). Meals earn experience the moment they are cooked. Low Heat, Exotic Taste,
               Healthy Diet, Inspired Cook and Frugal Gourmet work as before. No more sit-down trick that could get stuck and duplicate items.
  Hunting      Kills count when you make them, and every skinning counts when you do it. Simple Hunting Overhaul stops counting its own hunting
               experience at 100, so the old tree stopped growing there; now every skinning still earns experience (setting).
  Wayfarer     Days and discovered places count when they happen, not at the next sit-down. Books count for the first tree whose reading list holds them.
  Compass      The compass and the sneak eye stay hidden until the Wayfarer perks that unlock them, as before, but a mod that draws its own compass
               (Compass Navigation Overhaul) can be left alone. This replaces the "Skills of the Wild - Re-enable Compass" patch: remove that patch.
  Settings     SKSE Menu Framework section "Skills of the Wild": experience rates for each tree and per kill, skinning, day and place; the compass rule;
               the compass cheat and the default jump height, which used to be console-only; and the synergies below. No SkyUI or MCM needed.

Beast Handler (taming, riding, pets) is still run by the mod's own scripts.

WORKING TOGETHER WITH THE OTHER 2026 MODS
-----------------------------------------
Each link switches itself on when its partner is installed and running, and each has a switch on the Synergies page.
  Last Seed               The Culinary perk Healthy Diet makes a meal fill you more: a share of the meal's own relief of hunger, per rank.
  Frostfall               A hot dish (stew, soup, chowder, broth, pie...) takes some of the cold out of you. Hours spent outdoors while Frostfall has you
                          cold earn Wayfarer experience.
  Simple Hunting Overhaul Every Art of the Hunt point you earn also raises your hunting experience, so skinning gets faster.
  Campfire 2026, Gourmet  Nothing to set: their cooking stations and foods are recognised and earn Culinary Arts experience like any other.

THE SCRIPT LAYER (needed)
-------------------------
Three of the original mod's scripts (CookingScript, HuntingScript, _WWW_HUD) are rebuilt so that they step aside when the plugin has taken over, and
a new SOTW2026 script tells them whether it has. Without the plugin they behave exactly as before. Without these scripts the plugin does nothing
(it checks, and says so in the Overview page and in Documents/My Games/Skyrim Special Edition/SKSE/SkillsOfTheWild.log) so nothing is counted twice.

REQUIREMENTS
------------
- Skills of the Wild (Nexus 37693) and what it requires: Campfire, Simple Hunting Overhaul, Dynamic Activation Key
- SKSE Menu Framework 3
- Remove the "Skills of the Wild - Re-enable Compass" patch (this replaces it)

LOAD ORDER
----------
Install after (below) the original Skills of the Wild in MO2's left pane so the three scripts win. The plugin has no .esp. Safe to add or remove
in the middle of a save: nothing but the original globals is changed.

CAMPFIRE 2026 ESL
-----------------
SkillsOfTheWild.esp is already a light plugin, but it points at records of Campfire.esm by their original numbers, and Campfire 2026 ESL renumbers all of them.
With that build install the "ESL" layer (or choose "Campfire 2026 ESL" in the installer): it brings a copy of SkillsOfTheWild.esp whose 115 Campfire references
(30 records) use the new numbers, checked record by record against both Campfire builds. Everything else in the plugin is identical. With the normal
Campfire do NOT use it. The plugin (the DLL) reads Frostfall and Last Seed whichever build they are, normal or ESL, by itself.

BUILDS
------
The installer asks which SkillsOfTheWild.dll: "new" for Skyrim 1.6.1170 and newer, "older" for Skyrim VR and 1.6.1130 and older.

CREDITS AND LICENSE
-------------------
Skills of the Wild by JaySerpa. The plugin, the settings pages and the script changes are by CageTV, MIT licensed (see LICENSE.txt); the plugin
contains none of the original mod's meshes, textures, sounds or plugin. The three scripts are rebuilt from JaySerpa's sources.
