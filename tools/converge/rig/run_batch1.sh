#!/bin/sh
# Differential rig for Krellinux Phase 1 batch 1 (libgcc helpers + bcd).
# Builds the kernel C originals from this tree and the Krell conversions
# from lib/*.krell, links them against difftest_batch1.c, and runs the
# corpus. Exit 0 = exact agreement on every case.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TREE=${KRELLINUX_TREE:-$(cd "$HERE/../../.." && pwd)}
KRELLC=${KRELLC:-krellc}
B="$HERE/build_batch1"
mkdir -p "$B"

# --- Krell side: parse, verify, build, rename symbols to krell_* ---
for f in ashldi3 ashrdi3 lshrdi3 cmpdi2 ucmpdi2 bcd; do
    "$KRELLC" parse "$TREE/lib/$f.krell" -o "$B/$f.krel"
    "$KRELLC" verify "$B/$f.krel"
    "$KRELLC" build "$B/$f.krel" --target x86_64 -o "$B/$f.s.tmp"
    case $f in
        bcd) sed -e 's/\b_bcd2bin\b/krell__bcd2bin/g' \
                 -e 's/\b_bin2bcd\b/krell__bin2bcd/g' "$B/$f.s.tmp" > "$B/$f.s" ;;
        *)   sed "s/\b__$f\b/krell___$f/g" "$B/$f.s.tmp" > "$B/$f.s" ;;
    esac
    rm "$B/$f.s.tmp"
    gcc -c -o "$B/$f.o" "$B/$f.s"
done

# --- C side: kernel originals as-is, symbols renamed to *_c ---
for f in ashldi3 ashrdi3 lshrdi3 cmpdi2 ucmpdi2; do
    gcc -O2 -I "$HERE/shims_batch1" -c "$TREE/lib/$f.c" -o "$B/${f}_c.o"
    objcopy --redefine-sym "__$f=__${f}_c" "$B/${f}_c.o"
done
gcc -O2 -I "$HERE/shims_batch1" -c "$TREE/lib/bcd.c" -o "$B/bcd_c.o"
objcopy --redefine-sym _bcd2bin=_bcd2bin_c --redefine-sym _bin2bcd=_bin2bcd_c "$B/bcd_c.o"

gcc -O2 -o "$B/difftest_batch1" "$HERE/difftest_batch1.c" \
    "$B"/ashldi3.o "$B"/ashrdi3.o "$B"/lshrdi3.o "$B"/cmpdi2.o "$B"/ucmpdi2.o "$B"/bcd.o \
    "$B"/ashldi3_c.o "$B"/ashrdi3_c.o "$B"/lshrdi3_c.o "$B"/cmpdi2_c.o "$B"/ucmpdi2_c.o "$B"/bcd_c.o

"$B/difftest_batch1"
