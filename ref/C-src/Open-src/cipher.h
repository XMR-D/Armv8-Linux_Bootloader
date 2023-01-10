#ifndef CIPHER_H
#define CIPHER_H

void modExp(struct bn*  x, struct bn*   e, int eBits, struct bn*  m, int mBits, struct bn*  r2m,  struct bn* out);

#endif /* ! CIPHER_C */