#include <stdint.h>

#include "tools.h"
#include "Open-src/bn.h"
#include "Open-src/cipher.h"
#include "Open-src/sha256.h"
#include "rsa.h"
#include "kmain.h"

extern uint64_t Image_offset;
extern struct Info_header *infos;

#define INFO_HEADER Image_offset + 0xfe44 

#include "serial.h"

uint64_t get_8bits(char *str, uint64_t index)
{
    uint64_t val = hextoi(str + index);
    for(uint8_t i = 8; i > 0; i--)
    {
        str[index] = 0;
        index++;
    }
    return val;
}

void init_tab(char (*tab)[], char * str, uint64_t size)
{
    uint64_t i = 0;
    while(size > 0)
    {
        (*tab)[i] = str[i];
        size--;
        i++;
    }
}

static void memset(uint8_t *src, uint8_t oc, uint64_t n)
{
    while(n > 0)
    {
        *src = oc;
        n--;
        src++;
    }
}

char *reverse(char (*ret)[], unsigned char *src, uint64_t size)
{
    char tmp[65];
    tmp[64] = 0;

    ascii_to_hex(&tmp, (uint8_t *) src, 64);
    int64_t j = 0;
    for (int64_t i = size-1; i >= 0;)
    {
        (*ret)[j] = tmp[i-1];
        (*ret)[j+1] = tmp[i];
        j += 2;
        i -= 2;
    }
    (*ret)[j+1] = 0;
    (*ret)[64] = 0;
    uint64_t to_remove = strlen(*ret);
    puts("SHA length   = ");
    putint(to_remove);
    putc('\n');
    puts("SHA (intern) = ");
    puts(*ret);
    putc('\n');
    putc('\0');
    return (char *) ret;
}


int RSA_Sig_Check(void)
{
    //INIT
    struct bn result;
	bignum_init(&result);
    struct bn n;
    struct bn e;
    struct bn Sig;
    struct bn hash_t;
    struct bn r2m;
    uint64_t shift = 10;


    char sig[257];
    sig[256] = 0;

    uint8_t *s = (uint8_t *) INFO_HEADER + shift;

    shift += 128;


    init_tab(&sig, (char *) s, 256);
	bignum_from_string(&Sig, sig, 256, 1);

    char pub1[9];
    pub1[8] = 0;

    init_tab(&pub1, "10001", 5);
	bignum_from_string(&e, pub1, 8, 0);

    char n1024[257];
    n1024[256] = 0;
    uint8_t *n_v = (uint8_t *) INFO_HEADER + shift;
    shift += 128;
    init_tab(&n1024, (char *) n_v, 256);
	bignum_from_string(&n, n1024, 256, 1);


    char R[257];
    R[256] = 0;
    uint8_t *r_v = (uint8_t *) INFO_HEADER + shift;
    init_tab(&R, (char *) r_v, 256);
	bignum_from_string(&r2m, R, 256, 1);

    unsigned char hash[64];
    struct ctx hash_value;
    memset((uint8_t *) Image_offset + 0xfe4e, 0, 384);
    sha256_init(&hash_value);
    sha256_update(&hash_value, (uint8_t *) Image_offset, infos->Image_size);
    sha256_final(&hash_value, hash);

    char final[64];
    reverse(&final, hash, 64);
  	bignum_from_string(&hash_t, final, 64, 0);

    
    int eBits = bignum_numbits(&e);
	int nBits = bignum_numbits(&n);

    bignum_init(&result);
	modExp(&Sig, &e, eBits, &n, nBits, &r2m, &result);

    if(bignum_cmp(&result, &hash_t) == EQUAL)
    {
        return 1;
    }
    return 0;

}

