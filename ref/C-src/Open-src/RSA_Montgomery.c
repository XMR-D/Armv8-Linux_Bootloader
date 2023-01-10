/*
 ============================================================================
 Name        : RSA_Montgomery.c
 Author      : Tim Salomonsson
 Version     :
 Copyright   :
 Description : RSA using Montgomery Reduction
 ============================================================================
 */


#include "bn.h"

#define LUT_SIZE 2049

/*
 * Performs bitwise Montgomery modular multiplication ( X*Y*R^(-1) mod M)
 *
 * Parameters:
 * 		x,y,m - bignums
 * 		mBits - # of bits in m
 * 		out	  - bignum result
 */

void montMult(struct bn*  x, struct bn*  y, struct bn*  m, int mBits, struct bn*  out){

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

void modExp(struct bn*  x, struct bn*   e, int eBits, struct bn*  m, int mBits, struct bn*  r2m,  struct bn*   out)
{

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
	for(; i < eBits; i++){

		bignum_assign(&tm, &parr[1]);
		montMult(&tm,&parr[1],m, mBits, &parr[2]);

		if(bignum_getbit(e, i) == 1){
			montMult(&zarr[1],&parr[1],m,mBits,&zarr[2]);
		}else{
			bignum_assign(&zarr[2],&zarr[1]);
		}

		bignum_assign(&parr[1], &parr[2]);
		bignum_assign(&zarr[1], &zarr[2]);
	}

	bignum_from_int(&one, 1);
	montMult(&zarr[1], &one, m, mBits, out);
}



