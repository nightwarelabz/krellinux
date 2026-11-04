#ifndef _SHIM_LINUX_SCHED_H
#define _SHIM_LINUX_SCHED_H
/* In this tree's configuration (CONFIG_PREEMPTION=y)
 * cond_resched() compiles to no resched call. */
#define cond_resched() do { } while (0)
#endif
