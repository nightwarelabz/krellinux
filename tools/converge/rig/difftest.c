/* Auto-Convergence Engine -- differential rig for crc16.
 * C side: the kernel's own lib/crc16.c (symbol renamed to crc16_c).
 * Krell side: crc16.krell via krellc (symbol renamed to crc16_krell).
 * Agreement required on every case: outputs compared exactly. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

u16 crc16_c(u16 crc, const u8 *buffer, size_t len);
long crc16_krell(long crc, const u8 *buf, long len);

static uint64_t rng_state = 0x9E3779B97F4A7C15ULL;
static uint64_t rnd(void) {
    uint64_t z = (rng_state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

static long cases = 0, fails = 0;

static void check(u16 init, const u8 *buf, long len, const char *tag) {
    u16 a = crc16_c(init, buf, (size_t)len);
    long b = crc16_krell(init, buf, len) & 0xFFFF;
    cases++;
    if (a != (u16)b) {
        if (fails < 5)
            printf("MISMATCH %s init=%04x len=%ld: C=%04x Krell=%04lx\n",
                   tag, init, len, a, b);
        fails++;
    }
}

int main(void) {
    static u8 buf[8192];
    /* the standard check vector */
    check(0, (const u8 *)"123456789", 9, "check-vector");
    printf("crc16(\"123456789\", init 0) = %04x (C side)\n",
           crc16_c(0, (const u8 *)"123456789", 9));
    check(0, buf, 0, "empty");
    check(0xFFFF, buf, 0, "empty-ffff");
    for (int t = 0; t < 3000; t++) {
        long len = rnd() % 300;
        for (long i = 0; i < len; i++) buf[i] = (u8)rnd();
        u16 init = (u16)rnd();
        check(init, buf, len, "random");
    }
    for (int t = 0; t < 60; t++) {
        long len = 1024 + rnd() % 7168;
        for (long i = 0; i < len; i++) buf[i] = (u8)rnd();
        check((u16)rnd(), buf, len, "long");
    }
    /* chained: crc of a stream split at random points */
    for (int t = 0; t < 500; t++) {
        long len = 1 + rnd() % 512;
        for (long i = 0; i < len; i++) buf[i] = (u8)rnd();
        long cut = rnd() % (len + 1);
        u16 a = crc16_c(0, buf, (size_t)cut);
        a = crc16_c(a, buf + cut, (size_t)(len - cut));
        long b = crc16_krell(0, buf, cut);
        b = crc16_krell(b, buf + cut, len - cut) & 0xFFFF;
        cases++;
        if (a != (u16)b) {
            if (fails < 5) printf("MISMATCH chained len=%ld cut=%ld\n", len, cut);
            fails++;
        }
    }
    printf("cases=%ld fails=%ld -> %s\n", cases, fails,
           fails == 0 ? "DIFFTEST_OK" : "DIFFTEST_FAIL");
    return fails != 0;
}
