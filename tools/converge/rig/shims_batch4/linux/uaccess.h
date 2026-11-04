#pragma once
#ifndef __user
#define __user
#endif
unsigned long copy_from_user(void *to, const void *from, unsigned long n);
