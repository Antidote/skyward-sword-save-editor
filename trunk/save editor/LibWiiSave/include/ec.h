#ifndef EC_H
#define EC_H
#include <Types.hpp>

bool  check_ec      ( Uint8 *ng, Uint8 *ap, Uint8 *sig, Uint8 *sig_hash );
void  make_ec_cert  ( Uint8 *cert, Uint8 *sig, char *signer, char *name, Uint8 *priv, Uint32 key_id );
void  generate_ecdsa( Uint8 *R, Uint8 *S, Uint8 *k, Uint8 *hash );

#endif // EC_H

