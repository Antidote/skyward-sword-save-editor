#ifndef __AES_H_
#define __AES_H_

#ifdef __cplusplus
extern "C" {
#endif

void aes_encrypt(u8 *iv, const u8 *inbuf, u8 *outbuf, unsigned long long len );
void aes_decrypt(u8 *iv, const u8 *inbuf, u8 *outbuf, unsigned long long len );
void aes_set_key(const u8 *key );

#ifdef __cplusplus
}
#endif

#endif //__AES_H_


