#ifndef RSA_H
#define RSA_H

#include <stdint.h>

int RSA_Sig_Check(void);
void init_tab(char (*tab)[], char * str, uint64_t size);
uint64_t get_8bits(char *str, uint64_t index);
char *reverse(char (*ret)[], unsigned char *src, uint64_t size);


#endif /*! RSA_H*/