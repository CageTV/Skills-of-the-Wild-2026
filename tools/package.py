"""Lays out the Skills of the Wild 2026 packages (staging in package/, nothing outside the workspace is touched).

  package/Skills of the Wild 2026            the layer for a quick install: new SkillsOfTheWild.dll + the script layer + README + LICENSE
  package/Skills of the Wild 2026 - Installer(.zip)   FOMOD: pick the DLL build (new / older); the script layer comes with it

Needs plugin/build/release/SkillsOfTheWild.dll (older, plugin/build.cmd), E:/WorkSpace/ng-build/build/release/out/SkillsOfTheWild/SkillsOfTheWild.dll
(new, ng-build/build.cmd SkillsOfTheWild) and build/scripts (tools/build_scripts.py).

Usage: python tools/package.py [version]
"""
import os
import shutil
import sys
import xml.dom.minidom
import zipfile

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
VERSION = sys.argv[1] if len(sys.argv) > 1 else "1.0.0"
OLD = os.path.join(HERE, "plugin", "build", "release", "SkillsOfTheWild.dll")
NEW = r"E:\WorkSpace\ng-build\build\release\out\SkillsOfTheWild\SkillsOfTheWild.dll"
SCRIPTS = os.path.join(HERE, "build", "scripts")
ESP = os.path.join(HERE, "esl-work", "SkillsOfTheWild.esp")   # tools/esl_build.py: the plugin with Campfire 2026 ESL numbering
for p in (OLD, NEW, SCRIPTS, ESP):
    assert os.path.exists(p), f"missing {p}"
PEX = ["SOTW2026.pex", "CookingScript.pex", "HuntingScript.pex", "_WWW_HUD.pex"]
PSC = ["SOTW2026.psc", "CookingScript.psc", "HuntingScript.psc", "_WWW_HUD.psc"]


def put(src, dst):
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(src, dst)


def scripts_into(root):
    for n in PEX:
        put(os.path.join(SCRIPTS, n), os.path.join(root, "scripts", n))
    for n in PSC:
        put(os.path.join(HERE, "src", "scripts", n), os.path.join(root, "scripts", "source", n))


# ---- the quick layer --------------------------------------------------------------------------------------------------------------
quick = os.path.join(HERE, "package", "Skills of the Wild 2026")
if os.path.exists(quick):
    shutil.rmtree(quick)
put(NEW, os.path.join(quick, "SKSE", "Plugins", "SkillsOfTheWild.dll"))
scripts_into(quick)
put(os.path.join(HERE, "docs", "README.txt"), os.path.join(quick, "Skills of the Wild 2026 README.txt"))
put(os.path.join(HERE, "LICENSE.txt"), os.path.join(quick, "Skills of the Wild 2026 LICENSE.txt"))

# ---- the same layer for Campfire 2026 ESL: it also replaces SkillsOfTheWild.esp with the copy that points at the renumbered Campfire records -----------------
eslq = os.path.join(HERE, "package", "Skills of the Wild 2026 ESL")
if os.path.exists(eslq):
    shutil.rmtree(eslq)
shutil.copytree(quick, eslq)
put(ESP, os.path.join(eslq, "SkillsOfTheWild.esp"))

# ---- the installer ----------------------------------------------------------------------------------------------------------------
dest = os.path.join(HERE, "package", "Skills of the Wild 2026 - Installer")
if os.path.exists(dest):
    shutil.rmtree(dest)
put(NEW, os.path.join(dest, "new", "SKSE", "Plugins", "SkillsOfTheWild.dll"))
put(OLD, os.path.join(dest, "older", "SKSE", "Plugins", "SkillsOfTheWild.dll"))
put(ESP, os.path.join(dest, "esl", "SkillsOfTheWild.esp"))
scripts_into(os.path.join(dest, "scripts"))
put(os.path.join(HERE, "docs", "README.txt"), os.path.join(dest, "README.txt"))
put(os.path.join(HERE, "LICENSE.txt"), os.path.join(dest, "LICENSE.txt"))
NEW_VER = "1.6.1170.0"
config = f"""<config xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://qconsulting.ca/fo3/ModConfig5.0.xsd">
  <moduleName>Skills of the Wild 2026</moduleName>
  <requiredInstallFiles>
    <file source="README.txt" destination="Skills of the Wild 2026 README.txt"/>
    <file source="LICENSE.txt" destination="Skills of the Wild 2026 LICENSE.txt"/>
    <folder source="scripts" destination=""/>
  </requiredInstallFiles>
  <installSteps order="Explicit">
    <installStep name="Game version">
      <optionalFileGroups order="Explicit">
        <group name="SkillsOfTheWild.dll" type="SelectExactlyOne">
          <plugins order="Explicit">
            <plugin name="New (Skyrim 1.6.1170 and newer)">
              <description>The new build. Works on Skyrim SE/AE 1.6.1170 and on every later version, including 1.7.x. Pre-selected when the installer sees 1.6.1170 or newer.</description>
              <files><folder source="new" destination=""/></files>
              <typeDescriptor><dependencyType><defaultType name="Optional"/><patterns>
                <pattern><dependencies><gameDependency version="{NEW_VER}"/></dependencies><type name="Recommended"/></pattern>
              </patterns></dependencyType></typeDescriptor>
            </plugin>
            <plugin name="Older (Skyrim VR and 1.6.1130 and older)">
              <description>The older build, for Skyrim VR and SE/AE 1.6.1130 and older. On 1.6.1170 either build works.</description>
              <files><folder source="older" destination=""/></files>
              <typeDescriptor><dependencyType><defaultType name="Optional"/><patterns>
                <pattern><dependencies><gameDependency version="{NEW_VER}"/></dependencies><type name="Optional"/></pattern>
              </patterns></dependencyType></typeDescriptor>
            </plugin>
          </plugins>
        </group>
      </optionalFileGroups>
    </installStep>
    <installStep name="Campfire build">
      <optionalFileGroups order="Explicit">
        <group name="Which Campfire do you use?" type="SelectExactlyOne">
          <plugins order="Explicit">
            <plugin name="Campfire (the original from Nexus)">
              <description>The normal Campfire. Skills of the Wild's own plugin stays as it is.</description>
              <typeDescriptor><type name="Recommended"/></typeDescriptor>
            </plugin>
            <plugin name="Campfire 2026 ESL">
              <description>Campfire 2026's light (ESL) build renumbers every Campfire record, so Skills of the Wild's plugin has to point at the new numbers. This installs a copy of SkillsOfTheWild.esp that does (the same file name, so it replaces the original's). Choose this only with Campfire 2026 ESL.</description>
              <files><folder source="esl" destination=""/></files>
              <typeDescriptor><type name="Optional"/></typeDescriptor>
            </plugin>
          </plugins>
        </group>
      </optionalFileGroups>
    </installStep>
  </installSteps>
</config>
"""
info = (f"<fomod>\n  <Name>Skills of the Wild 2026</Name>\n  <Author>CageTV (Skills of the Wild by JaySerpa)</Author>\n  <Version>{VERSION}</Version>\n"
        "  <Website>https://github.com/CageTV/Skills-of-the-Wild-2026</Website>\n"
        "  <Description>The skill trees of Skills of the Wild run on the game's own events, with settings in SKSE Menu Framework and links to Last Seed, Frostfall and Simple Hunting Overhaul.</Description>\n</fomod>\n")
xml.dom.minidom.parseString(config)
xml.dom.minidom.parseString(info)
os.makedirs(os.path.join(dest, "fomod"), exist_ok=True)
open(os.path.join(dest, "fomod", "ModuleConfig.xml"), "w", encoding="utf-8", newline="\n").write(config)
open(os.path.join(dest, "fomod", "info.xml"), "w", encoding="utf-8", newline="\n").write(info)
zpath = dest + ".zip"
if os.path.exists(zpath):
    os.remove(zpath)
with zipfile.ZipFile(zpath, "w", zipfile.ZIP_DEFLATED) as z:
    for dp, _, fs in os.walk(dest):
        for fn in fs:
            p = os.path.join(dp, fn)
            z.write(p, os.path.relpath(p, dest))
    names = z.namelist()
print(f"{zpath}: {len(names)} entries, {os.path.getsize(zpath)//1024} KB")
for n in sorted(names):
    print("  ", n)

# ---- zips of the two quick layers ------------------------------------------------------------------------------------------------------
for folder in (quick, eslq):
    zp = folder + ".zip"
    if os.path.exists(zp):
        os.remove(zp)
    with zipfile.ZipFile(zp, "w", zipfile.ZIP_DEFLATED) as z:
        for dp, _, fs in os.walk(folder):
            for fn in fs:
                pth = os.path.join(dp, fn)
                z.write(pth, os.path.relpath(pth, folder))
    print(f"{zp}: {os.path.getsize(zp)//1024} KB")
