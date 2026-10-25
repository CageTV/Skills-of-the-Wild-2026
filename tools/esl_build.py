"""Builds the Campfire-ESL copy of SkillsOfTheWild.esp and checks it.

Skills of the Wild's plugin is already light (ESL-flagged) but it points at records of Campfire.esm by their original FormIDs. Campfire 2026 ESL renumbers
every Campfire record into the light range (esl-work/map_campfire.json of the Campfire 2026 workspace), so with that build the original SkillsOfTheWild.esp
points at the wrong records. This writes a copy whose Campfire references use the new numbers, the way the Campfire patches were remapped:
  1. serializes the original with Spriggit,
  2. rewrites every NNNNNN:Campfire.esm reference with the map,
  3. deserializes the result to esl-work/SkillsOfTheWild.esp (same file name),
  4. re-serializes that and compares it with step 2, so a lossy round trip is caught,
  5. checks, for every Campfire record the plugin uses, that the record at the old number in the shipped Campfire.esm and the record at the new
     number in the ESL Campfire.esm have the same EditorID.
Nothing in the install is modified. Usage: python tools/esl_build.py
"""
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CAMP = r"E:\WorkSpace\Campfire-2026"
MODS = r"E:\Tabula Rasa\mods"
SPRIGGIT = os.path.join(MODS, "Skyrim-Claude Code Modder's Toolkit", "tools", "spriggit-cli.sh")
ORIGINAL = r"H:\Hunting\Skills of the Wild\SkillsOfTheWild.esp"
W = os.path.join(HERE, "work")
OUT = os.path.join(HERE, "esl-work")
mp = json.load(open(os.path.join(CAMP, "esl-work", "map_campfire.json")))
ref = re.compile(r"\b([0-9A-F]{6}):Campfire\.esm")


def spriggit(*args):
    r = subprocess.run(["bash", SPRIGGIT, *args], capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit("spriggit failed: " + " ".join(args) + "\n" + (r.stdout + r.stderr)[-600:])


def serialize(plugin_path, dest):
    if os.path.exists(dest):
        shutil.rmtree(dest)
    stage = os.path.join(W, "stage", os.path.basename(plugin_path))
    os.makedirs(os.path.dirname(stage), exist_ok=True)
    shutil.copyfile(plugin_path, stage)
    spriggit("serialize", "--InputPath", stage, "--OutputPath", dest, "--GameRelease", "SkyrimSE", "--PackageName", "Spriggit.Yaml", "--PackageVersion", "0.41.0")


def read(p):
    with open(p, encoding="utf-8", errors="replace", newline="") as f:
        return f.read()


def norm_tree(root):
    """{relative path: sorted non-empty lines} with -0 normalised, so list order and negative zero do not count as differences."""
    out = {}
    for dp, _, fs in os.walk(root):
        for fn in fs:
            if fn.endswith(".yaml"):
                p = os.path.join(dp, fn)
                lines = sorted(l.strip().replace(", -0,", ", 0,") for l in read(p).replace("\r", "").split("\n") if l.strip())
                # records that override Campfire's carry its FormID in the file name, which differs by build: key on the name without it
                out[re.sub(r" - [0-9A-F]{6}_Campfire", " - _Campfire", os.path.relpath(p, root))] = lines
    return out


def editor_ids(root):
    """{FormID (6 hex): EditorID} from the file names of a Spriggit export of Campfire.esm."""
    d = {}
    pat = re.compile(r"^(.*) - ([0-9A-F]{6})_Campfire\.esm\.yaml$")
    for dp, _, fs in os.walk(root):
        for fn in fs:
            m = pat.match(fn)
            if m:
                d[m.group(2)] = m.group(1)
    return d


def remap_tree(src, dst):
    if os.path.exists(dst):
        shutil.rmtree(dst)
    shutil.copytree(src, dst)
    bad, used, refs = set(), set(), 0
    for dp, _, fs in os.walk(dst):
        for fn in fs:
            if not fn.endswith(".yaml"):
                continue
            p = os.path.join(dp, fn)
            t = read(p)
            found = ref.findall(t)
            refs += len(found)

            def f(m):
                used.add(m.group(1))
                if m.group(1) not in mp:
                    bad.add(m.group(1))
                    return m.group(0)
                return mp[m.group(1)] + ":Campfire.esm"
            t2 = ref.sub(f, t)
            if t2 != t:
                with open(p, "w", encoding="utf-8", newline="") as o:
                    o.write(t2)
    return bad, used, refs


os.makedirs(OUT, exist_ok=True)
y_orig = os.path.join(W, "y_orig")
serialize(ORIGINAL, y_orig)
y_map = os.path.join(W, "y_map")
bad, used, refs = remap_tree(y_orig, y_map)
if bad:
    sys.exit(f"UNMAPPED Campfire ids: {sorted(bad)}")

# (5) every record used is the same record in the new numbering
old_ids = editor_ids(os.path.join(CAMP, "src", "esp", "Campfire"))
new_ids = editor_ids(os.path.join(CAMP, "esl-work", "yaml"))
mismatch = []
for old in sorted(used):
    new = mp[old]
    a, b = old_ids.get(old), new_ids.get(new)
    if a != b:
        mismatch.append((old, new, a, b))
print(f"{len(used)} Campfire records used, {refs} references remapped; EditorID check: {len(used) - len(mismatch)} same, {len(mismatch)} different")
for m in mismatch:
    print("  MISMATCH", m)

# (3) the ESL copy
out_plugin = os.path.join(OUT, "SkillsOfTheWild.esp")
if os.path.exists(out_plugin):
    os.remove(out_plugin)
tmp = os.path.join(W, "deser")
shutil.rmtree(tmp, ignore_errors=True)
os.makedirs(tmp)
spriggit("deserialize", "--InputPath", y_map, "--OutputPath", os.path.join(tmp, "SkillsOfTheWild.esp"))
shutil.copyfile(os.path.join(tmp, "SkillsOfTheWild.esp"), out_plugin)

# (4) round trip
y_back = os.path.join(W, "y_back")
serialize(out_plugin, y_back)
ok_round = norm_tree(y_back) == norm_tree(y_map)
# and the original round trips to itself, so any difference above is ours: everything but the Campfire numbers must be identical to the original
a, b = norm_tree(y_orig), norm_tree(y_back)
diff_files = [k for k in set(a) | set(b) if a.get(k) != b.get(k)]
print(f"round trip ok: {ok_round}; files that differ from the original: {len(diff_files)} (the ones with Campfire references: {sum(1 for k in a if any(ref.search(l) for l in a[k]))})")
changed_ok = True
for k in diff_files:
    # the only lines that may differ are the ones carrying a Campfire reference
    la = [l for l in a.get(k, []) if not ref.search(l)]
    lb = [l for l in b.get(k, []) if not ref.search(l) and not re.search(r"\b[0-9A-F]{6}:Campfire\.esm", l)]
    if sorted(la) != sorted(lb):
        # compare after mapping the originals' Campfire lines the same way
        changed_ok = False
        print("  DIFFERS BEYOND CAMPFIRE REFERENCES:", k)
print("ESL plugin:", out_plugin, os.path.getsize(out_plugin), "bytes; only Campfire references changed:", changed_ok)
sys.exit(0 if (ok_round and not mismatch and changed_ok) else 1)
