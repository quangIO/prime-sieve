#pragma once

//
// Created by quangio on 10/19/19.
//
#include <stdbool.h>
#include <stdio.h>

#define pc(x, f) fputc_unlocked(x, f)
#define WORD_BITS 64u
#define LOG_BITS 6u

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

static void setBit(size_t *b, unsigned int idx) {
#ifdef COMPRESSED
    if (!(idx & 1u)) return;
    idx >>= 1u;
#endif
    b[idx >> LOG_BITS] |= (1ull << (idx % WORD_BITS));
}

static bool getBit(const size_t *b, unsigned int idx) {
#ifdef COMPRESSED
    if (!(idx & 1u)) return 1;
    idx >>= 1u;
#endif
    return (b[idx >> LOG_BITS]) & (1ull << (idx % WORD_BITS));
}

static void writeInt(unsigned int n, FILE *f) { // fastIO based on some CodeChef submissions
    unsigned int N = n, rev, count = 0;
    if (!N) {
        pc('0', f);
        pc(' ', f);
        return;
    }
    rev = N;
    while (rev % 10 == 0) count++, rev /= 10;
    rev = 0;
    while (N) {
        rev = (rev << 3u) + (rev << 1u) + N % 10;
        N /= 10;
    }
    while (rev != 0) {
        pc(rev % 10 + '0', f);
        rev /= 10;
    }
    while (count--) pc('0', f);
    pc(' ', f);
}
