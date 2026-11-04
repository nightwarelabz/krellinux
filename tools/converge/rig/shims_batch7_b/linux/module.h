/* Rig shims for lib/btree.c - userspace differential rig. */
#ifndef _RIG_MODULE_H
#define _RIG_MODULE_H
#define __init
#define __exit
#define EXPORT_SYMBOL_GPL(x)
#define EXPORT_SYMBOL(x)
#define MODULE_AUTHOR(x)
#define MODULE_DESCRIPTION(x)
#define MODULE_LICENSE(x)
#define module_init(x) int rig_module_init_##x(void) { return x(); }
#define module_exit(x) void rig_module_exit_##x(void) { x(); }
#endif
