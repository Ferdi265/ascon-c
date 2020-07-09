#include "api.h"
#include "endian.h"
#include "permutations.h"

#define RATE (64 / 8)
#define PA_ROUNDS 12
#define PB_ROUNDS 6
#define IV                                                        \
  ((u64)(8 * (CRYPTO_KEYBYTES)) << 56 | (u64)(8 * (RATE)) << 48 | \
   (u64)(PA_ROUNDS) << 40 | (u64)(PB_ROUNDS) << 32)

int crypto_aead_decrypt(unsigned char* m, unsigned long long* mlen,
                        unsigned char* nsec, const unsigned char* c,
                        unsigned long long clen, const unsigned char* ad,
                        unsigned long long adlen, const unsigned char* npub,
                        const unsigned char* k) {
  if (clen < CRYPTO_ABYTES) {
    *mlen = 0;
    return -1;
  }

  const u64 K0 = LOAD64(k);
  const u64 K1 = LOAD64(k + 8);
  const u64 N0 = LOAD64(npub);
  const u64 N1 = LOAD64(npub + 8);
  state s;
  u64 i;
  (void)nsec;

  /* set plaintext size */
  *mlen = clen - CRYPTO_ABYTES;

  /* initialization */
  s.x0 = IV;
  s.x1 = K0;
  s.x2 = K1;
  s.x3 = N0;
  s.x4 = N1;
  P12();
  s.x3 ^= K0;
  s.x4 ^= K1;

  /* process associated data */
  if (adlen) {
    while (adlen >= RATE) {
      s.x0 ^= LOAD64(ad);
      P6();
      adlen -= RATE;
      ad += RATE;
    }
    for (i = 0; i < adlen; ++i, ++ad) s.x0 ^= INS_BYTE64(*ad, i);
    s.x0 ^= INS_BYTE64(0x80, adlen);
    P6();
  }
  s.x4 ^= 1;

  /* process plaintext */
  clen -= CRYPTO_ABYTES;
  while (clen >= RATE) {
    STORE64(m, s.x0 ^ LOAD64(c));
    s.x0 = LOAD64(c);
    P6();
    clen -= RATE;
    m += RATE;
    c += RATE;
  }
  for (i = 0; i < clen; ++i, ++m, ++c) {
    *m = EXT_BYTE64(s.x0, i) ^ *c;
    s.x0 &= ~INS_BYTE64(0xff, i);
    s.x0 |= INS_BYTE64(*c, i);
  }
  s.x0 ^= INS_BYTE64(0x80, clen);

  /* finalization */
  s.x1 ^= K0;
  s.x2 ^= K1;
  P12();
  s.x3 ^= K0;
  s.x4 ^= K1;

  /* verify tag (should be constant time, check compiler output) */
  if (((s.x3 ^ LOAD64(c)) | (s.x4 ^ LOAD64(c + 8))) != 0) {
    *mlen = 0;
    return -1;
  }

  return 0;
}
