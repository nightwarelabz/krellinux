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
| `lib/assoc_array.c` | KRELL (functions) | ASSIST | All 10 exported functions and all 8 file-local helpers rewritten in Krell (lib/assoc_array.krell): assoc_array_iterate, assoc_array_find, assoc_array_destroy, assoc_array_insert, assoc_array_insert_set_object, assoc_array_delete, assoc_array_clear, assoc_array_apply_edit, assoc_array_cancel_edit and assoc_array_gc. Tagged pointers travel as i64 bit patterns end to end; the ops callbacks and iterators are reached through helpers that load the target from memory at the call site, and C bool callback results are normalised through the low byte. READ_ONCE sites use volatile loads and the smp_wmb in apply_edit is a release fence. Differential rig vs the C original: 41,843 cases, 0 divergence, with identical allocation totals by size; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 7). The C file still provides the symbols. |
| `lib/atomic64.c` | C | AUTO | - |
| `lib/atomic64_test.c` | C | ASSIST | indirect calls / function pointers; atomics beyond load/store/add; inline asm |
| `lib/audit.c` | C | AUTO | - |
| `lib/base64.c` | C | AUTO | - |
| `lib/bcd.c` | KRELL (functions) | AUTO | _bcd2bin() and _bin2bcd() rewritten in Krell (lib/bcd.krell); differential rig vs the C original: 160,256 cases (_bcd2bin exhaustive over all 256 inputs; _bin2bcd exhaustive 0..99999 plus random full-range values), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 1). The C file still provides the symbols. |
| `lib/bch.c` | C | ASSIST | inline asm |
| `lib/bitmap-str.c` | KRELL (functions) | AUTO | All 6 exported functions rewritten in Krell (lib/bitmap-str.krell): bitmap_parse and bitmap_parse_user, bitmap_parselist and bitmap_parselist_user, bitmap_print_bitmask_to_buf and bitmap_print_list_to_buf, with the file-local region helpers carried as internal functions (the C file's struct region is passed as four scalar slots owned by bitmap_parselist); differential rig vs the C originals: part of the batch 5 rig (22,855 cases across the batch, 0 divergence), including the kasprintf argument contract of the print functions, which the boot self-check compares through the real vsprintf path; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 5). The C file still provides the symbols. |
| `lib/bitmap.c` | KRELL (functions) | AUTO | 33 of 35 functions rewritten in Krell (lib/bitmap.krell): the __bitmap_* core (equal/or_equal/complement, shifts, cut, and/or/xor/andnot/replace, intersects/subset, the weight family, set/clear), bitmap_find_next_zero_area_off, bitmap_remap/bitmap_bitremap/bitmap_onto/bitmap_fold (the last two under CONFIG_NUMA, as configured), the bitmap_alloc/zalloc family and bitmap_from/to_arr32; differential rig vs the C originals: part of the batch 5 rig (22,855 cases across the batch, 0 divergence); Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 5). Residue: devm_bitmap_alloc() and devm_bitmap_zalloc() stay C -- they register the file-local static devm_bitmap_free() callback whose address the Krell form cannot name. bitmap_from/to_arr64 are compiled only when BITS_PER_LONG == 32 and have no x86_64 instance. The C file still provides the symbols. |
| `lib/bitrev.c` | C | AUTO | - |
| `lib/bootconfig.c` | C | ASSIST | inline asm |
| `lib/bsearch.c` | KRELL (function) | AUTO | bsearch() rewritten in Krell (lib/bsearch.krell): the C file is a wrapper over __inline_bsearch() from include/linux/bsearch.h, and the Krell form is that algorithm, with the comparator reached through an indirect call on its C signature (ptr, ptr) -> int; differential rig vs the header algorithm: part of the batch 4 rig (215,447 cases across the batch, 0 divergence), including a comparator with poisoned upper return bits; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 4). The C file still provides the symbol. |
| `lib/btree.c` | KRELL (functions) | ASSIST | 16 of 18 functions rewritten in Krell (lib/btree.krell): btree_init_mempool, btree_init, btree_destroy, btree_last, btree_lookup, btree_update, btree_get_prev, btree_insert, btree_remove, btree_merge, visitorl/32/64/128, btree_visitor and btree_grim_visitor, with the recursive helpers carried as internal functions. Residue: btree_alloc and btree_free stay C (they alone touch the file-local btree_cachep); the Krell form reaches them as externs, and btree_init passes their addresses to mempool_create. The C implementation's separator keys are hints that find_level rewrites on descent, and btree_get_prev wraps to the largest key below the smallest key's predecessor; the Krell form reproduces both behaviours operation for operation (get_prev works in place on the caller's key buffer, saving and restoring it on the NULL path, as Krell data blobs are constants). Differential rig vs the C original: 969,563 cases, 0 divergence, with identical mempool alloc/free accounting; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 7). The C file still provides the symbols. |
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
| `lib/cmdline.c` | KRELL (functions) | AUTO | get_option, get_options, memparse, parse_option_str and next_arg rewritten in Krell (lib/cmdline.krell), with the file-local get_range as an internal helper; differential rig vs the C original: part of the batch 4 rig (215,447 cases across the batch, 0 divergence); Krell objects linked into vmlinux and self-checked at boot (Phase 1 batch 4). simple_strtoull/simple_strtol, skip_spaces and strlen/strncmp are called externally, as in C. The C file still provides the symbols. |
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
| `lib/find_bit.c` | KRELL (functions) | AUTO | All 16 functions rewritten in Krell (lib/find_bit.krell): the _find_first/_find_next families (plain, and, andnot, or, zero variants), __find_nth_bit and its and/andnot forms, _find_last_bit, find_next_clump8 and find_random_bit, mirroring the C FIND_* macro expansions operation for operation, including the unmasked last word of the first-bit scan and the nth-bit fall-through; differential rig vs the C original: part of the batch 5 rig (22,855 cases across the batch, 0 divergence); Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 5). The _le variants are compiled only on big-endian and have no x86_64 instance. The C file still provides the symbols. |
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
| `lib/kstrtox.c` | KRELL (functions) | AUTO | 17 of 28 functions rewritten in Krell (lib/kstrtox.krell): _parse_integer_fixup_radix, _parse_integer_limit, the kstrtoull/kstrtoll family and its width wrappers, kstrtobool, and the kstrtoudec64/kstrtodec64 scaled decimals; differential rig vs the C original: part of the batch 4 rig (215,447 cases across the batch, 0 divergence); Krell objects linked into vmlinux and self-checked at boot (Phase 1 batch 4). Residue: the 11 *_from_user wrappers stay C -- they stage the user string in a stack buffer of 11 to 67 bytes, and the current Krell native backend addresses an alloca region from its top, so writes past the first 8 bytes of a buffer overwrite the frame (batch 4 record has the reproducer); the wrappers convert when a compiler with the alloca fix is pinned. The C file still provides the symbols. |
| `lib/linear_ranges.c` | C | AUTO | - |
| `lib/list_debug.c` | C | AUTO | - |
| `lib/list_sort.c` | KRELL (function) | AUTO | list_sort() rewritten in Krell (lib/list_sort.krell) with its merge and merge_final helpers, operation for operation, including the stability rule (on equality take from the first list). C's pointer-to-pointer walks become plain addresses (next is at offset 0, prev at offset 8 in struct list_head); the address of the pending head uses an 8-byte alloca slot. Differential rig vs the C original: part of the batch 6 rig (11,143 cases across the batch, 0 divergence), comparing forward and backward walks and circular closure, with both three-way and boolean comparators; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 6). The C file still provides the symbol. |
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
| `lib/muldi3.c` | KRELL (function) | AUTO | __muldi3() rewritten in Krell (lib/muldi3.krell); differential rig vs the C original: 100,225 cases (structured cross-product plus random pairs), 0 divergence, and the same corpus checked against a 128-bit oracle; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 2). The C file still provides the symbol where configured. |
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
| `lib/sort.c` | KRELL (functions) | ASSIST | All 4 exported functions rewritten in Krell (lib/sort.krell): sort_r, sort_r_nonatomic, sort and sort_nonatomic over the bottom-up heapsort, operation for operation. The sentinel dispatch (SWAP_WORDS_64/32, SWAP_BYTES, SWAP_WRAPPER, _CMP_WRAPPER) is reproduced by reading the function pointers as integers through the alloca bridge; sort()/sort_nonatomic() pass their wrapper's two fields as separate internal arguments instead of a stack struct (nothing outside sort.c can observe it), while sort_r() callers using the wrapper sentinel get the pointers loaded from their priv struct on every use, as in C. cond_resched() compiles to nothing under this tree's configuration, so the may_schedule flag is plumbed but empty, exactly as in the C object. Differential rig vs the C original: part of the batch 6 rig (11,143 cases across the batch, 0 divergence), including swap-call counts through counting swap functions; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 6). The C file still provides the symbols. |
| `lib/stackdepot.c` | C | ASSIST | indirect calls / function pointers; inline asm |
| `lib/stmp_device.c` | C | AUTO | - |
| `lib/string.c` | KRELL (functions) | ASSIST | 33 of 34 functions rewritten in Krell (lib/string.krell): the full string/memory set (strlen/strnlen, the copy and compare families, the search family, strspn/strcspn/strpbrk, strsep, memset/memset16/32/64, memcpy/memmove, memcmp/bcmp, memscan/memchr/memchr_inv, strstr/strnstr, sized_strscpy); differential rig vs the C originals: 92,899 cases, 0 divergence, including all 256x256 single-byte pairs for the case functions against the real _ctype table (its Latin-1 upper ranges are mirrored); Krell objects linked into vmlinux and self-checked at boot (Phase 1 batch 3). Residue: strlcat() stays C -- its BUG_ON() trap on a caller contract violation has no Krell counterpart. Notes: sized_strscpy() past-the-NUL dest bytes are unspecified in C (word-at-a-time may zero some) and left untouched by the Krell form; its WARN_ON_ONCE for count > INT_MAX is a log line, not a result. The C file still provides the symbols. |
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
| `lib/crc/crc16.c` | KRELL (function) | - | crc16() rewritten in Krell (lib/crc/crc16.krell); differential rig vs the C original: 3,563 cases, 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 0 spike). The C file still provides the symbol; replacement is Phase 1. |
| `lib/crc/crc-ccitt.c` | KRELL (function) | - | crc_ccitt() rewritten in Krell (lib/crc/crc-ccitt.krell, bitwise reflected form, polynomial 0x8408); differential rig vs the C original: 3,565 cases, 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 2). The table data and the C function remain in the C file. |
| `lib/crc/crc-itu-t.c` | KRELL (function) | - | crc_itu_t() rewritten in Krell (lib/crc/crc-itu-t.krell, bitwise MSB-first form, polynomial 0x1021); differential rig vs the C original: 3,565 cases, 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 2). The table data and the C function remain in the C file. |
| `lib/crc/crc4.c` | KRELL (function) | - | crc4() rewritten in Krell (lib/crc/crc4.krell, table step performed as four bit steps); differential rig vs the C original: 62,048 cases (every bit count 0..63 in the defined domain), 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 2). |
| `lib/crc/crc7.c` | KRELL (function) | - | crc7_be() rewritten in Krell (lib/crc/crc7.krell, bitwise MSB-first form, polynomial x^7 + x^3 + 1); differential rig vs the C original: 3,060 cases, 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 2). The table data and the C function remain in the C file. |
| `lib/crc/crc8.c` | KRELL (functions) | - | crc8(), crc8_populate_msb() and crc8_populate_lsb() rewritten in Krell (lib/crc/crc8.krell, table-generic structure kept); differential rig vs the C originals: 8,020 cases including full 256-byte table comparisons for ten polynomials, 0 divergence; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 2). |
| `lib/math/gcd.c` | KRELL (function) | - | gcd() rewritten in Krell (lib/math/gcd.krell), following the binary GCD the C file selects on its static key (the fallback loop computes the same gcd; the choice is result-invariant); differential rig vs the C original: part of the batch 4 rig (215,447 cases across the batch, 0 divergence); Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 4). The C file still provides the symbol. |
| `lib/math/lcm.c` | KRELL (functions) | - | lcm() and lcm_not_zero() rewritten in Krell (lib/math/lcm.krell), calling the Krell gcd; differential rig vs the C originals: part of the batch 4 rig (215,447 cases across the batch, 0 divergence); Krell objects linked into vmlinux and self-checked at boot (Phase 1 batch 4). The C file still provides the symbols. |
| `lib/math/int_pow.c` | KRELL (function) | - | int_pow() rewritten in Krell (lib/math/int_pow.krell); differential rig vs the C original: part of the batch 4 rig (215,447 cases across the batch, 0 divergence); Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 4). The C file still provides the symbol. |
| `lib/math/int_sqrt.c` | KRELL (function) | - | int_sqrt() rewritten in Krell (lib/math/int_sqrt.krell), the C file's shift-and-subtract algorithm operation for operation; differential rig vs the C original: part of the batch 4 rig (215,447 cases across the batch, 0 divergence), including the full 0..100000 range; Krell object linked into vmlinux and self-checked at boot (Phase 1 batch 4). int_sqrt64() is compiled only when BITS_PER_LONG < 64 and has no x86_64 instance. The C file still provides the symbol. |

Directories beyond lib/ enter the ledger as their phase's census completes.
