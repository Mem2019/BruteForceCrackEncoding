#include <windows.h>
#include <cstdio>
#include <cstring>
#include "BruteForce.h"
void printHex(bfbyte* input, size_t inputLen);

//this can be changed for test purpose
#define SLOW_PRINT_SPEED 4096

//put the argument here
#define BLOCK_SIZE 1
#define ANSWER ((const bfbyte*)"123456")
#define ANSWER_LEN (strlen((const char*)ANSWER))
#define INPUT_LEN ((size_t)7)

typedef void(*encode_t)();
void encode();
CrackCtf<encode_t> crack(INPUT_LEN, ANSWER, ANSWER_LEN, encode, BLOCK_SIZE);

void encode()
{
	bfbyte input[INPUT_LEN];
	//you can also get input in your hook function
	int len = crack.getInput(input, INPUT_LEN);
	if (rand() % SLOW_PRINT_SPEED == 0)
	{
		printHex(input, len);
	}
	/*do your own encoding!
	encrypt_str((char*)input, len, &key);
	*/
	//call testEncodingResult
	//or you can delete this to call testEncodeResult in your hook function
	if (crack.testEncodeResult(input))
	{
		crack.getInput(input);
		printf("%s\n", input);
		system("pause");
	}
	//*/
}
int main()
{
	HMODULE hMod = LoadLibraryA("zctf20171.dll");
	if (hMod == NULL)
	{
		printf("cannot LoadLibrary");
		return -1;
	}
	//return 0;
	crack.startCrack();
}

void printHex(bfbyte* input, size_t inputLen)
{
	for (size_t i = 0; i < inputLen; i++)
	{
		printf("%02x ", input[i]);
	}
	printf("\n");
}