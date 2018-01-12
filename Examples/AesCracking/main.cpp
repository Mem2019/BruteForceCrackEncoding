
#include "BruteForce.h"
#include "aes.h"
#include <stdio.h>
#include <stdlib.h>
uint8_t key[] = {
	0x00, 0x01, 0x02, 0x03,
	0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13,
	0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b,
	0x1c, 0x1d, 0x1e, 0x1f };

uint8_t res[] =
{
	0x8e, 0xa2, 0xb7, 0xca, 
	0x51, 0x67, 0x45, 0xbf, 
	0xea, 0xfc, 0x49, 0x90, 
	0x4b, 0x49, 0x60, 0x89,
	0xf2, 0x90, 0x00, 0xb6, 
	0x2a, 0x49, 0x9f, 0xd0, 
	0xa9, 0xf3, 0x9a, 0x6a, 
	0xdd, 0x2e, 0x77, 0x80
};

class CrackAes : public BruteForce
{
public:
	CrackAes(size_t inputLen, const bfbyte* answer, size_t answerLen)
		:BruteForce(inputLen, answer, answerLen, AES_BLOCK_SIZE)
	{
		this->outputBufLen = answerLen - answerLen % AES_BLOCK_SIZE + AES_BLOCK_SIZE;
		this->inputBufLen = inputLen;
	}
	
	~CrackAes() {};
private:
	size_t outputBufLen;
	size_t inputBufLen;
	virtual void doEncode() override
	{//继承重写doEncode函数，他必须要通过getInput获取到当前输入，然后把这个输入加密，再把加密结果作为参数调用testEncodeResult
	 //this function must call getInput to get the input, encode it, 
	 //and call testEncodeResult with the result of encoding
		uint8_t* input = new uint8_t[inputBufLen];
		uint8_t* output = new uint8_t[outputBufLen];
		size_t inputLen = getInput(input);
		if (rand() % 4096 == 0)
		{
			for (size_t i = 0; i < inputLen; i++)
			{
				printf("%02x ", input[i]);
			}
			printf("\n");
		}
		if (!aesEncrypt(input, inputLen, output, outputBufLen, key))
		{
#ifdef _DEBUG
			throw 0;
#endif // DEBUG
		}
		this->testEncodeResult(output);
		delete[] output;
		delete[] input;
	}
};


int main()
{
	CrackAes aes(17, res, AES_BLOCK_SIZE * 2);
	aes.startCrack();
}