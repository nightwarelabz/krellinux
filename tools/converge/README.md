# Auto-Convergence Engine

Tooling for the Krellinux conversion: it censuses C code against the
Krell capability table, converts what it can, and proves every
converted function equal to its C original on a differential corpus
before the conversion is integrated. It never commits, pushes, or
lands anything; humans review its output like any other patch.

## v0 -- census (tools/converge/census.py)

Parses each C file with libclang under its real Kbuild compile flags
(from compile_commands.json) and classifies it:

- AUTO -- only constructs Krell expresses directly.
- ASSIST -- blocked by named gaps (indirect calls, cmpxchg-family
  atomics, volatile, varargs, bitfields, i128, inline asm).
- RESIDUE -- stays C/asm (file-scope asm, clang parse failure).

    python3 scripts/clang-tools/gen_compile_commands.py
    python3 tools/converge/census.py <tree-root> lib

Writes tools/converge/census-lib.json and CENSUS-lib.md (the blocker
histogram is the compiler work program, in the tree's own order).

## Differential rig (tools/converge/rig/)

Per-function proof harness. difftest.c is the crc16 instance: the
kernel's own lib/crc/crc16.c compiled as-is (symbol renamed) against
crc16.krell through krellc, over a randomized corpus with known
vectors, boundary lengths, and chained/split-stream cases.
Agreement is exact or the conversion does not integrate.

## Ledger (tools/converge/gen_ledger.py)

Regenerates CONVERSION.md at the tree root from census data plus the
recorded conversion evidence.
