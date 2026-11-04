/* Rig shims for lib/assoc_array.c - userspace rig. */
#ifndef _RIG_A_RCUPDATE_H
#define _RIG_A_RCUPDATE_H
#include <linux/kernel.h>
struct rcu_head {
	struct rcu_head *next;
	void (*func)(struct rcu_head *head);
};
typedef void (*rcu_callback_t)(struct rcu_head *head);
void call_rcu(struct rcu_head *head, rcu_callback_t func);
#define rcu_assign_pointer(p, v) ((p) = (v))
#define RCU_INIT_POINTER(p, v) ((p) = (v))
#define rcu_dereference(p) (p)
#define rcu_dereference_protected(p, c) (p)
#endif
