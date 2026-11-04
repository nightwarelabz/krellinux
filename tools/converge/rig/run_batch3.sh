#!/bin/sh
# Differential rig for Krellinux Phase 1 batch 3 (lib/string.c).
# Builds the kernel C originals from this tree (with the real
# lib/ctype.c table) and the Krell conversions from lib/string.krell,
# links them against difftest_batch3.c, and runs the corpus.
# Exit 0 = exact agreement.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TREE=${KRELLINUX_TREE:-$(cd "$HERE/../../.." && pwd)}
KRELLC=${KRELLC:-$HOME/workspace/krellinux-toolchain/krellc}
B="$HERE/build_batch3"
mkdir -p "$B"

NAMES="strncasecmp strcasecmp strcpy sized_strscpy stpcpy strcat strncat
strlcat strcmp strncmp strchr strchrnul strnchrnul strrchr strnchr strlen
strnlen strspn strcspn strpbrk strsep memset memset16 memset32 memset64
memcpy memmove memcmp bcmp memscan strstr strnstr memchr memchr_inv"

# --- Krell side: parse, verify, build, rename symbols to krell_* ---
"$KRELLC" parse "$TREE/lib/string.krell" -o "$B/string.krel"
"$KRELLC" verify "$B/string.krel"
"$KRELLC" build "$B/string.krel" --target x86_64 -o "$B/string.s.tmp"
SEDARGS=""
for n in $NAMES; do
    [ "$n" = strlcat ] && continue
    SEDARGS="$SEDARGS -e s/\\b$n\\b/krell_$n/g"
done
# shellcheck disable=SC2086
sed $SEDARGS "$B/string.s.tmp" > "$B/string.s"
rm "$B/string.s.tmp"
gcc -c -o "$B/string_krell.o" "$B/string.s"

# --- C side: kernel originals as-is, symbols renamed to *_c ---
gcc -O2 -fno-builtin -fno-stack-protector -I "$HERE/shims_batch3" \
    -c "$TREE/lib/string.c" -o "$B/string_c.o"
gcc -O2 -fno-builtin -fno-stack-protector -I "$HERE/shims_batch3" \
    -c "$TREE/lib/ctype.c" -o "$B/ctype_c.o"
REDEFS=""
for n in $NAMES; do REDEFS="$REDEFS --redefine-sym $n=${n}_c"; done
# shellcheck disable=SC2086
objcopy $REDEFS "$B/string_c.o"

gcc -O2 -o "$B/difftest_batch3" "$HERE/difftest_batch3.c" \
    "$B/string_krell.o" "$B/string_c.o" "$B/ctype_c.o"

"$B/difftest_batch3"
