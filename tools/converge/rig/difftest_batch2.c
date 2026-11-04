/* Auto-Convergence Engine -- differential rig, Krellinux Phase 1 batch 2.
 * Functions under proof (kernel originals vs their Krell forms):
 *   crc_ccitt   (lib/crc/crc-ccitt.c)   crc_itu_t (lib/crc/crc-itu-t.c)
 *   crc4        (lib/crc/crc4.c)        crc7_be   (lib/crc/crc7.c)
 *   crc8, crc8_populate_msb, crc8_populate_lsb    (lib/crc/crc8.c)
 *   __muldi3    (lib/muldi3.c)
 * C side: the kernel sources compiled as-is, symbols renamed *_c.
 * Krell side: the .krell conversions through krellc, symbols krell_*.
 * Agreement is exact or the conversion does not integrate.
 * crc4's proven domain is bit counts 0..63 (the C mask expression
 * is undefined outside it). __muldi3 is additionally checked
 * against a 128-bit oracle computed in this rig.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef long long ll;

/* C originals (renamed) */
u16 crc_ccitt_c(u16 crc, const u8 *buf, size_t len);
u16 crc_itu_t_c(u16 crc, const u8 *buf, size_t len);
uint8_t crc4_c(uint8_t c, uint64_t x, int bits);
u8 crc7_be_c(u8 crc, const u8 *buf, size_t len);
void crc8_populate_msb_c(u8 *table, u8 poly);
void crc8_populate_lsb_c(u8 *table, u8 poly);
u8 crc8_c(const u8 *table, const u8 *p, size_t n, u8 crc);
ll __muldi3_c(ll u, ll v);

/* Krell forms (renamed) */
long krell_crc_ccitt(long crc, const u8 *buf, long len);
long krell_crc_itu_t(long crc, const u8 *buf, long len);
long krell_crc4(long c, long x, long bits);
long krell_crc7_be(long crc, const u8 *buf, long len);
void krell_crc8_populate_msb(u8 *table, long poly);
void krell_crc8_populate_lsb(u8 *table, long poly);
long krell_crc8(const u8 *table, const u8 *p, long n, long crc);
ll krell___muldi3(ll u, ll v);

static uint64_t rng_state = 0x9E3779B97F4A7C15ULL;
static uint64_t rnd(void) {
    uint64_t z = (rng_state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

static long cases = 0, fails = 0;

static void eq(const char *fn, long long a, long long b, long long x, long long y) {
    cases++;
    if (x != y) {
        if (fails < 8)
            printf("MISMATCH %s(%lld, %lld): C=%lld Krell=%lld\n", fn, a, b, x, y);
        fails++;
    }
}

static void test_crc16s(void) {
    static u8 buf[8192];
    printf("crc_ccitt(\"123456789\", init 0) = %04x (C side)\n",
           crc_ccitt_c(0, (const u8 *)"123456789", 9));
    printf("crc_itu_t(\"123456789\", init 0) = %04x (C side)\n",
           crc_itu_t_c(0, (const u8 *)"123456789", 9));
    static const u16 inits[] = {0, 0xffff, 0x1d0f, 0x8408, 0x5555};
    for (unsigned k = 0; k < sizeof(inits) / sizeof(inits[0]); k++) {
        eq("crc_ccitt", inits[k], 9, crc_ccitt_c(inits[k], (const u8 *)"123456789", 9),
           krell_crc_ccitt(inits[k], (const u8 *)"123456789", 9) & 0xffff);
        eq("crc_itu_t", inits[k], 9, crc_itu_t_c(inits[k], (const u8 *)"123456789", 9),
           krell_crc_itu_t(inits[k], (const u8 *)"123456789", 9) & 0xffff);
    }
    for (int t = 0; t < 3000; t++) {
        long len = rnd() % 300;
        for (long i = 0; i < len; i++) buf[i] = (u8)rnd();
        u16 init = (u16)rnd();
        eq("crc_ccitt", init, len, crc_ccitt_c(init, buf, len),
           krell_crc_ccitt(init, buf, len) & 0xffff);
        eq("crc_itu_t", init, len, crc_itu_t_c(init, buf, len),
           krell_crc_itu_t(init, buf, len) & 0xffff);
        eq("crc7_be", init & 0xff, len, crc7_be_c((u8)init, buf, len),
           krell_crc7_be(init & 0xff, buf, len) & 0xff);
    }
    for (int t = 0; t < 60; t++) {
        long len = 1024 + rnd() % 7168;
        for (long i = 0; i < len; i++) buf[i] = (u8)rnd();
        u16 init = (u16)rnd();
        eq("crc_ccitt", init, len, crc_ccitt_c(init, buf, len),
           krell_crc_ccitt(init, buf, len) & 0xffff);
        eq("crc_itu_t", init, len, crc_itu_t_c(init, buf, len),
           krell_crc_itu_t(init, buf, len) & 0xffff);
        eq("crc7_be", init & 0xff, len, crc7_be_c((u8)init, buf, len),
           krell_crc7_be(init & 0xff, buf, len) & 0xff);
    }
    /* chained: split streams must compose identically */
    for (int t = 0; t < 500; t++) {
        long len = 1 + rnd() % 512;
        for (long i = 0; i < len; i++) buf[i] = (u8)rnd();
        long cut = rnd() % (len + 1);
        u16 a = crc_ccitt_c(0, buf, cut);
        a = crc_ccitt_c(a, buf + cut, len - cut);
        long b = krell_crc_ccitt(0, buf, cut);
        b = krell_crc_ccitt(b & 0xffff, buf + cut, len - cut) & 0xffff;
        eq("crc_ccitt-chain", len, cut, a, b);
        u16 a2 = crc_itu_t_c(0xffff, buf, cut);
        a2 = crc_itu_t_c(a2, buf + cut, len - cut);
        long b2 = krell_crc_itu_t(0xffff, buf, cut);
        b2 = krell_crc_itu_t(b2 & 0xffff, buf + cut, len - cut) & 0xffff;
        eq("crc_itu_t-chain", len, cut, a2, b2);
    }
    printf("crc7_be(\"123456789\", init 0) = %02x (C side)\n",
           crc7_be_c(0, (const u8 *)"123456789", 9));
}

static void test_crc4(void) {
    static const uint64_t xs[] = {
        0, 1, ~0ULL, 0xaaaaaaaaaaaaaaaaULL, 0x5555555555555555ULL,
        0x0123456789abcdefULL, 0x8000000000000000ULL, 0xfedcba9876543210ULL,
    };
    for (int bits = 0; bits < 64; bits++)
        for (unsigned k = 0; k < sizeof(xs) / sizeof(xs[0]); k++)
            for (int c = 0; c < 16; c += 5)
                eq("crc4", (ll)xs[k], bits, crc4_c((uint8_t)c, xs[k], bits),
                   krell_crc4(c, (long)xs[k], bits) & 0xff);
    for (long t = 0; t < 60000; t++) {
        uint64_t x = rnd();
        int bits = rnd() % 64;
        int c = rnd() % 16;
        eq("crc4", (ll)x, bits, crc4_c((uint8_t)c, x, bits),
           krell_crc4(c, (long)x, bits) & 0xff);
    }
}

static void test_crc8(void) {
    static const u8 polys[] = {0x07, 0x31, 0x9b, 0xa6, 0x1d, 0xd5, 0x2f, 0x39, 0xff, 0x00};
    static u8 buf[4096];
    for (unsigned k = 0; k < sizeof(polys) / sizeof(polys[0]); k++) {
        u8 tc[256], tk[256];
        memset(tc, 0xa5, sizeof tc);
        memset(tk, 0xa5, sizeof tk);
        crc8_populate_msb_c(tc, polys[k]);
        krell_crc8_populate_msb(tk, polys[k]);
        cases++;
        if (memcmp(tc, tk, 256)) {
            if (fails < 8) printf("MISMATCH crc8_populate_msb(poly=%02x)\n", polys[k]);
            fails++;
        }
        memset(tc, 0x5a, sizeof tc);
        memset(tk, 0x5a, sizeof tk);
        crc8_populate_lsb_c(tc, polys[k]);
        krell_crc8_populate_lsb(tk, polys[k]);
        cases++;
        if (memcmp(tc, tk, 256)) {
            if (fails < 8) printf("MISMATCH crc8_populate_lsb(poly=%02x)\n", polys[k]);
            fails++;
        }
        /* crc8 over random buffers, both table kinds (C-populated) */
        u8 tmsb[256], tlsb[256];
        crc8_populate_msb_c(tmsb, polys[k]);
        crc8_populate_lsb_c(tlsb, polys[k]);
        for (int t = 0; t < 400; t++) {
            long len = rnd() % 300;
            for (long i = 0; i < len; i++) buf[i] = (u8)rnd();
            u8 init = (u8)rnd();
            eq("crc8-msb", polys[k], len, crc8_c(tmsb, buf, len, init),
               krell_crc8(tmsb, buf, len, init) & 0xff);
            eq("crc8-lsb", polys[k], len, crc8_c(tlsb, buf, len, init),
               krell_crc8(tlsb, buf, len, init) & 0xff);
        }
    }
}

static void test_muldi3(void) {
    static const ll vals[] = {
        0, 1, -1, 2, -2, 0x10000, 0xffffffffLL, 0x100000000LL,
        0x7fffffffffffffffLL, (ll)0x8000000000000000ULL,
        0x0123456789abcdefLL, (ll)0xfedcba9876543210ULL,
        3037000500LL, -3037000500LL, 0x5555555555555555LL,
    };
    const long n = sizeof(vals) / sizeof(vals[0]);
    for (long i = 0; i < n; i++)
        for (long j = 0; j < n; j++) {
            ll u = vals[i], v = vals[j];
            ll oracle = (ll)(unsigned long long)((unsigned __int128)(uint64_t)u *
                                                  (unsigned __int128)(uint64_t)v);
            eq("__muldi3", u, v, __muldi3_c(u, v), krell___muldi3(u, v));
            eq("__muldi3-oracle", u, v, oracle, krell___muldi3(u, v));
        }
    for (long t = 0; t < 100000; t++) {
        ll u = (ll)rnd(), v = (ll)rnd();
        ll oracle = (ll)(unsigned long long)((unsigned __int128)(uint64_t)u *
                                              (unsigned __int128)(uint64_t)v);
        eq("__muldi3", u, v, __muldi3_c(u, v), krell___muldi3(u, v));
        eq("__muldi3-oracle", u, v, oracle, krell___muldi3(u, v));
    }
}

int main(void) {
    test_crc16s();
    test_crc4();
    test_crc8();
    test_muldi3();
    printf("cases=%ld fails=%ld -> %s\n", cases, fails,
           fails == 0 ? "DIFFTEST_OK" : "DIFFTEST_FAIL");
    return fails != 0;
}
