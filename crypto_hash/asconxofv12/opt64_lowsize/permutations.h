#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_

typedef unsigned char u8;
typedef unsigned long long u64;

typedef struct {
  u64 x0, x1, x2, x3, x4;
} state;

#define ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))
#define START_CONSTANT(x) (((0xf - (12 - (x))) << 4) | (12 - (x)))

#define ROUND(C)                    \
  do {                              \
    state t;                        \
    s.x2 ^= C;                      \
    s.x0 ^= s.x4;                   \
    s.x4 ^= s.x3;                   \
    s.x2 ^= s.x1;                   \
    t.x0 = s.x0;                    \
    t.x4 = s.x4;                    \
    t.x3 = s.x3;                    \
    t.x1 = s.x1;                    \
    t.x2 = s.x2;                    \
    s.x0 = t.x0 ^ ((~t.x1) & t.x2); \
    s.x2 = t.x2 ^ ((~t.x3) & t.x4); \
    s.x4 = t.x4 ^ ((~t.x0) & t.x1); \
    s.x1 = t.x1 ^ ((~t.x2) & t.x3); \
    s.x3 = t.x3 ^ ((~t.x4) & t.x0); \
    s.x1 ^= s.x0;                   \
    t.x1 = s.x1;                    \
    s.x1 = ROTR64(s.x1, 39);        \
    s.x3 ^= s.x2;                   \
    t.x2 = s.x2;                    \
    s.x2 = ROTR64(s.x2, 1);         \
    t.x4 = s.x4;                    \
    t.x2 ^= s.x2;                   \
    s.x2 = ROTR64(s.x2, 6 - 1);     \
    t.x3 = s.x3;                    \
    t.x1 ^= s.x1;                   \
    s.x3 = ROTR64(s.x3, 10);        \
    s.x0 ^= s.x4;                   \
    s.x4 = ROTR64(s.x4, 7);         \
    t.x3 ^= s.x3;                   \
    s.x2 ^= t.x2;                   \
    s.x1 = ROTR64(s.x1, 61 - 39);   \
    t.x0 = s.x0;                    \
    s.x2 = ~s.x2;                   \
    s.x3 = ROTR64(s.x3, 17 - 10);   \
    t.x4 ^= s.x4;                   \
    s.x4 = ROTR64(s.x4, 41 - 7);    \
    s.x3 ^= t.x3;                   \
    s.x1 ^= t.x1;                   \
    s.x0 = ROTR64(s.x0, 19);        \
    s.x4 ^= t.x4;                   \
    t.x0 ^= s.x0;                   \
    s.x0 = ROTR64(s.x0, 28 - 19);   \
    s.x0 ^= t.x0;                   \
  } while (0)

void P(state *p, u8 rounds);

#endif /* PERMUTATIONS_H_ */
