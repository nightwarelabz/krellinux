#!/bin/bash
# Krellinux Phase 1 batch 5 differential rig (in-tree copy).
# Compares the Krell forms of lib/find_bit.c, lib/bitmap.c and
# lib/bitmap-str.c against the C originals.  Run from
# tools/converge/rig/:
#   ./run_batch5.sh [path-to-krellc]
set -e
cd "$(dirname "$0")"
KRELLC=${1:-$HOME/workspace/krellinux-toolchain/krellc}
TREE=$(cd ../../.. && pwd)
SRC=$TREE/lib
B=build_batch5
mkdir -p $B
SHIM="-I shims_batch5 -I $TREE/lib"
SHIM4="-I shims_batch4 -I $TREE/lib"

FB_RENAME=(
	-e 's/\b__find_nth_and_andnot_bit\b/krell___find_nth_and_andnot_bit/g'
	-e 's/\b__find_nth_and_bit\b/krell___find_nth_and_bit/g'
	-e 's/\b__find_nth_bit\b/krell___find_nth_bit/g'
	-e 's/\b_find_first_and_and_bit\b/krell__find_first_and_and_bit/g'
	-e 's/\b_find_first_andnot_bit\b/krell__find_first_andnot_bit/g'
	-e 's/\b_find_first_and_bit\b/krell__find_first_and_bit/g'
	-e 's/\b_find_first_zero_bit\b/krell__find_first_zero_bit/g'
	-e 's/\b_find_first_bit\b/krell__find_first_bit/g'
	-e 's/\b_find_next_andnot_bit\b/krell__find_next_andnot_bit/g'
	-e 's/\b_find_next_and_bit\b/krell__find_next_and_bit/g'
	-e 's/\b_find_next_or_bit\b/krell__find_next_or_bit/g'
	-e 's/\b_find_next_zero_bit\b/krell__find_next_zero_bit/g'
	-e 's/\b_find_next_bit\b/krell__find_next_bit/g'
	-e 's/\b_find_last_bit\b/krell__find_last_bit/g'
	-e 's/\bfind_next_clump8\b/krell_find_next_clump8/g'
	-e 's/\bfind_random_bit\b/krell_find_random_bit/g'
	-e 's/\b__bitmap_weight\b/krell___bitmap_weight/g'
)
BM_RENAME=(
	-e 's/\b__bitmap_weighted_or\b/krell___bitmap_weighted_or/g'
	-e 's/\b__bitmap_weighted_xor\b/krell___bitmap_weighted_xor/g'
	-e 's/\b__bitmap_weight_andnot\b/krell___bitmap_weight_andnot/g'
	-e 's/\b__bitmap_weight_and\b/krell___bitmap_weight_and/g'
	-e 's/\b__bitmap_weight\b/krell___bitmap_weight/g'
	-e 's/\b__bitmap_intersects_and\b/krell___bitmap_intersects_and/g'
	-e 's/\b__bitmap_intersects\b/krell___bitmap_intersects/g'
	-e 's/\b__bitmap_shift_right\b/krell___bitmap_shift_right/g'
	-e 's/\b__bitmap_shift_left\b/krell___bitmap_shift_left/g'
	-e 's/\b__bitmap_complement\b/krell___bitmap_complement/g'
	-e 's/\b__bitmap_or_equal\b/krell___bitmap_or_equal/g'
	-e 's/\b__bitmap_equal\b/krell___bitmap_equal/g'
	-e 's/\b__bitmap_andnot\b/krell___bitmap_andnot/g'
	-e 's/\b__bitmap_and\b/krell___bitmap_and/g'
	-e 's/\b__bitmap_or\b/krell___bitmap_or/g'
	-e 's/\b__bitmap_xor\b/krell___bitmap_xor/g'
	-e 's/\b__bitmap_replace\b/krell___bitmap_replace/g'
	-e 's/\b__bitmap_subset\b/krell___bitmap_subset/g'
	-e 's/\b__bitmap_set\b/krell___bitmap_set/g'
	-e 's/\b__bitmap_clear\b/krell___bitmap_clear/g'
	-e 's/\bbitmap_find_next_zero_area_off\b/krell_bitmap_find_next_zero_area_off/g'
	-e 's/\bbitmap_cut\b/krell_bitmap_cut/g'
	-e 's/\bbitmap_remap\b/krell_bitmap_remap/g'
	-e 's/\bbitmap_bitremap\b/krell_bitmap_bitremap/g'
	-e 's/\bbitmap_onto\b/krell_bitmap_onto/g'
	-e 's/\bbitmap_fold\b/krell_bitmap_fold/g'
	-e 's/\bbitmap_alloc_node\b/krell_bitmap_alloc_node/g'
	-e 's/\bbitmap_zalloc_node\b/krell_bitmap_zalloc_node/g'
	-e 's/\bbitmap_alloc\b/krell_bitmap_alloc/g'
	-e 's/\bbitmap_zalloc\b/krell_bitmap_zalloc/g'
	-e 's/\bbitmap_free\b/krell_bitmap_free/g'
	-e 's/\bbitmap_from_arr32\b/krell_bitmap_from_arr32/g'
	-e 's/\bbitmap_to_arr32\b/krell_bitmap_to_arr32/g'
	"${FB_RENAME[@]}"
)
BS_RENAME=(
	-e 's/\bbitmap_parselist_user\b/krell_bitmap_parselist_user/g'
	-e 's/\bbitmap_parse_user\b/krell_bitmap_parse_user/g'
	-e 's/\bbitmap_parselist\b/krell_bitmap_parselist/g'
	-e 's/\bbitmap_parse\b/krell_bitmap_parse/g'
	-e 's/\bbitmap_print_bitmask_to_buf\b/krell_bitmap_print_bitmask_to_buf/g'
	-e 's/\bbitmap_print_list_to_buf\b/krell_bitmap_print_list_to_buf/g'
	-e 's/\b__bitmap_set\b/krell___bitmap_set/g'
	-e 's/\b__bitmap_clear\b/krell___bitmap_clear/g'
	-e 's/\b_find_next_bit\b/krell__find_next_bit/g'
)

build_krell() { # src-file, name, renames...
	local srcf=$1; local name=$2; shift 2
	$KRELLC parse "$srcf" -o $B/$name.krel
	$KRELLC verify $B/$name.krel > /dev/null
	$KRELLC build $B/$name.krel --target x86_64 -o $B/$name.s
	sed -i "$@" $B/$name.s
	gcc -c -o $B/${name}_krell.o $B/$name.s
}

build_krell $SRC/find_bit.krell find_bit "${FB_RENAME[@]}"
build_krell $SRC/bitmap.krell bitmap "${BM_RENAME[@]}"
build_krell $SRC/bitmap-str.krell bitmap_str "${BS_RENAME[@]}"

gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/find_bit.c -o $B/find_bit_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w -DCONFIG_NUMA $SHIM -c $TREE/lib/bitmap.c -o $B/bitmap_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/bitmap-str.c -o $B/bitmap_str_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/ctype.c -o $B/ctype_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM4 -c $TREE/lib/kstrtox.c -o $B/kstrtox_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM4 -c $TREE/lib/math/int_pow.c -o $B/int_pow_c.o

gcc -O2 -o $B/difftest difftest_batch5.c \
	$B/find_bit_krell.o $B/bitmap_krell.o $B/bitmap_str_krell.o \
	$B/find_bit_c.o $B/bitmap_c.o $B/bitmap_str_c.o \
	$B/ctype_c.o $B/kstrtox_c.o $B/int_pow_c.o
$B/difftest
