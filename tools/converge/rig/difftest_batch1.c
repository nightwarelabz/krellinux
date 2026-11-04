/* Auto-Convergence Engine -- differential rig, Krellinux Phase 1 batch 1.
 * Functions under proof (kernel lib/ originals vs their Krell forms):
 *   __ashldi3, __ashrdi3, __lshrdi3  (lib/{ashl,ashr,lshr}di3.c)
 *   __cmpdi2, __ucmpdi2              (lib/{,u}cmpdi2.c)
 *   _bcd2bin, _bin2bcd               (lib/bcd.c)
 * C side: the kernel sources compiled as-is, symbols renamed *_c.
 * Krell side: the .krell conversions through krellc, symbols krell_*.
 * Agreement is exact (bit-for-bit) or the conversion does not integrate.
 *
 * Domains: the shift helpers are proven for every shift count in
 * their defined domain 0..63 (C is undefined outside it; compiler
 * generated calls only ever pass 0..63). _bcd2bin is proven on all
 * 256 inputs. _bin2bcd is proven exhaustively on 0..99999 and on
 * random full-range unsigned values (its arithmetic is total).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef long long ll;
typedef unsigned long long ull;

/* C originals (renamed) */
ll __ashldi3_c(ll u, long b);
ll __ashrdi3_c(ll u, long b);
ll __lshrdi3_c(ll u, long b);
long __cmpdi2_c(ll a, ll b);
long __ucmpdi2_c(ull a, ull b);
unsigned _bcd2bin_c(unsigned char v);
unsigned char _bin2bcd_c(unsigned v);

/* Krell forms (renamed) */
ll krell___ashldi3(ll u, ll b);
ll krell___ashrdi3(ll u, ll b);
ll krell___lshrdi3(ll u, ll b);
ll krell___cmpdi2(ll a, ll b);
ll krell___ucmpdi2(ll a, ll b);
int krell__bcd2bin(int v);
int krell__bin2bcd(int v);

static uint64_t rng_state = 0x9E3779B97F4A7C15ULL;
static uint64_t rnd(void) {
    uint64_t z = (rng_state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

static long cases = 0, fails = 0;

static void eq_ll(const char *fn, ll a, ll b, ll x, ll y) {
    cases++;
    if (x != y) {
        if (fails < 8)
            printf("MISMATCH %s(%lld, %lld): C=%lld Krell=%lld\n", fn, a, b, x, y);
        fails++;
    }
}

static void test_shifts(void) {
    static const ll vals[] = {
        0, 1, -1, 2, -2, 0x7fffffffffffffffLL, (ll)0x8000000000000000ULL,
        0x0123456789abcdefLL, (ll)0xfedcba9876543210ULL,
        0x00000000ffffffffLL, (ll)0xffffffff00000000ULL,
        0x0000000100000000LL, (ll)0xfffffffffffffffeULL,
        0x4000000000000000LL, 0x2000000000000000LL,
        0x0000000080000000LL, (ll)0xffffffffffffffffULL >> 1,
    };
    const long nvals = sizeof(vals) / sizeof(vals[0]);
    /* every count in the defined domain x structured values */
    for (long b = 0; b < 64; b++)
        for (long i = 0; i < nvals; i++) {
            ll u = vals[i];
            eq_ll("__ashldi3", u, b, __ashldi3_c(u, b), krell___ashldi3(u, b));
            eq_ll("__ashrdi3", u, b, __ashrdi3_c(u, b), krell___ashrdi3(u, b));
            eq_ll("__lshrdi3", u, b, __lshrdi3_c(u, b), krell___lshrdi3(u, b));
        }
    /* every count x every single-bit and sign-extended pattern */
    for (long b = 0; b < 64; b++)
        for (int bit = 0; bit < 64; bit++) {
            ll u1 = (ll)(1ULL << bit);
            ll u2 = (ll)(~0ULL << bit);
            eq_ll("__ashldi3", u1, b, __ashldi3_c(u1, b), krell___ashldi3(u1, b));
            eq_ll("__ashrdi3", u2, b, __ashrdi3_c(u2, b), krell___ashrdi3(u2, b));
            eq_ll("__lshrdi3", u1, b, __lshrdi3_c(u1, b), krell___lshrdi3(u1, b));
        }
    /* random fill */
    for (long t = 0; t < 60000; t++) {
        ll u = (ll)rnd();
        long b = rnd() % 64;
        eq_ll("__ashldi3", u, b, __ashldi3_c(u, b), krell___ashldi3(u, b));
        eq_ll("__ashrdi3", u, b, __ashrdi3_c(u, b), krell___ashrdi3(u, b));
        eq_ll("__lshrdi3", u, b, __lshrdi3_c(u, b), krell___lshrdi3(u, b));
    }
}

static void test_cmps(void) {
    static const ll vals[] = {
        0, 1, -1, 2, -2, 100, -100, 0x7fffffffffffffffLL,
        (ll)0x8000000000000000ULL, 0x00000000ffffffffLL,
        (ll)0xffffffff00000000ULL, 0x0000000100000000LL,
        (ll)0xffffffff00000001ULL, 123456789012345LL, -987654321098765LL,
    };
    const long nvals = sizeof(vals) / sizeof(vals[0]);
    for (long i = 0; i < nvals; i++)
        for (long j = 0; j < nvals; j++) {
            ll a = vals[i], b = vals[j];
            eq_ll("__cmpdi2", a, b, __cmpdi2_c(a, b), krell___cmpdi2(a, b));
            eq_ll("__ucmpdi2", a, b, __ucmpdi2_c((ull)a, (ull)b),
                  krell___ucmpdi2(a, b));
        }
    for (long t = 0; t < 60000; t++) {
        ll a = (ll)rnd(), b = (ll)rnd();
        /* bias half the cases to near-equal pairs, where compares bite */
        if (t & 1) b = a + (ll)(rnd() % 5) - 2;
        eq_ll("__cmpdi2", a, b, __cmpdi2_c(a, b), krell___cmpdi2(a, b));
        eq_ll("__ucmpdi2", a, b, __ucmpdi2_c((ull)a, (ull)b),
              krell___ucmpdi2(a, b));
    }
}

static void test_bcd(void) {
    for (int v = 0; v < 256; v++) {
        cases++;
        unsigned a = _bcd2bin_c((unsigned char)v);
        int b = krell__bcd2bin(v);
        if ((int)a != b) {
            if (fails < 8) printf("MISMATCH _bcd2bin(%d): C=%u Krell=%d\n", v, a, b);
            fails++;
        }
    }
    for (unsigned v = 0; v < 100000; v++) {
        cases++;
        unsigned a = _bin2bcd_c(v);
        int b = krell__bin2bcd((int)v);
        if ((int)a != b) {
            if (fails < 8) printf("MISMATCH _bin2bcd(%u): C=%u Krell=%d\n", v, a, b);
            fails++;
        }
    }
    for (long t = 0; t < 60000; t++) {
        unsigned v = (unsigned)rnd();
        cases++;
        unsigned a = _bin2bcd_c(v);
        int b = krell__bin2bcd((int)v);
        if ((int)a != b) {
            if (fails < 8) printf("MISMATCH _bin2bcd(%u): C=%u Krell=%d\n", v, a, b);
            fails++;
        }
    }
}

int main(void) {
    test_shifts();
    test_cmps();
    test_bcd();
    printf("cases=%ld fails=%ld -> %s\n", cases, fails,
           fails == 0 ? "DIFFTEST_OK" : "DIFFTEST_FAIL");
    return fails != 0;
}
