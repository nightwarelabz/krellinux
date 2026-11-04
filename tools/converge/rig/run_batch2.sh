#!/bin/sh
# Differential rig for Krellinux Phase 1 batch 2 (CRC family + muldi3).
# Builds the kernel C originals from this tree and the Krell conversions
# from lib/crc/*.krell and lib/muldi3.krell, links them against
# difftest_batch2.c, and runs the corpus. Exit 0 = exact agreement.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TREE=${KRELLINUX_TREE:-$(cd "$HERE/../../.." && pwd)}
KRELLC=${KRELLC:-krellc}
B="$HERE/build_batch2"
mkdir -p "$B"

# --- Krell side: parse, verify, build, rename symbols to krell_* ---
build_krell() { # $1 = source stem, $2 = tree path, $3.. = sed expressions
    src=$1; path=$2; shift 2
    "$KRELLC" parse "$TREE/$path.krell" -o "$B/$src.krel"
    "$KRELLC" verify "$B/$src.krel"
    "$KRELLC" build "$B/$src.krel" --target x86_64 -o "$B/$src.s.tmp"
    sed "$@" "$B/$src.s.tmp" > "$B/$src.s"
    rm "$B/$src.s.tmp"
    gcc -c -o "$B/$src.o" "$B/$src.s"
}
build_krell crc-ccitt lib/crc/crc-ccitt -e 's/\bcrc_ccitt\b/krell_crc_ccitt/g'
build_krell crc-itu-t lib/crc/crc-itu-t -e 's/\bcrc_itu_t\b/krell_crc_itu_t/g'
build_krell crc4 lib/crc/crc4 -e 's/\bcrc4\b/krell_crc4/g'
build_krell crc7 lib/crc/crc7 -e 's/\bcrc7_be\b/krell_crc7_be/g'
build_krell crc8 lib/crc/crc8 \
    -e 's/\bcrc8_populate_msb\b/krell_crc8_populate_msb/g' \
    -e 's/\bcrc8_populate_lsb\b/krell_crc8_populate_lsb/g' \
    -e 's/\bcrc8\b/krell_crc8/g'
build_krell muldi3 lib/muldi3 -e 's/\b__muldi3\b/krell___muldi3/g'

# --- C side: kernel originals as-is, symbols renamed to *_c ---
gcc -O2 -I "$HERE/shims_batch2" -c "$TREE/lib/crc/crc-ccitt.c" -o "$B/crc-ccitt_c.o"
objcopy --redefine-sym crc_ccitt=crc_ccitt_c "$B/crc-ccitt_c.o"
gcc -O2 -I "$HERE/shims_batch2" -c "$TREE/lib/crc/crc-itu-t.c" -o "$B/crc-itu-t_c.o"
objcopy --redefine-sym crc_itu_t=crc_itu_t_c "$B/crc-itu-t_c.o"
gcc -O2 -I "$HERE/shims_batch2" -c "$TREE/lib/crc/crc4.c" -o "$B/crc4_c.o"
objcopy --redefine-sym crc4=crc4_c "$B/crc4_c.o"
gcc -O2 -I "$HERE/shims_batch2" -c "$TREE/lib/crc/crc7.c" -o "$B/crc7_c.o"
objcopy --redefine-sym crc7_be=crc7_be_c "$B/crc7_c.o"
gcc -O2 -I "$HERE/shims_batch2" -c "$TREE/lib/crc/crc8.c" -o "$B/crc8_c.o"
objcopy --redefine-sym crc8=crc8_c \
        --redefine-sym crc8_populate_msb=crc8_populate_msb_c \
        --redefine-sym crc8_populate_lsb=crc8_populate_lsb_c "$B/crc8_c.o"
gcc -O2 -I "$HERE/shims_batch2" -c "$TREE/lib/muldi3.c" -o "$B/muldi3_c.o"
objcopy --redefine-sym __muldi3=__muldi3_c "$B/muldi3_c.o"

gcc -O2 -o "$B/difftest_batch2" "$HERE/difftest_batch2.c" \
    "$B"/crc-ccitt.o "$B"/crc-itu-t.o "$B"/crc4.o "$B"/crc7.o "$B"/crc8.o "$B"/muldi3.o \
    "$B"/crc-ccitt_c.o "$B"/crc-itu-t_c.o "$B"/crc4_c.o "$B"/crc7_c.o "$B"/crc8_c.o "$B"/muldi3_c.o

"$B/difftest_batch2"
