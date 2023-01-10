#include "../Open-src/bn.h"

#define LUT_SIZE 2049

static struct bn lut[LUT_SIZE];

void montMult(struct bn*  x, struct bn*  y, struct bn*  m, int mBits, struct bn*  out)
{

	struct bn t;
	bignum_init(&t);

	int i;
	for(i = mBits; i > 0 ; i--){					//efficient loop exit

		int t0Bit = bignum_getbit(&t,0);
		int xiBit = bignum_getbit(x, mBits - i);	//loop exit requires subtraction here
		int y0Bit = bignum_getbit(y,0);
		int op = t0Bit + (xiBit * y0Bit);

		if(xiBit == 1){
			bignum_add(&t, y, &t);
		}

		if(op == 1){
			bignum_add(&t, m, &t);
		}

		bignum_rshift(&t,&t, 1);
	}

	if(bignum_cmp(&t, m) >= 0){
		bignum_sub(&t,m,&t);
	}

	bignum_assign(out,&t);
}



/*mod exp, no LUT */

void modExp(struct bn*  x, struct bn*   e, int eBits, struct bn*  m, int mBits, struct bn*  r2m,  struct bn*   out){

	struct bn z,one;
	struct bn parr[3];
	struct bn zarr[3];

	//reduce z?
	bignum_from_int(&z, 1);
	montMult(&z,r2m,m, mBits, &zarr[1]);

	//reduce x, assign to p
	montMult(x,r2m,m, mBits,&parr[1]);

	struct bn tm;

	int i = 0;
	for(; i < eBits; i++)
  {

		bignum_assign(&tm, &parr[1]);
		montMult(&tm,&parr[1],m, mBits, &parr[2]);

		if(bignum_getbit(e, i) == 1)
    {
			montMult(&zarr[1],&parr[1],m,mBits,&zarr[2]);
		}
    else
    {
			bignum_assign(&zarr[2],&zarr[1]);
		}

		//printf("num bits p: %d, num bits z: %d\n", bignum_numbits(&parr[1]), bignum_numbits(&zarr[1]));
		bignum_assign(&parr[1], &parr[2]);
		bignum_assign(&zarr[1], &zarr[2]);
	}

	bignum_from_int(&one, 1);
	montMult(&zarr[1], &one, m, mBits, out);
}



int main(void) 
{

    struct bn r2m;
    int e1 = 65537;

    struct bn result;
	bignum_init(&result);
    
	/* ----------- 1024-bit Modulus Test -------------- */

	  struct bn n1024;
    struct bn pub1;
    struct bn priv1;
    struct bn v2Dec1;
    struct bn hash_t;

//e == pub1
//n == n1024
//s == v2Dec1

	char str4[] = "b3525698549eb6c98868c4d7b227b9fdc4051f224aa9d21320feda96dac840d43c6c751659eddc635bc7e254e4d697f9d8f1e8dbbf3dc2267f411d63a8c157ae7541b912f00e4198459a1973ff900379d3fa8a6094bc445c904ed6f51e715cc05748003b389919596f5c29ec5295928555d30c6900e4cfdd4e1e007233b834cf";
	bignum_from_string(&n1024,str4, 256);

	char str5[] = "10001";
	bignum_from_string(&pub1,str5, 8);

  //value to decipher i guess
	char str6[] = "9eb6b4a08cc22ef448a59e83d58914117ad9491b89f14522ea0a314ec9f997379499cd14f149038ffc3fe4297d10ed5056505fc2de18905e71647db546d6c3dbb03404beea4ffc1a549d399bc4efa57aecf1085d4560a2495d3ddd8fbacb2080d5251f0b282f5df19dea68552e50908dfadca3c46dfa9a6bc95c6f48070a7497";
	bignum_from_string(&v2Dec1,str6,256);

  //??? montgomry compute val
	char r2ms1[] = "7796e166030edb64bfd6b6d3e0be9562991e679dc9778e15d205420108cab70e63817cc66cac94b6a24a762e2320a3dbe8059a6f053ab3bb0655ac6b0c1d97b730049bdc5d12015e76e7f69507a399e0adccdb7cdda888bee4d5cb4ffff69783e030fb9fb644e2d151902853f84d2cc0796deef9b5df2258361261e920e4e8cb";
	bignum_from_string(&r2m, r2ms1, 256);

  	char hash[] = "3e934d74927ae4d1cf260b7173cd61c2b57b38638b232edddeea2f2dff29383d";
  	bignum_from_string(&hash_t, hash, 64);
	
	int eBits = bignum_numbits(&pub1);
	int nBits = bignum_numbits(&n1024);

	bignum_init(&result);

	modExp(&v2Dec1, &pub1, eBits, &n1024, nBits, &r2m, &result);


	bignum_print(&result);
	bignum_init(&result);
  	bignum_print(&hash_t);

	return 0;
}

//TODO SHA256 + REPLACE SSCANF BY FUNCTION TO GET INT VALUE OF 8 bytes of string from ending of the string to the begginning