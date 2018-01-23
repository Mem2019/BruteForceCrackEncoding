#include <windows.h>
#include <stdio.h>
#include "BruteForce.h"
typedef int (__cdecl *encrypt_str_t)(char *a1, int a2, unsigned int *a3);
typedef void(*encode_t)();
encrypt_str_t encrypt_str;
unsigned int key = 0xEFBEADDE;
unsigned char ans[] = { 0x0BF,0x0F1,0x6A,0x2C,0x10,0x0B,0x16,0x59,0x0BA,0x3A,0x8C,0x49,0x5,0x1B,0x4,0x0E2,0x85,0x0D5,0x0C2,0x0FC,0x0D7,0x9B,0x0E9,0x42,0x0,0x0,0x0,0x0 };
void encode();
CrackCtf<encode_t> crack(0x25, ans, strlen((char*)ans), encode, 2);
void printHex(bfbyte* input, size_t inputLen)
{
	for (size_t i = 0; i < inputLen; i++)
	{
		printf("%02x ", input[i]);
	}
	printf("\n");
}
void encode()
{
	bfbyte input[0x25];
	int len = crack.getInput(input);
	if (rand() % 4096 == 0)
	{
		printHex(input, len);
	}
	encrypt_str((char*)input, len, &key);
	if (crack.testEncodeResult(input))
	{
		crack.getInput(input);
		printf("%s\n", input);
		system("pause");
	}
}
int main()
{
	HMODULE hMod = LoadLibraryA("zctf20171.dll");
	if (hMod == NULL)
	{
		printf("cannot LoadLibrary");
		return -1;
	}
	encrypt_str = (encrypt_str_t)GetProcAddress(hMod, "encrypt_str");
	char buf[256];
	strcpy_s(buf, 256, "zctf45078");
	encrypt_str(buf, strlen(buf), &key);
	//return 0;
	crack.startCrack();
}