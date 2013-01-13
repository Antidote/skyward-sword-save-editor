#ifndef BN_H
#define BN_H

#include <Types.hpp>

int  bn_compare(Uint8 *a, Uint8 *b, Uint32 n);
void bn_sub_modulus(Uint8 *a, Uint8 *N, Uint32 n);
void bn_add(Uint8 *d, Uint8 *a, Uint8 *b, Uint8 *N, Uint32 n);
void bn_mul(Uint8 *d, Uint8 *a, Uint8 *b, Uint8 *N, Uint32 n);
void bn_exp(Uint8 *d, Uint8 *a, Uint8 *N, Uint32 n, Uint8 *e, Uint32 en);
void bn_inv(Uint8 *d, Uint8 *a, Uint8 *N, Uint32 n);

#endif // BN_H
