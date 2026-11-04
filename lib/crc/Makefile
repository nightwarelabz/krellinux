# SPDX-License-Identifier: GPL-2.0-only

# Makefile for the kernel's cyclic redundancy check (CRC) library code

obj-$(CONFIG_CRC4) += crc4.o
obj-$(CONFIG_CRC7) += crc7.o
obj-$(CONFIG_CRC8) += crc8.o
obj-$(CONFIG_CRC16) += crc16.o
obj-$(CONFIG_CRC_CCITT) += crc-ccitt.o
obj-$(CONFIG_CRC_ITU_T) += crc-itu-t.o

obj-$(CONFIG_CRC_T10DIF) += crc-t10dif.o
crc-t10dif-y := crc-t10dif-main.o
ifeq ($(CONFIG_CRC_T10DIF_ARCH),y)
CFLAGS_crc-t10dif-main.o += -I$(src)/$(SRCARCH)
crc-t10dif-$(CONFIG_ARM) += arm/crc-t10dif-core.o
crc-t10dif-$(CONFIG_ARM64) += arm64/crc-t10dif-core.o
crc-t10dif-$(CONFIG_PPC) += powerpc/crct10dif-vpmsum_asm.o
crc-t10dif-$(CONFIG_RISCV) += riscv/crc16_msb.o
crc-t10dif-$(CONFIG_X86) += x86/crc16-msb-pclmul.o
endif

obj-$(CONFIG_CRC32) += crc32.o
crc32-y := crc32-main.o
ifeq ($(CONFIG_CRC32_ARCH),y)
CFLAGS_crc32-main.o += -I$(src)/$(SRCARCH)
crc32-$(CONFIG_ARM) += arm/crc32-core.o
crc32-$(CONFIG_ARM64) += arm64/crc32-core.o
crc32-$(CONFIG_PPC) += powerpc/crc32c-vpmsum_asm.o
crc32-$(CONFIG_RISCV) += riscv/crc32_lsb.o riscv/crc32_msb.o
crc32-$(CONFIG_S390) += s390/crc32le-vx.o s390/crc32be-vx.o
crc32-$(CONFIG_SPARC) += sparc/crc32c_asm.o
crc32-$(CONFIG_X86) += x86/crc32-pclmul.o
crc32-$(CONFIG_X86_64) += x86/crc32c-3way.o
endif

obj-$(CONFIG_CRC64) += crc64.o
crc64-y := crc64-main.o
ifeq ($(CONFIG_CRC64_ARCH),y)
CFLAGS_crc64-main.o += -I$(src)/$(SRCARCH)

crc64-cflags-$(CONFIG_ARM) += -march=armv8-a -mfpu=crypto-neon-fp-armv8
crc64-cflags-$(CONFIG_ARM64) += -march=armv8-a+crypto
CFLAGS_REMOVE_crc64-neon.o += $(CC_FLAGS_NO_FPU)
CFLAGS_crc64-neon.o += $(CC_FLAGS_FPU) -I$(src)/$(SRCARCH) $(crc64-cflags-y)
crc64-$(CONFIG_ARM) += crc64-neon.o
crc64-$(CONFIG_ARM64) += crc64-neon.o

crc64-$(CONFIG_RISCV) += riscv/crc64_lsb.o riscv/crc64_msb.o
crc64-$(CONFIG_X86) += x86/crc64-pclmul.o
endif # CONFIG_CRC64_ARCH

obj-y += tests/

hostprogs := gen_crc32table gen_crc64table
clean-files := crc32table.h crc64table.h

$(obj)/crc32-main.o: $(obj)/crc32table.h
$(obj)/crc64-main.o: $(obj)/crc64table.h

quiet_cmd_crc32 = GEN     $@
      cmd_crc32 = $< > $@

quiet_cmd_crc64 = GEN     $@
      cmd_crc64 = $< > $@

$(obj)/crc32table.h: $(obj)/gen_crc32table
	$(call cmd,crc32)

$(obj)/crc64table.h: $(obj)/gen_crc64table
	$(call cmd,crc64)

# Krellinux Phase 0 spike: build lib/crc/crc16.krell through krellc and
# link it as crc16_krell.o; krell_spike.o self-checks it at boot.
# KRELLC must name a krellc binary; KRELL_TARGETS its targets/ dir.
KRELLC ?= krellc
obj-y += krell_spike.o crc16_krell.o

$(obj)/crc16_krell.krel: $(src)/crc16.krell FORCE
	$(Q)$(KRELLC) parse $< -o $@

$(obj)/crc16_krell.s: $(obj)/crc16_krell.krel FORCE
	$(Q)$(KRELLC) build $< --target x86_64 -o $@.tmp && \
		sed 's/\bcrc16\b/krell_crc16/g' $@.tmp > $@ && rm $@.tmp

$(obj)/crc16_krell.o: $(obj)/crc16_krell.s FORCE
	$(Q)$(CC) -c -o $@ $<

# Krellinux Phase 1 batch 2: Krell conversions of the remaining small
# CRC files, built the same way as the Phase 0 spike and self-checked
# at boot by krell_crc_check.o. Each conversion is proven against its
# C original by the differential rig in tools/converge/rig/ (batch 2).
obj-y += krell_crc_check.o crc_ccitt_krell.o crc_itu_t_krell.o \
	 crc4_krell.o crc7_krell.o crc8_krell.o

$(obj)/crc_ccitt_krell.krel: $(src)/crc-ccitt.krell FORCE
	$(Q)$(KRELLC) parse $< -o $@

$(obj)/crc_ccitt_krell.s: $(obj)/crc_ccitt_krell.krel FORCE
	$(Q)$(KRELLC) build $< --target x86_64 -o $@.tmp && \
		sed 's/\bcrc_ccitt\b/krell_crc_ccitt/g' $@.tmp > $@ && rm $@.tmp

$(obj)/crc_ccitt_krell.o: $(obj)/crc_ccitt_krell.s FORCE
	$(Q)$(CC) -c -o $@ $<

$(obj)/crc_itu_t_krell.krel: $(src)/crc-itu-t.krell FORCE
	$(Q)$(KRELLC) parse $< -o $@

$(obj)/crc_itu_t_krell.s: $(obj)/crc_itu_t_krell.krel FORCE
	$(Q)$(KRELLC) build $< --target x86_64 -o $@.tmp && \
		sed 's/\bcrc_itu_t\b/krell_crc_itu_t/g' $@.tmp > $@ && rm $@.tmp

$(obj)/crc_itu_t_krell.o: $(obj)/crc_itu_t_krell.s FORCE
	$(Q)$(CC) -c -o $@ $<

$(obj)/crc4_krell.krel: $(src)/crc4.krell FORCE
	$(Q)$(KRELLC) parse $< -o $@

$(obj)/crc4_krell.s: $(obj)/crc4_krell.krel FORCE
	$(Q)$(KRELLC) build $< --target x86_64 -o $@.tmp && \
		sed 's/\bcrc4\b/krell_crc4/g' $@.tmp > $@ && rm $@.tmp

$(obj)/crc4_krell.o: $(obj)/crc4_krell.s FORCE
	$(Q)$(CC) -c -o $@ $<

$(obj)/crc7_krell.krel: $(src)/crc7.krell FORCE
	$(Q)$(KRELLC) parse $< -o $@

$(obj)/crc7_krell.s: $(obj)/crc7_krell.krel FORCE
	$(Q)$(KRELLC) build $< --target x86_64 -o $@.tmp && \
		sed 's/\bcrc7_be\b/krell_crc7_be/g' $@.tmp > $@ && rm $@.tmp

$(obj)/crc7_krell.o: $(obj)/crc7_krell.s FORCE
	$(Q)$(CC) -c -o $@ $<

$(obj)/crc8_krell.krel: $(src)/crc8.krell FORCE
	$(Q)$(KRELLC) parse $< -o $@

$(obj)/crc8_krell.s: $(obj)/crc8_krell.krel FORCE
	$(Q)$(KRELLC) build $< --target x86_64 -o $@.tmp && \
		sed -e 's/\bcrc8_populate_msb\b/krell_crc8_populate_msb/g' \
		    -e 's/\bcrc8_populate_lsb\b/krell_crc8_populate_lsb/g' \
		    -e 's/\bcrc8\b/krell_crc8/g' $@.tmp > $@ && rm $@.tmp

$(obj)/crc8_krell.o: $(obj)/crc8_krell.s FORCE
	$(Q)$(CC) -c -o $@ $<
