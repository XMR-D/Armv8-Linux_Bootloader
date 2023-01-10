#include "bn.h"
#include "../tools.h"
#include "../serial.h"

/* Functions for shifting number in-place. */
static void _lshift_one_bit(struct bn* a);
static void _rshift_one_bit(struct bn* a);
static void _lshift_word(struct bn* a, int nwords);
static void _rshift_word(struct bn* a, int nwords);

void bignum_mod(struct bn* a, struct bn* b, struct bn* c)
{
  /*
    Take divmod and throw away div part
  */
  struct bn tmp;

  bignum_divmod(a,b,&tmp,c);
}

void bignum_pow(struct bn* a, struct bn* b, struct bn* c)
{


  struct bn tmp;

  bignum_init(c);

  if (bignum_cmp(b, c) == EQUAL)
  {
    /* Return 1 when exponent is 0 -- n^0 = 1 */
    bignum_inc(c);
  }
  else
  {
    /* Copy a -> tmp */
    bignum_assign(&tmp, a);

    bignum_dec(b);
 
    /* Begin summing products: */
    while (!bignum_is_zero(b))
    {

      /* c = tmp * tmp */
      bignum_mul(&tmp, a, c);
      /* Decrement b by one */
      bignum_dec(b);

      bignum_assign(&tmp, c);
    }

    /* c = tmp */
    bignum_assign(c, &tmp);
  }
}

void bignum_init(struct bn* n)
{
	register int z = z-z;
	int i = z;
	n->array[0] = z;
	for (i = BN_ARRAY_SIZE - 1; i > 0; i -=4)
	{
		n->array[i] = z;
		n->array[i-1] = z;
		n->array[i-2] = z;
		n->array[i-3] = z;
	}
}


void bignum_from_int(struct bn* n, DTYPE_TMP i)
{
	bignum_init(n);

	/* Endianness issue if machine is not little-endian? */
#ifdef WORD_SIZE
#if (WORD_SIZE == 1)
	n->array[0] = (i & 0x000000ff);
	n->array[1] = (i & 0x0000ff00) >> 8;
	n->array[2] = (i & 0x00ff0000) >> 16;
	n->array[3] = (i & 0xff000000) >> 24;
#elif (WORD_SIZE == 2)
	n->array[0] = (i & 0x0000ffff);
	n->array[1] = (i & 0xffff0000) >> 16;
#elif (WORD_SIZE == 4)
	n->array[0] = i;
	DTYPE_TMP num_32 = 32;
	DTYPE_TMP tmp = i >> num_32; /* bit-shift with U64 operands to force 64-bit results */
	n->array[1] = tmp;
#endif
#endif
}

int bignum_to_int(struct bn* n)
{
	return n->array[0];
}

void ascii_to_hex(char (*tab)[], uint8_t *offset, uint64_t length)
{
  uint64_t j = 0;
  offset += (length/2) - 1;
  for(; j < length; j += 2)
  {
      uint8_t val = *offset;
      uint8_t digit1 = val % 16;
      if (digit1 > 9)
        (*tab)[j + 1] = digit1 + 87;
      else
        (*tab)[j + 1] = digit1 + 48;
      
      val /= 16;
      uint8_t digit2 = val % 16;
      if (digit2 > 9)
        (*tab)[j] = digit2 + 87;
      else
        (*tab)[j] = digit2 + 48;
      offset--;
  }
  (*tab)[j] = 0;
}

static void cp(char (*tab)[], char *str)
{
    uint64_t len = strlen(str);
    uint8_t j = 0;
    while (j < len)
    {
      (*tab)[j] = str[j];
      j++;
    }
    (*tab)[j+1] = 0;
}



void bignum_from_string(struct bn* n, char* str, int nbytes, int mode)
{
	bignum_init(n);
  //modify str to fit the byte representation of it not the litteral caracters

  char str2[nbytes];

  if (mode == 1)
    ascii_to_hex(&str2, (uint8_t *) str, (uint64_t) nbytes);
  else
    cp(&str2, str);

	DTYPE tmp;                        /* DTYPE is defined in bn.h - uint{8,16,32,64}_t */
	int i = nbytes - (2 * WORD_SIZE); /* index into string */
	int j = 0;                        /* index into array */

	/* reading last hex-byte "MSB" from string first -> big endian */
	/* MSB ~= most significant byte / block ? :) */
	while (i >= 0)
	{
		tmp = 0;
    tmp = get_8bits(str2, i);
    str2[i] = 0;
		n->array[j] = tmp;
		i -= (2 * WORD_SIZE); /* step WORD_SIZE hex-byte(s) back in the string. */
		j += 1;               /* step one element forward in the array. */
	}
}


void bignum_to_string(struct bn* n, char* str, int nbytes)
{
	int j = BN_ARRAY_SIZE - 1; /* index into array - reading "MSB" first -> big-endian */
	int i = 0;                 /* index into string representation. */

	/* reading last array-element "MSB" first -> big endian */
	while ((j >= 0) && (nbytes > (i + 1)))
	{
    n = n; //replace here if you want to 
		i += (2 * WORD_SIZE); /* step WORD_SIZE hex-byte(s) forward in the string. */
		j -= 1;               /* step one element back in the array. */
	}

	/* Count leading zeros: */
	j = 0;
	while (str[j] == '0')
	{
		j += 1;
	}

	/* Move string j places ahead, effectively skipping leading zeros */
	for (i = 0; i < (nbytes - j); ++i)
	{
		str[i] = str[i + j];
	}

	/* Zero-terminate string */
	str[i] = 0;
}

void bignum_add(struct bn* a, struct bn* b, struct bn* c)
{
	DTYPE_TMP tmp;
	int carry = carry - carry;
	int i = 1;

	tmp = (DTYPE_TMP)a->array[0] + b->array[0] + carry;
	carry = (tmp > MAX_VAL);
	c->array[0] = (tmp & MAX_VAL);

	for (; i < BN_ARRAY_SIZE; i+=4)
	{

		tmp = (DTYPE_TMP)a->array[i] + b->array[i] + carry;
		carry = (tmp > MAX_VAL);
		c->array[i] = (tmp & MAX_VAL);

		tmp = (DTYPE_TMP)a->array[i+1] + b->array[i+1] + carry;
		carry = (tmp > MAX_VAL);
		c->array[i+1] = (tmp & MAX_VAL);

		tmp = (DTYPE_TMP)a->array[i+2] + b->array[i+2] + carry;
		carry = (tmp > MAX_VAL);
		c->array[i+2] = (tmp & MAX_VAL);

		tmp = (DTYPE_TMP)a->array[i+3] + b->array[i+3] + carry;
		carry = (tmp > MAX_VAL);
		c->array[i+3] = (tmp & MAX_VAL);
	}
}

void bignum_sub(struct bn* a, struct bn* b, struct bn* c)
{
	DTYPE_TMP res;
	DTYPE_TMP tmp1;
	DTYPE_TMP tmp2;
	register int zero  = zero - zero;
	int borrow = zero;
	int i = zero;
	for (; i < BN_ARRAY_SIZE; ++i)
	{
		tmp1 = (DTYPE_TMP)a->array[i] + (MAX_VAL + 1); /* + number_base */
		tmp2 = (DTYPE_TMP)b->array[i] + borrow;;
		res = (tmp1 - tmp2);
		c->array[i] = (DTYPE)(res & MAX_VAL); /* "modulo number_base" == "% (number_base - 1)" if number_base is 2^N */
		borrow = (res <= MAX_VAL);
	}
}

void bignum_rshift(struct bn* a, struct bn* b, int nbits)
{
  b = b;
	/* Handle shift in multiples of word-size */
	int nwords = nbits >> 5;
	if (nwords != 0)
	{
		int z = nwords << 5;
		_rshift_word(a, nwords);
		nbits -= (z);
	}

	if (nbits != 0)
	{
		int z = 32 - nbits;
		int i;
		for (i = 0; i < (BN_ARRAY_SIZE - 1); i++)
		{
			a->array[i] = (a->array[i] >> nbits) | (a->array[i + 1] << (z));
		}
		a->array[i] >>= nbits;
	}
}
int bignum_cmp(struct bn* a, struct bn* b)
{
	int i = BN_ARRAY_SIZE;
	do
	{
		i -= 1; /* Decrement first, to start with last array element */
		if (a->array[i] > b->array[i])
		{
			return LARGER;
		}
		else if (a->array[i] < b->array[i])
		{
			return SMALLER;
		}
	}
	while (i != 0);

	return EQUAL;
}

void bignum_assign(struct bn* dst, struct bn* src)
{
	register int i = BN_ARRAY_SIZE-1;
	for (; i > 0; i-=4)		//loop opt
	{
		dst->array[i] = src->array[i];
		dst->array[i-1] = src->array[i-1];
		dst->array[i-2] = src->array[i-2];
		dst->array[i-3] = src->array[i-3];

	}
	dst->array[0] = src->array[0];

}

int bignum_getbit(struct bn* a, int n){
	int arrayInd = (n >> 5);
	int shift = (n - (arrayInd << 5));
	return (a->array[arrayInd] >> shift) & 1;
}

int bignum_numbits(struct bn* bn){

	register int f = (BN_ARRAY_SIZE << 5) -1;

	for (;f > 0; --f){
		int b = bignum_getbit(bn, f);
		if (b == 1){
			return f+1;
		}
	}
	return 0;
}

void bignum_print(struct bn* num, uint32_t size)
{
  uint32_t i = 0;
	while(size > 0)
  {
    puthex(num->array[i], 0);
    i++;
    size--;
  }
}

void bignum_mul(struct bn* a, struct bn* b, struct bn* c)
{


  struct bn row;
  struct bn tmp;
  int i, j;

  bignum_init(c);

  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    bignum_init(&row);

    for (j = 0; j < BN_ARRAY_SIZE; ++j)
    {
      if (i + j < BN_ARRAY_SIZE)
      {
        bignum_init(&tmp);
        DTYPE_TMP intermediate = ((DTYPE_TMP)a->array[i] * (DTYPE_TMP)b->array[j]);
        bignum_from_int(&tmp, intermediate);
        _lshift_word(&tmp, i + j);
        bignum_add(&tmp, &row, &row);
      }
    }
    bignum_add(c, &row, c);
  }
}


void bignum_div(struct bn* a, struct bn* b, struct bn* c)
{

  struct bn current;
  struct bn denom;
  struct bn tmp;

  bignum_from_int(&current, 1);               // int current = 1;
  bignum_assign(&denom, b);                   // denom = b
  bignum_assign(&tmp, a);                     // tmp   = a

  const DTYPE_TMP half_max = 1 + (DTYPE_TMP)(MAX_VAL / 2);
  int overflow = 0;
  while (bignum_cmp(&denom, a) != LARGER)     // while (denom <= a) {
  {
    if (denom.array[BN_ARRAY_SIZE - 1] >= half_max)
    {
      overflow = 1;
      break;
    }
    _lshift_one_bit(&current);                //   current <<= 1;
    _lshift_one_bit(&denom);                  //   denom <<= 1;
  }
  if (!overflow)
  {
    _rshift_one_bit(&denom);                  // denom >>= 1;
    _rshift_one_bit(&current);                // current >>= 1;
  }
  bignum_init(c);                             // int answer = 0;

  while (!bignum_is_zero(&current))           // while (current != 0)
  {
    if (bignum_cmp(&tmp, &denom) != SMALLER)  //   if (dividend >= denom)
    {
      bignum_sub(&tmp, &denom, &tmp);         //     dividend -= denom;
      bignum_or(c, &current, c);              //     answer |= current;
    }
    _rshift_one_bit(&current);                //   current >>= 1;
    _rshift_one_bit(&denom);                  //   denom >>= 1;
  }                                           // return answer;
}

void bignum_divmod(struct bn* a, struct bn* b, struct bn* c, struct bn* d)
{



  struct bn tmp;

  /* c = (a / b) */
  bignum_div(a, b, c);

  /* tmp = (c * b) */
  bignum_mul(c, b, &tmp);

  /* c = a - tmp */
  bignum_sub(a, &tmp, d);
}

static void _rshift_word(struct bn* a, int nwords)
{
	register int i = i-i;
	for (; i < nwords; i+=4)
	{
		a->array[i]   = a->array[i + 1];
		a->array[i+1] = a->array[i + 2];
		a->array[i+2] = a->array[i + 3];
		a->array[i+3] = a->array[i + 4];
	}
	register int z = z-z;
	for (; i < BN_ARRAY_SIZE; i+=4)
	{
		a->array[i]   = z;
		a->array[i+1] = z;
		a->array[i+2] = z;
		a->array[i+3] = z;
	}
}

/* Private / Static functions. */

static void _lshift_word(struct bn* a, int nwords)
{

  int i;
  /* Shift whole words */
  for (i = (BN_ARRAY_SIZE - 1); i >= nwords; --i)
  {
    a->array[i] = a->array[i - nwords];
  }
  /* Zero pad shifted words. */
  for (; i >= 0; --i)
  {
    a->array[i] = 0;
  }  
}


static void _lshift_one_bit(struct bn* a)
{


  int i;
  for (i = (BN_ARRAY_SIZE - 1); i > 0; --i)
  {
    a->array[i] = (a->array[i] << 1) | (a->array[i - 1] >> ((8 * WORD_SIZE) - 1));
  }
  a->array[0] <<= 1;
}


static void _rshift_one_bit(struct bn* a)
{


  int i;
  for (i = 0; i < (BN_ARRAY_SIZE - 1); ++i)
  {
    a->array[i] = (a->array[i] >> 1) | (a->array[i + 1] << ((8 * WORD_SIZE) - 1));
  }
  a->array[BN_ARRAY_SIZE - 1] >>= 1;
}

int bignum_is_zero(struct bn* n)
{


  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    if (n->array[i])
    {
      return 0;
    }
  }

  return 1;
}

void bignum_dec(struct bn* n)
{


  DTYPE tmp; /* copy of n */
  DTYPE res;

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    tmp = n->array[i];
    res = tmp - 1;
    n->array[i] = res;

    if (!(res > tmp))
    {
      break;
    }
  }
}


void bignum_inc(struct bn* n)
{

  DTYPE res;
  DTYPE_TMP tmp; /* copy of n */

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    tmp = n->array[i];
    res = tmp + 1;
    n->array[i] = res;

    if (res > tmp)
    {
      break;
    }
  }
}

void bignum_or(struct bn* a, struct bn* b, struct bn* c)
{


  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    c->array[i] = (a->array[i] | b->array[i]);
  }
}