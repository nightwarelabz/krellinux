#!/bin/bash
# Krellinux Phase 1 batch 7 differential rig (in-tree copy).
# Compares the Krell forms of lib/btree.c and lib/assoc_array.c
# against the C originals.  Run from tools/converge/rig/:
#   ./run_batch7.sh [path-to-krellc]
# Prints the case counts and MATCH on success; any divergence
# aborts with detail.
set -e
cd "$(dirname "$0")"
KRELLC=${1:-$HOME/workspace/krellinux-toolchain/krellc}
TREE=$(cd ../../.. && pwd)
SRC=$TREE/lib
B=build_batch7
rm -rf $B
mkdir -p $B

CFLAGS_B="-O1 -g -Wall -Wextra -Werror=implicit-function-declaration -I shims_batch7_b -I $TREE/include"
CFLAGS_A="-O1 -g -Wall -Wextra -Werror=implicit-function-declaration -DCONFIG_ASSOCIATIVE_ARRAY=1 -I shims_batch7_a -I $TREE/include"

# ---- btree ----
$KRELLC parse "$SRC/btree.krell" -o $B/btree.krel
$KRELLC verify $B/btree.krel
$KRELLC build $B/btree.krel --target x86_64 -o $B/btree_krell.s
gcc -c $B/btree_krell.s -o $B/btree_krell_raw.o
cat > $B/rename_b.map <<'EOF'
btree_init_mempool krell_btree_init_mempool
btree_init krell_btree_init
btree_destroy krell_btree_destroy
btree_last krell_btree_last
btree_lookup krell_btree_lookup
btree_update krell_btree_update
btree_get_prev krell_btree_get_prev
btree_insert krell_btree_insert
btree_remove krell_btree_remove
btree_merge krell_btree_merge
visitorl krell_visitorl
visitor32 krell_visitor32
visitor64 krell_visitor64
visitor128 krell_visitor128
btree_visitor krell_btree_visitor
btree_grim_visitor krell_btree_grim_visitor
EOF
objcopy --redefine-syms=$B/rename_b.map $B/btree_krell_raw.o $B/btree_krell.o
gcc $CFLAGS_B -c "$SRC/btree.c" -o $B/btree_c.o
gcc $CFLAGS_B difftest_batch7_btree.c $B/btree_c.o $B/btree_krell.o -o $B/difftest_batch7_btree
$B/difftest_batch7_btree

# ---- assoc_array ----
$KRELLC parse "$SRC/assoc_array.krell" -o $B/assoc.krel
$KRELLC verify $B/assoc.krel
$KRELLC build $B/assoc.krel --target x86_64 -o $B/assoc_krell.s
gcc -c $B/assoc_krell.s -o $B/assoc_krell_raw.o
cat > $B/rename_a.map <<'EOF'
assoc_array_iterate krell_assoc_array_iterate
assoc_array_find krell_assoc_array_find
assoc_array_destroy krell_assoc_array_destroy
assoc_array_insert krell_assoc_array_insert
assoc_array_insert_set_object krell_assoc_array_insert_set_object
assoc_array_delete krell_assoc_array_delete
assoc_array_clear krell_assoc_array_clear
assoc_array_apply_edit krell_assoc_array_apply_edit
assoc_array_cancel_edit krell_assoc_array_cancel_edit
assoc_array_gc krell_assoc_array_gc
EOF
objcopy --redefine-syms=$B/rename_a.map $B/assoc_krell_raw.o $B/assoc_krell.o
gcc $CFLAGS_A -c "$SRC/assoc_array.c" -o $B/assoc_c.o
gcc $CFLAGS_A difftest_batch7_assoc.c $B/assoc_c.o $B/assoc_krell.o -o $B/difftest_batch7_assoc
$B/difftest_batch7_assoc
