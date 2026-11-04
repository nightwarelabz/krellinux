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
    "lib/kstrtox.c": (
        "KRELL (functions)",
        "17 of 28 functions rewritten in Krell (lib/kstrtox.krell): "
        "_parse_integer_fixup_radix, _parse_integer_limit, the "
        "kstrtoull/kstrtoll family and its width wrappers, "
        "kstrtobool, and the kstrtoudec64/kstrtodec64 scaled "
        "decimals; differential rig vs the C original: part of the "
        "batch 4 rig (215,447 cases across the batch, 0 divergence); "
        "Krell objects linked into vmlinux and self-checked at boot "
        "(Phase 1 batch 4). Residue: the 11 *_from_user wrappers "
        "stay C -- they stage the user string in a stack buffer of "
        "11 to 67 bytes, and the current Krell native backend "
        "addresses an alloca region from its top, so writes past "
        "the first 8 bytes of a buffer overwrite the frame (batch 4 "
        "record has the reproducer); the wrappers convert when a "
        "compiler with the alloca fix is pinned. The C file still "
        "provides the symbols."),
    "lib/cmdline.c": (
        "KRELL (functions)",
        "get_option, get_options, memparse, parse_option_str and "
        "next_arg rewritten in Krell (lib/cmdline.krell), with the "
        "file-local get_range as an internal helper; differential "
        "rig vs the C original: part of the batch 4 rig (215,447 "
        "cases across the batch, 0 divergence); Krell objects "
        "linked into vmlinux and self-checked at boot (Phase 1 "
        "batch 4). simple_strtoull/simple_strtol, skip_spaces and "
        "strlen/strncmp are called externally, as in C. The C file "
        "still provides the symbols."),
    "lib/bsearch.c": (
        "KRELL (function)",
        "bsearch() rewritten in Krell (lib/bsearch.krell): the C "
        "file is a wrapper over __inline_bsearch() from "
        "include/linux/bsearch.h, and the Krell form is that "
        "algorithm, with the comparator reached through an "
        "indirect call on its C signature (ptr, ptr) -> int; "
        "differential rig vs the header algorithm: part of the "
        "batch 4 rig (215,447 cases across the batch, 0 "
        "divergence), including a comparator with poisoned upper "
        "return bits; Krell object linked into vmlinux and "
        "self-checked at boot (Phase 1 batch 4). The C file still "
        "provides the symbol."),
    "lib/string.c": (
        "KRELL (functions)",
        "33 of 34 functions rewritten in Krell (lib/string.krell): "
        "the full string/memory set (strlen/strnlen, the copy and "
        "compare families, the search family, strspn/strcspn/strpbrk, "
        "strsep, memset/memset16/32/64, memcpy/memmove, memcmp/bcmp, "
        "memscan/memchr/memchr_inv, strstr/strnstr, sized_strscpy); "
        "differential rig vs the C originals: 92,899 cases, 0 "
        "divergence, including all 256x256 single-byte pairs for the "
        "case functions against the real _ctype table (its Latin-1 "
        "upper ranges are mirrored); Krell objects linked into "
        "vmlinux and self-checked at boot (Phase 1 batch 3). "
        "Residue: strlcat() stays C -- its BUG_ON() trap on a caller "
        "contract violation has no Krell counterpart. Notes: "
        "sized_strscpy() past-the-NUL dest bytes are unspecified in "
        "C (word-at-a-time may zero some) and left untouched by the "
        "Krell form; its WARN_ON_ONCE for count > INT_MAX is a log "
        "line, not a result. The C file still provides the symbols."),
    "lib/muldi3.c": (
        "KRELL (function)",
        "__muldi3() rewritten in Krell (lib/muldi3.krell); differential "
        "rig vs the C original: 100,225 cases (structured cross-product "
        "plus random pairs), 0 divergence, and the same corpus checked "
        "against a 128-bit oracle; Krell object linked into vmlinux and "
        "self-checked at boot (Phase 1 batch 2). The C file still "
        "provides the symbol where configured."),
}

# lib/crc/ sits below the census's top-level lib/ rows; converted
# files there are recorded as extra rows until the census extends.
EXTRA_ROWS = [
    ("lib/crc/crc16.c", "KRELL (function)", "-",
     "crc16() rewritten in Krell (lib/crc/crc16.krell); differential "
     "rig vs the C original: 3,563 cases, 0 divergence; Krell object "
     "linked into vmlinux and self-checked at boot (Phase 0 spike). "
     "The C file still provides the symbol; replacement is Phase 1."),
    ("lib/crc/crc-ccitt.c", "KRELL (function)", "-",
     "crc_ccitt() rewritten in Krell (lib/crc/crc-ccitt.krell, "
     "bitwise reflected form, polynomial 0x8408); differential rig vs "
     "the C original: 3,565 cases, 0 divergence; Krell object linked "
     "into vmlinux and self-checked at boot (Phase 1 batch 2). The "
     "table data and the C function remain in the C file."),
    ("lib/crc/crc-itu-t.c", "KRELL (function)", "-",
     "crc_itu_t() rewritten in Krell (lib/crc/crc-itu-t.krell, "
     "bitwise MSB-first form, polynomial 0x1021); differential rig vs "
     "the C original: 3,565 cases, 0 divergence; Krell object linked "
     "into vmlinux and self-checked at boot (Phase 1 batch 2). The "
     "table data and the C function remain in the C file."),
    ("lib/crc/crc4.c", "KRELL (function)", "-",
     "crc4() rewritten in Krell (lib/crc/crc4.krell, table step "
     "performed as four bit steps); differential rig vs the C "
     "original: 62,048 cases (every bit count 0..63 in the defined "
     "domain), 0 divergence; Krell object linked into vmlinux and "
     "self-checked at boot (Phase 1 batch 2)."),
    ("lib/crc/crc7.c", "KRELL (function)", "-",
     "crc7_be() rewritten in Krell (lib/crc/crc7.krell, bitwise "
     "MSB-first form, polynomial x^7 + x^3 + 1); differential rig vs "
     "the C original: 3,060 cases, 0 divergence; Krell object linked "
     "into vmlinux and self-checked at boot (Phase 1 batch 2). The "
     "table data and the C function remain in the C file."),
    ("lib/crc/crc8.c", "KRELL (functions)", "-",
     "crc8(), crc8_populate_msb() and crc8_populate_lsb() rewritten "
     "in Krell (lib/crc/crc8.krell, table-generic structure kept); "
     "differential rig vs the C originals: 8,020 cases including "
     "full 256-byte table comparisons for ten polynomials, 0 "
     "divergence; Krell object linked into vmlinux and self-checked "
     "at boot (Phase 1 batch 2)."),
    ("lib/math/gcd.c", "KRELL (function)", "-",
     "gcd() rewritten in Krell (lib/math/gcd.krell), following "
     "the binary GCD the C file selects on its static key (the "
     "fallback loop computes the same gcd; the choice is "
     "result-invariant); differential rig vs the C original: "
     "part of the batch 4 rig (215,447 cases across the batch, "
     "0 divergence); Krell object linked into vmlinux and "
     "self-checked at boot (Phase 1 batch 4). The C file still "
     "provides the symbol."),
    ("lib/math/lcm.c", "KRELL (functions)", "-",
     "lcm() and lcm_not_zero() rewritten in Krell "
     "(lib/math/lcm.krell), calling the Krell gcd; differential "
     "rig vs the C originals: part of the batch 4 rig (215,447 "
     "cases across the batch, 0 divergence); Krell objects "
     "linked into vmlinux and self-checked at boot (Phase 1 "
     "batch 4). The C file still provides the symbols."),
    ("lib/math/int_pow.c", "KRELL (function)", "-",
     "int_pow() rewritten in Krell (lib/math/int_pow.krell); "
     "differential rig vs the C original: part of the batch 4 "
     "rig (215,447 cases across the batch, 0 divergence); "
     "Krell object linked into vmlinux and self-checked at "
     "boot (Phase 1 batch 4). The C file still provides the "
     "symbol."),
    ("lib/math/int_sqrt.c", "KRELL (function)", "-",
     "int_sqrt() rewritten in Krell (lib/math/int_sqrt.krell), "
     "the C file's shift-and-subtract algorithm operation for "
     "operation; differential rig vs the C original: part of "
     "the batch 4 rig (215,447 cases across the batch, 0 "
     "divergence), including the full 0..100000 range; Krell "
     "object linked into vmlinux and self-checked at boot "
     "(Phase 1 batch 4). int_sqrt64() is compiled only when "
     "BITS_PER_LONG < 64 and has no x86_64 instance. The C "
     "file still provides the symbol."),
]

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
    for f, state, cls, note in EXTRA_ROWS:
        lines.append(f"| `{f}` | {state} | {cls} | {note} |")
    lines.append("")
    lines.append("Directories beyond lib/ enter the ledger as their "
                 "phase's census completes.")
    out = os.path.join(tree, "CONVERSION.md")
    with open(out, "w") as fh:
        fh.write("\n".join(lines) + "\n")
    print("wrote", out, f"({len(rows)} files)")

if __name__ == "__main__":
    main()
