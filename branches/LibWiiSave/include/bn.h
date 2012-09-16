#ifndef BN_H
#define BN_H

#include <tools.h>

int bn_compare(u8 *a, u8 *b, u32 n);
void bn_sub_modulus(u8 *a, u8 *N, u32 n);
void bn_add(u8 *d, u8 *a, u8 *b, u8 *N, u32 n);
void bn_mul(u8 *d, u8 *a, u8 *b, u8 *N, u32 n);
void bn_exp(u8 *d, u8 *a, u8 *N, u32 n, u8 *e, u32 en);
void bn_inv(u8 *d, u8 *a, u8 *N, u32 n);

#endif // BN_H
