#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <vector>
#include "BruteForce.h"
#include "WindowsHookFramework.h"

typedef int (*main_t)();
#define GETS_IAT 0x40DC
#define MAIN_DISPL 0x13E0
#define FWRITE_IAT 0x4060
#define SYSTEM_IAT 0x4068
#define FOPEN_IAT 0x406C
#define FCLOSE_IAT 0x4064
#define COUT_IAT 0x4038
 
#define KEY "Itl9qnxD/IJhoarL"

#define INPUT_LEN 12

main_t hismain;

class CrackCtf : public BruteForce
{
public:
	CrackCtf(size_t inputLen, const bfbyte* answer, size_t answerLen)
		:BruteForce(inputLen, answer, answerLen){}
	~CrackCtf();
private:

	virtual void doEncode() override
	{
		hismain();
	}
public:
	static size_t __cdecl myfwrite(char *str, size_t Size, size_t Count, FILE *File);
};
CrackCtf::~CrackCtf(){}

CrackCtf crack(INPUT_LEN, (const bfbyte*)KEY, strlen(KEY));
//本应该是单例模式，但是CTF比赛的时间不可能给你写单例

size_t CrackCtf::myfwrite(char * str, size_t Size, size_t Count, FILE * File)
{
		//printf("%s\n", str);
		if (crack.testEncodeResult((bfbyte*)str))
		{
			bfbyte buf[INPUT_LEN + 1];
			memset(buf, 0, INPUT_LEN + 1);
			crack.getInput(buf, INPUT_LEN + 1);
			MessageBoxA(NULL, (LPCSTR)buf, "flag", MB_OK);
		}
	
	return 0;
}

char* __cdecl mygets(char* buffer)
{
	crack.getInput((bfbyte*)buffer);
	//printf("input: %s\n", buffer);
// 	for (char* i = buffer; *i; i++)
// 	{
// 		printf("%x ", (bfbyte)*i);
// 	}
// 	printf("\n");
	return buffer;
}

DECLARE_EMPTY_FUNC(emptyfunc, 0)
DECLARE_EMPTY_FUNC(emptyfunc2, 4)
int main()
{
	HMODULE pBase = LoadLibraryA("Encrypt messages.dll");

	hookIAT(pBase, GETS_IAT, (func_p_t)&mygets);
	hookIAT(pBase, FWRITE_IAT, (func_p_t)&CrackCtf::myfwrite);
	hookIAT(pBase, SYSTEM_IAT, (func_p_t)&emptyfunc);
	hookIAT(pBase, FOPEN_IAT, (func_p_t)&emptyfunc);
	hookIAT(pBase, FCLOSE_IAT, (func_p_t)&emptyfunc);
	hookIAT(pBase, COUT_IAT, (func_p_t)&emptyfunc2);
	
	hismain = (main_t)((PBYTE)pBase + MAIN_DISPL);

	crack.startCrack();

	return 0;
}