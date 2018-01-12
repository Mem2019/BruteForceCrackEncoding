#include <stdint.h>

#define AES_BLOCK_SIZE 16u
size_t aesEncrypt(uint8_t* input, size_t inputLen,
	uint8_t* output, size_t outputLen, uint8_t* key/*256 bits key*/);