#!/usr/bin/env python3
"""Generate CONVERSION.md, the Krellinux conversion ledger, from the
engine's census data. One row per file: state, conversion class, and
the evidence note. States: C (untouched), CONVERTING, KRELL (proven
by the differential rig), RESIDUE (named, stays C/asm)."""
import json, os, sys, datetime

OVERRIDES = {
    "lib/ashldi3.c": (
        "KRELL (function)",
        "__ashldi3() rewritten in Krell (lib/ashldi3.krell); differential "
        "rig vs the C original: 65,184 cases (every shift count 0..63 x "
        "structured and random operands), 0 divergence; Krell object "
        "linked into vmlinux and self-checked at boot (Phase 1 batch 1). "
        "The C file still provides the symbol where configured."),
    "lib/ashrdi3.c": (
        "KRELL (function)",
        "__ashrdi3() rewritten in Krell (lib/ashrdi3.krell); differential "
        "rig vs the C original: 65,184 cases (every shift count 0..63 x "
        "structured and random operands), 0 divergence; Krell object "
        "linked into vmlinux and self-checked at boot (Phase 1 batch 1). "
        "The C file still provides the symbol where configured."),
    "lib/lshrdi3.c": (
        "KRELL (function)",
        "__lshrdi3() rewritten in Krell (lib/lshrdi3.krell); differential "
        "rig vs the C original: 65,184 cases (every shift count 0..63 x "
        "structured and random operands), 0 divergence; Krell object "
        "linked into vmlinux and self-checked at boot (Phase 1 batch 1). "
        "The C file still provides the symbol where configured."),
    "lib/cmpdi2.c": (
        "KRELL (function)",
        "__cmpdi2() rewritten in Krell (lib/cmpdi2.krell); differential "
        "rig vs the C original: 60,225 cases (structured cross-product "
        "plus random and near-equal pairs), 0 divergence; Krell object "
        "linked into vmlinux and self-checked at boot (Phase 1 batch 1). "
        "The C file still provides the symbol where configured."),
    "lib/ucmpdi2.c": (
        "KRELL (function)",
        "__ucmpdi2() rewritten in Krell (lib/ucmpdi2.krell); "
        "differential rig vs the C original: 60,225 cases (structured "
        "cross-product plus random and near-equal pairs), 0 divergence; "
        "Krell object linked into vmlinux and self-checked at boot "
        "(Phase 1 batch 1). The C file still provides the symbol where "
        "configured."),
    "lib/bcd.c": (
        "KRELL (functions)",
        "_bcd2bin() and _bin2bcd() rewritten in Krell (lib/bcd.krell); "
        "differential rig vs the C original: 160,256 cases (_bcd2bin "
        "exhaustive over all 256 inputs; _bin2bcd exhaustive 0..99999 "
        "plus random full-range values), 0 divergence; Krell object "
        "linked into vmlinux and self-checked at boot (Phase 1 batch 1). "
        "The C file still provides the symbols."),
    "lib/crc/crc16.c": (
        "KRELL (function)",
        "crc16() rewritten in Krell (lib/crc/crc16.krell); differential "
        "rig vs the C original: 3,563 cases, 0 divergence; Krell object "
        "linked into vmlinux and self-checked at boot (Phase 0 spike). "
        "The C file still provides the symbol; replacement is Phase 1."),
}

def main():
    tree = os.path.abspath(sys.argv[1])
    census_path = os.path.join(tree, "tools/converge/census-lib.json")
    data = json.load(open(census_path))
    rows = data["files"]
    lines = []
    lines.append("# Krellinux conversion ledger")
    lines.append("")
    lines.append(f"Generated {datetime.date.today().isoformat()} by the "
                 "Auto-Convergence Engine (gen_ledger.py) from census data. "
                 "Base: Linux 7.4.0-rc1 (05441d69), fork master 2c3d62bc.")
    lines.append("")
    lines.append("States: **C** untouched - **CONVERTING** in progress - "
                 "**KRELL** converted and proven by the differential rig - "
                 "**RESIDUE** named, stays C/asm by decision.")
    lines.append("")
    lines.append(f"Census of lib/: {data['totals']} (AUTO / ASSIST / RESIDUE "
                 "are the engine's conversion classes, not states).")
    lines.append("")
    lines.append("| File | State | Class | Evidence / blockers |")
    lines.append("|---|---|---|---|")
    for r in rows:
        state, note = OVERRIDES.get(r["file"], ("C", r["blockers"] or "-"))
        lines.append(f"| `{r['file']}` | {state} | {r['class']} | {note} |")
    lines.append("")
    lines.append("Directories beyond lib/ enter the ledger as their "
                 "phase's census completes.")
    out = os.path.join(tree, "CONVERSION.md")
    with open(out, "w") as fh:
        fh.write("\n".join(lines) + "\n")
    print("wrote", out, f"({len(rows)} files)")

if __name__ == "__main__":
    main()
