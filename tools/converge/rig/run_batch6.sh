#!/bin/bash
# Krellinux Phase 1 batch 6 differential rig (in-tree copy).
# Compares the Krell forms of lib/sort.c and lib/list_sort.c
# against the C originals.  Run from tools/converge/rig/:
#   ./run_batch6.sh [path-to-krellc]
set -e
cd "$(dirname "$0")"
KRELLC=${1:-$HOME/workspace/krellinux-toolchain/krellc}
TREE=$(cd ../../.. && pwd)
SRC=$TREE/lib
B=build_batch6
mkdir -p $B
CFLAGS="-O1 -I shims_batch6 -Wall -Wextra -Werror=implicit-function-declaration"

gcc $CFLAGS -c "$SRC/sort.c" -o $B/sort_c.o
gcc $CFLAGS -c "$SRC/list_sort.c" -o $B/list_sort_c.o

for m in sort list_sort; do
	$KRELLC parse "$SRC/$m.krell" -o $B/$m.krel
	$KRELLC verify $B/$m.krel
	$KRELLC build $B/$m.krel --target x86_64 -o $B/$m.s
	sed -e 's/\bsort_r_nonatomic\b/krell_sort_r_nonatomic/g' \
	    -e 's/\bsort_r\b/krell_sort_r/g' \
	    -e 's/\bsort_nonatomic\b/krell_sort_nonatomic/g' \
	    -e 's/\bsort\b/krell_sort/g' \
	    -e 's/\blist_sort\b/krell_list_sort/g' \
	    $B/$m.s > $B/${m}_krell.s
	gcc -c $B/${m}_krell.s -o $B/${m}_krell.o
done

gcc $CFLAGS difftest_batch6.c $B/sort_c.o $B/list_sort_c.o \
	$B/sort_krell.o $B/list_sort_krell.o -o $B/difftest_batch6
$B/difftest_batch6
