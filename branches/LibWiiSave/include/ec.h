#ifndef EC_H
#define EC_H
#include "tools.h"

int check_ec( u8 *ng, u8 *ap, u8 *sig, u8 *sig_hash );
void make_ec_cert( u8 *cert, u8 *sig, char *signer, char *name, u8 *priv, u32 key_id );
void generate_ecdsa( u8 *R, u8 *S, u8 *k, u8 *hash );

#endif // EC_H

