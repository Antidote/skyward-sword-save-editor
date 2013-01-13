#ifndef __AES_H_
#define __AES_H_

#include <Types.hpp>

#ifdef __cplusplus
extern "C" {
#endif

void aes_encrypt(Uint8 *iv, const Uint8 *inbuf, Uint8 *outbuf, Uint64 len);
void aes_decrypt(Uint8 *iv, const Uint8 *inbuf, Uint8 *outbuf, Uint64 len);
void aes_set_key(const Uint8 *key );

#ifdef __cplusplus
}
#endif

#endif //__AES_H_


