#ifndef CRYPTO_HELPER_H
#define CRYPTO_HELPER_H

#include <uECC.h>
#include <sha256.h>

////////////////////////////////////////////////////
typedef struct SHA256_HashContext {
	uECC_HashContext uECC;
	ShaOBJ ctx;
} SHA256_HashContext;

void init_SHA256(const uECC_HashContext *base);

void update_SHA256(const uECC_HashContext *base, const uint8_t *message, unsigned message_size);

void finish_SHA256(const uECC_HashContext *base, uint8_t *hash_result);

#endif
