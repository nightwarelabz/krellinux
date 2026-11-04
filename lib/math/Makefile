# SPDX-License-Identifier: GPL-2.0-only
obj-y += div64.o gcd.o lcm.o int_log.o int_pow.o int_sqrt.o reciprocal_div.o

# Krellinux conversion (Phase 1 batch 4): the Krell forms of
# gcd.c, lcm.c, int_pow.c and int_sqrt.c are built through
# krellc and linked beside their C originals with symbols
# renamed to krell_*; lib/krell_batch4_check.c self-checks
# them at boot.
KRELLC ?= krellc
obj-y += gcd_krell.o lcm_krell.o int_pow_krell.o int_sqrt_krell.o

krell_math_rename = sed -e 's/\bgcd\b/krell_gcd/g' \
	-e 's/\blcm_not_zero\b/krell_lcm_not_zero/g' \
	-e 's/\blcm\b/krell_lcm/g' \
	-e 's/\bint_pow\b/krell_int_pow/g' \
	-e 's/\bint_sqrt\b/krell_int_sqrt/g'

$(obj)/%_krell.krel: $(src)/%.krell FORCE
	$(Q)$(KRELLC) parse $< -o $@

$(obj)/%_krell.s: $(obj)/%_krell.krel FORCE
	$(Q)$(KRELLC) build $< --target x86_64 -o $@.tmp && \
	$(krell_math_rename) $@.tmp > $@ && rm $@.tmp

$(obj)/%_krell.o: $(obj)/%_krell.s FORCE
	$(Q)$(CC) -c -o $@ $<

obj-$(CONFIG_CORDIC)		+= cordic.o
obj-$(CONFIG_POLYNOMIAL)	+= polynomial.o
obj-$(CONFIG_PRIME_NUMBERS)	+= prime_numbers.o
obj-$(CONFIG_RATIONAL)		+= rational.o

obj-$(CONFIG_TEST_DIV64)	+= test_div64.o
obj-$(CONFIG_TEST_MULDIV64)	+= test_mul_u64_u64_div_u64.o

obj-y += tests/
