/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/** @file low-level cryto functions */

#ifndef _HEADER_CRYPTO_FUNCTIONS_H_
#define _HEADER_CRYPTO_FUNCTIONS_H_

#include "exception.h"

#include <stdint.h>

#include <openssl/hmac.h>
#include <openssl/rand.h>
#ifdef WIN32
extern "C" {
#include <crypto/crypto_scrypt.h>
}
#define libscrypt_scrypt crypto_scrypt
#else
#include <libscrypt.h>
#endif

// * crypto_scrypt(passwd, passwdlen, salt, saltlen, N, r, p, buf, buflen):
/**
 * calculate scrypt. see libscrypt library
 * @param secret
 * @param secret_len
 * @param salt
 * @param salt_len
 * @param N
 * @param r
 * @param p
 * @param buffer_out
 * @param buffer_out_len
 * @return 0 on success, <0 otherwise
 */
static inline int scrypt(const uint8_t* secret, size_t secret_len,
		const uint8_t* salt, size_t salt_len, uint64_t N, uint32_t r,
		uint32_t p, uint8_t* buffer_out, size_t buffer_out_len) {

	return libscrypt_scrypt(secret, secret_len, salt, salt_len, N, r, p,
			buffer_out, buffer_out_len);
}

/**
 * calculate HMAC using SHA256
 * @param key
 * @param key_len
 * @param data
 * @param data_len
 * @param output_buffer returned buffer, size is 32 bytes
 * @return pointer to output_buffer or NULL on error
 */
static inline const unsigned char* HMAC_SHA256(const uint8_t *key, size_t key_len,
		const unsigned char* data, size_t data_len,
		unsigned char* output_buffer) {

	unsigned int buffer_len;
	unsigned char* ret;
	ret = HMAC(EVP_sha256(), key, key_len, data, data_len, output_buffer,
			&buffer_len);

	DEBUG_ASSERT1(buffer_len == 32);

	return ret;
}

/**
 * secure random number generator
 * @param buffer
 * @param num
 * @return 1 on success, 0 if unsecure pseudo numbers were used, -1 on error
 */
static inline int secureRandomBytes(unsigned char* buffer, int num) {
	return RAND_bytes(buffer, num);
}

#endif /* _HEADER_CRYPTO_FUNCTIONS_H_ */
