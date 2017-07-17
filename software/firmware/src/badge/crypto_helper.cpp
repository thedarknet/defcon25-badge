#include "crypto_helper.h"

void init_SHA256(const uECC_HashContext *base) {
	SHA256_HashContext *context = (SHA256_HashContext *) base;
	sha256_init(&context->ctx);
}

void update_SHA256(const uECC_HashContext *base, const uint8_t *message, unsigned message_size) {
	SHA256_HashContext *context = (SHA256_HashContext *) base;
	sha256_add(&context->ctx, message, message_size);
}

void finish_SHA256(const uECC_HashContext *base, uint8_t *hash_result) {
	SHA256_HashContext *context = (SHA256_HashContext *) base;
	//SHA256_Final(hash_result, &context->ctx);
	sha256_digest(&context->ctx, hash_result);
}
