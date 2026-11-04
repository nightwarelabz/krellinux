#!/bin/bash
# Krellinux Phase 1 batch 4 differential rig (in-tree copy).
# Compares the Krell forms of lib/math/{gcd,lcm,int_pow,int_sqrt}.c,
# lib/bsearch.c, lib/kstrtox.c and lib/cmdline.c against the C
# originals.  Run from tools/converge/rig/:
#   ./run_batch4.sh [path-to-krellc]
set -e
cd "$(dirname "$0")"
KRELLC=${1:-$HOME/workspace/krellinux-toolchain/krellc}
TREE=$(cd ../../.. && pwd)
B=build_batch4
mkdir -p $B
SHIM="-I shims_batch4 -I $TREE/lib"

build_krell() { # src-path, name, sed-exprs...
	local srcp=$1; local name=$2; shift 2
	$KRELLC parse "$srcp" -o $B/$name.krel
	$KRELLC verify $B/$name.krel > /dev/null
	$KRELLC build $B/$name.krel --target x86_64 -o $B/$name.s
	sed -i "$@" $B/$name.s
	gcc -c -o $B/${name}_krell.o $B/$name.s
}

build_krell $TREE/lib/math/gcd.krell gcd -e 's/\bgcd\b/krell_gcd/g'
build_krell $TREE/lib/math/lcm.krell lcm -e 's/\bgcd\b/krell_gcd/g' -e 's/\blcm_not_zero\b/krell_lcm_not_zero/g' -e 's/\blcm\b/krell_lcm/g'
build_krell $TREE/lib/math/int_pow.krell int_pow -e 's/\bint_pow\b/krell_int_pow/g'
build_krell $TREE/lib/math/int_sqrt.krell int_sqrt -e 's/\bint_sqrt\b/krell_int_sqrt/g'
build_krell $TREE/lib/bsearch.krell bsearch -e 's/\bbsearch\b/krell_bsearch/g'
build_krell $TREE/lib/kstrtox.krell kstrtox \
	-e 's/\bkstrtoull\b/krell_kstrtoull/g' -e 's/\bkstrtoll\b/krell_kstrtoll/g' \
	-e 's/\b_kstrtoul\b/krell__kstrtoul/g' -e 's/\b_kstrtol\b/krell__kstrtol/g' \
	-e 's/\bkstrtouint\b/krell_kstrtouint/g' -e 's/\bkstrtoint\b/krell_kstrtoint/g' \
	-e 's/\bkstrtou16\b/krell_kstrtou16/g' -e 's/\bkstrtos16\b/krell_kstrtos16/g' \
	-e 's/\bkstrtou8\b/krell_kstrtou8/g' -e 's/\bkstrtos8\b/krell_kstrtos8/g' \
	-e 's/\bkstrtobool\b/krell_kstrtobool/g' \
	-e 's/\bkstrtoudec64\b/krell_kstrtoudec64/g' -e 's/\bkstrtodec64\b/krell_kstrtodec64/g' \
	-e 's/\b_parse_integer_fixup_radix\b/krell__parse_integer_fixup_radix/g' \
	-e 's/\b_parse_integer_limit\b/krell__parse_integer_limit/g' \
	-e 's/\bint_pow\b/krell_int_pow/g'
build_krell $TREE/lib/cmdline.krell cmdline -e 's/\bget_options\b/krell_get_options/g' -e 's/\bget_option\b/krell_get_option/g' \
	-e 's/\bmemparse\b/krell_memparse/g' -e 's/\bparse_option_str\b/krell_parse_option_str/g' \
	-e 's/\bnext_arg\b/krell_next_arg/g'

gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/math/gcd.c -o $B/gcd_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/math/lcm.c -o $B/lcm_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/math/int_pow.c -o $B/int_pow_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/math/int_sqrt.c -o $B/int_sqrt_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/kstrtox.c -o $B/kstrtox_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/cmdline.c -o $B/cmdline_c.o
gcc -O2 -fno-builtin -Werror=implicit-function-declaration -w $SHIM -c $TREE/lib/ctype.c -o $B/ctype_c.o
gcc -O2 -fno-builtin -w -c bsearch_ref.c -o $B/bsearch_ref.o

gcc -O2 -o $B/difftest difftest_batch4.c \
	$B/gcd_krell.o $B/lcm_krell.o $B/int_pow_krell.o $B/int_sqrt_krell.o \
	$B/bsearch_krell.o $B/kstrtox_krell.o $B/cmdline_krell.o \
	$B/gcd_c.o $B/lcm_c.o $B/int_pow_c.o $B/int_sqrt_c.o \
	$B/kstrtox_c.o $B/cmdline_c.o $B/ctype_c.o $B/bsearch_ref.o
$B/difftest
