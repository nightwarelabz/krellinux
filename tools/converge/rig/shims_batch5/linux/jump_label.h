#pragma once
struct static_key_true { int dummy; };
#define DEFINE_STATIC_KEY_TRUE(name) struct static_key_true name = {0}
#define static_branch_likely(key) 1
#define static_branch_unlikely(key) 0
