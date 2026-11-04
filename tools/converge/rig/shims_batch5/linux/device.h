#ifndef _SHIM_LINUX_DEVICE_H
#define _SHIM_LINUX_DEVICE_H
struct device;
int devm_add_action_or_reset(struct device *, void (*)(void *), void *);
#endif
