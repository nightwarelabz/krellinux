# Krellinux conversion ledger

Generated 2026-11-04 by the Auto-Convergence Engine (gen_ledger.py) from census data. Base: Linux 7.4.0-rc1 (05441d69), fork master 2c3d62bc.

States: **C** untouched - **CONVERTING** in progress - **KRELL** converted and proven by the differential rig - **RESIDUE** named, stays C/asm by decision.

Census of lib/: {'AUTO': 83, 'ASSIST': 104, 'RESIDUE': 25} (AUTO / ASSIST / RESIDUE are the engine's conversion classes, not states).

| File | State | Class | Evidence / blockers |
|---|---|---|---|
| `lib/argv_split.c` | C | AUTO | - |
| `lib/ashldi3.c` | KRELL (function) | AUTO | __ashldi3() rewritten in Krell (lib/ashldi3.krell); differential rig vs the C original: 65,184 cases (every shift count 0..63 x structured and random operands), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 1). The C file still provides the symbol where configured. |
| `lib/ashrdi3.c` | KRELL (function) | AUTO | __ashrdi3() rewritten in Krell (lib/ashrdi3.krell); differential rig vs the C original: 65,184 cases (every shift count 0..63 x structured and random operands), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 1). The C file still provides the symbol where configured. |
| `lib/asn1_decoder.c` | C | ASSIST | indirect calls / function pointers |
| `lib/asn1_encoder.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/assoc_array.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/atomic64.c` | C | AUTO | - |
| `lib/atomic64_test.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/audit.c` | C | AUTO | - |
| `lib/base64.c` | C | AUTO | - |
| `lib/bcd.c` | KRELL (functions) | AUTO | _bcd2bin() and _bin2bcd() rewritten in Krell (lib/bcd.krell); differential rig vs the C original: 160,256 cases (_bcd2bin exhaustive over all 256 inputs; _bin2bcd exhaustive 0..99999 plus random full-range values), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 1). The C file still provides the symbols. |
| `lib/bch.c` | C | ASSIST | inline asm |
| `lib/bitmap-str.c` | C | AUTO | - |
| `lib/bitmap.c` | C | AUTO | - |
| `lib/bitrev.c` | C | AUTO | - |
| `lib/bootconfig.c` | C | ASSIST | inline asm |
| `lib/bsearch.c` | C | AUTO | - |
| `lib/btree.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/bucket_locks.c` | C | AUTO | - |
| `lib/bug.c` | C | ASSIST | varargs |
| `lib/buildid.c` | C | ASSIST | inline asm |
| `lib/bust_spinlocks.c` | C | AUTO | - |
| `lib/cache_maint.c` | C | ASSIST | indirect calls / function pointers |
| `lib/check_signature.c` | C | ASSIST | volatile |
| `lib/checksum.c` | C | AUTO | - |
| `lib/closure.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/clz_ctz.c` | C | AUTO | - |
| `lib/clz_tab.c` | C | AUTO | - |
| `lib/cmdline.c` | C | AUTO | - |
| `lib/cmpdi2.c` | KRELL (function) | AUTO | __cmpdi2() rewritten in Krell (lib/cmpdi2.krell); differential rig vs the C original: 60,225 cases (structured cross-product plus random and near-equal pairs), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 1). The C file still provides the symbol where configured. |
| `lib/cmpxchg-emu.c` | C | ASSIST | atomics beyond load/store/add; volatile; inline asm |
| `lib/codetag.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/compat_audit.c` | C | RESIDUE | clang parse error: 'asm/unistd32.h' file not found |
| `lib/cpu_rmap.c` | C | AUTO | - |
| `lib/cpumask.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/ctype.c` | C | AUTO | - |
| `lib/debug_info.c` | C | AUTO | - |
| `lib/debug_locks.c` | C | AUTO | - |
| `lib/debugobjects.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; volatile; inline asm |
| `lib/dec_and_lock.c` | C | ASSIST | atomics beyond load/store/add |
| `lib/decompress.c` | C | AUTO | - |
| `lib/decompress_bunzip2.c` | C | ASSIST | indirect calls / function pointers |
| `lib/decompress_inflate.c` | C | ASSIST | indirect calls / function pointers |
| `lib/decompress_unlz4.c` | C | ASSIST | indirect calls / function pointers |
| `lib/decompress_unlzma.c` | C | ASSIST | indirect calls / function pointers |
| `lib/decompress_unlzo.c` | C | ASSIST | indirect calls / function pointers |
| `lib/decompress_unxz.c` | C | ASSIST | indirect calls / function pointers |
| `lib/decompress_unzstd.c` | C | ASSIST | indirect calls / function pointers |
| `lib/devmem_is_allowed.c` | C | AUTO | - |
| `lib/devres.c` | C | ASSIST | inline asm |
| `lib/dhry_1.c` | C | AUTO | - |
| `lib/dhry_2.c` | C | AUTO | - |
| `lib/dhry_run.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/digsig.c` | C | AUTO | - |
| `lib/dump_stack.c` | C | ASSIST | atomics beyond load/store/add; varargs |
| `lib/dynamic_debug.c` | C | ASSIST | indirect calls / function pointers; varargs; bitfields; inline asm |
| `lib/dynamic_queue_limits.c` | C | ASSIST | inline asm |
| `lib/earlycpio.c` | C | AUTO | - |
| `lib/errname.c` | C | AUTO | - |
| `lib/error-inject.c` | C | ASSIST | indirect calls / function pointers |
| `lib/errseq.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; volatile; inline asm |
| `lib/extable.c` | C | AUTO | - |
| `lib/fault-inject-usercopy.c` | C | AUTO | - |
| `lib/fault-inject.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/fdt.c` | C | AUTO | - |
| `lib/fdt_addresses.c` | C | AUTO | - |
| `lib/fdt_empty_tree.c` | C | AUTO | - |
| `lib/fdt_ro.c` | C | AUTO | - |
| `lib/fdt_rw.c` | C | AUTO | - |
| `lib/fdt_strerror.c` | C | AUTO | - |
| `lib/fdt_sw.c` | C | AUTO | - |
| `lib/fdt_wip.c` | C | AUTO | - |
| `lib/find_bit.c` | C | AUTO | - |
| `lib/find_bit_benchmark.c` | C | ASSIST | inline asm |
| `lib/flex_proportions.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/fw_table.c` | C | ASSIST | indirect calls / function pointers |
| `lib/genalloc.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; volatile; inline asm |
| `lib/generic-radix-tree.c` | C | ASSIST | atomics beyond load/store/add; volatile; inline asm |
| `lib/glob.c` | C | AUTO | - |
| `lib/group_cpus.c` | C | ASSIST | inline asm |
| `lib/hexdump.c` | C | AUTO | - |
| `lib/hweight.c` | C | AUTO | - |
| `lib/idr.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/inflate.c` | C | RESIDUE | clang parse error: 'gzip.h' file not found |
| `lib/interval_tree.c` | C | AUTO | - |
| `lib/interval_tree_test.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/iomap.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/iomap_copy.c` | C | AUTO | - |
| `lib/iomem_copy.c` | C | AUTO | - |
| `lib/iommu-helper.c` | C | AUTO | - |
| `lib/iov_iter.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/irq_poll.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/irq_regs.c` | C | AUTO | - |
| `lib/is_single_threaded.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/kasprintf.c` | C | ASSIST | indirect calls / function pointers; varargs; inline asm |
| `lib/kfifo.c` | C | ASSIST | inline asm |
| `lib/klist.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/kobject.c` | C | ASSIST | indirect calls / function pointers; varargs; inline asm |
| `lib/kobject_uevent.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; varargs; inline asm |
| `lib/kstrtox.c` | C | AUTO | - |
| `lib/linear_ranges.c` | C | AUTO | - |
| `lib/list_debug.c` | C | AUTO | - |
| `lib/list_sort.c` | C | AUTO | - |
| `lib/llist.c` | C | ASSIST | atomics beyond load/store/add; volatile; inline asm |
| `lib/locking-selftest.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/lockref.c` | C | ASSIST | atomics beyond load/store/add; volatile; inline asm |
| `lib/logic_iomem.c` | C | ASSIST | indirect calls / function pointers; volatile; inline asm |
| `lib/logic_pio.c` | C | AUTO | - |
| `lib/lru_cache.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; volatile; inline asm |
| `lib/lshrdi3.c` | KRELL (function) | AUTO | __lshrdi3() rewritten in Krell (lib/lshrdi3.krell); differential rig vs the C original: 65,184 cases (every shift count 0..63 x structured and random operands), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 1). The C file still provides the symbol where configured. |
| `lib/lwq.c` | C | ASSIST | inline asm |
| `lib/maple_tree.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/memcat_p.c` | C | AUTO | - |
| `lib/memory-notifier-error-inject.c` | C | AUTO | - |
| `lib/memregion.c` | C | AUTO | - |
| `lib/memweight.c` | C | ASSIST | inline asm |
| `lib/min_heap.c` | C | AUTO | - |
| `lib/muldi3.c` | C | AUTO | - |
| `lib/net_utils.c` | C | AUTO | - |
| `lib/netdev-notifier-error-inject.c` | C | AUTO | - |
| `lib/nlattr.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/nmi_backtrace.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/notifier-error-inject.c` | C | ASSIST | indirect calls / function pointers |
| `lib/objagg.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/objpool.c` | C | ASSIST | indirect calls / function pointers |
| `lib/of-reconfig-notifier-error-inject.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/oid_registry.c` | C | AUTO | - |
| `lib/oid_registry_data.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/once.c` | C | ASSIST | inline asm |
| `lib/packing.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/packing_test.c` | C | ASSIST | indirect calls / function pointers |
| `lib/parman.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/parser.c` | C | AUTO | - |
| `lib/percpu-refcount.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/percpu_counter.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/percpu_test.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; volatile; inline asm |
| `lib/plist.c` | C | ASSIST | inline asm |
| `lib/pm-notifier-error-inject.c` | C | AUTO | - |
| `lib/radix-tree.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/random32.c` | C | AUTO | - |
| `lib/ratelimit.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/rbtree.c` | C | ASSIST | indirect calls / function pointers |
| `lib/rbtree_test.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/rcuref.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/ref_tracker.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/refcount.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/region_alloc_benchmark.c` | C | ASSIST | inline asm |
| `lib/rhashtable.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; volatile; inline asm |
| `lib/sbitmap.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/scatterlist.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/seq_buf.c` | C | ASSIST | varargs; inline asm |
| `lib/sg_pool.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/sg_split.c` | C | AUTO | - |
| `lib/smp_processor_id.c` | C | ASSIST | inline asm |
| `lib/sort.c` | C | ASSIST | indirect calls / function pointers |
| `lib/stackdepot.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/stmp_device.c` | C | AUTO | - |
| `lib/string.c` | C | ASSIST | inline asm |
| `lib/string_helpers.c` | C | ASSIST | inline asm |
| `lib/strncpy_from_user.c` | C | ASSIST | inline asm |
| `lib/strnlen_user.c` | C | ASSIST | inline asm |
| `lib/sys_info.c` | C | AUTO | - |
| `lib/syscall.c` | C | AUTO | - |
| `lib/test-kstrtox.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/test_bitmap.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_bitops.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_context-analysis.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/test_csd_lock.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_debug_virtual.c` | C | AUTO | - |
| `lib/test_dynamic_debug.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_firmware.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_fpu_glue.c` | C | AUTO | - |
| `lib/test_fpu_impl.c` | C | ASSIST | volatile |
| `lib/test_free_pages.c` | C | AUTO | - |
| `lib/test_hexdump.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_hmm.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_ida.c` | C | AUTO | - |
| `lib/test_kho.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_kmod.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_lockup.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_maple_tree.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/test_memcat_p.c` | C | AUTO | - |
| `lib/test_meminit.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_module.c` | C | AUTO | - |
| `lib/test_objagg.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_objpool.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/test_parman.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_ref_tracker.c` | C | ASSIST | atomics beyond load/store/add |
| `lib/test_rhashtable.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_static_key_base.c` | C | AUTO | - |
| `lib/test_static_keys.c` | C | ASSIST | indirect calls / function pointers |
| `lib/test_sysctl.c` | C | ASSIST | indirect calls / function pointers |
| `lib/test_ubsan.c` | C | ASSIST | indirect calls / function pointers; volatile; inline asm |
| `lib/test_vmalloc.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_workqueue.c` | C | RESIDUE | clang parse error: too many errors emitted, stopping now |
| `lib/test_xarray.c` | C | ASSIST | atomics beyond load/store/add; inline asm |
| `lib/textsearch.c` | C | ASSIST | indirect calls / function pointers |
| `lib/timerqueue.c` | C | ASSIST | inline asm |
| `lib/trace_readwrite.c` | C | AUTO | - |
| `lib/ts_bm.c` | C | ASSIST | indirect calls / function pointers |
| `lib/ts_fsm.c` | C | ASSIST | indirect calls / function pointers |
| `lib/ts_kmp.c` | C | ASSIST | indirect calls / function pointers |
| `lib/ubsan.c` | C | ASSIST | inline asm |
| `lib/ucmpdi2.c` | KRELL (function) | AUTO | __ucmpdi2() rewritten in Krell (lib/ucmpdi2.krell); differential rig vs the C original: 60,225 cases (structured cross-product plus random and near-equal pairs), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 1). The C file still provides the symbol where configured. |
| `lib/ucs2_string.c` | C | AUTO | - |
| `lib/union_find.c` | C | AUTO | - |
| `lib/usercopy.c` | C | ASSIST | inline asm |
| `lib/uuid.c` | C | AUTO | - |
| `lib/vsprintf.c` | C | ASSIST | indirect calls / function pointers; varargs; inline asm |
| `lib/win_minmax.c` | C | AUTO | - |
| `lib/xarray.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/xxhash.c` | C | AUTO | - |

Directories beyond lib/ enter the ledger as their phase's census completes.
