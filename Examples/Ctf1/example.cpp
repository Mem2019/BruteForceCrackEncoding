#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <vector>
#include "BruteForce.h"
#include "WindowsHookFramework.h"

typedef int(*main_t)();
#define GETS_IAT 0x40DC
#define MAIN_DISPL 0x13E0
#define FWRITE_IAT 0x4060
#define SYSTEM_IAT 0x4068
#define FOPEN_IAT 0x406C
#define FCLOSE_IAT 0x4064
#define COUT_IAT 0x4038
#define OUTPUT_STR_E8ARG 0x1445

#define KEY "Itl9qnxD/IJhoarL"

#define INPUT_LEN 12

main_t hismain;



CrackCtf<main_t> crack(INPUT_LEN, (const bfbyte*)KEY, strlen(KEY));
//本应该是单例模式，但是CTF比赛的时间不可能给你写单例
//第一个参数是所需要破解的最大长度，这个我是自己试出来的（看多长的输入才能跟要求的答案一样长并且没有等于号（因为这是魔改base64）。。。
//后面两个参数是密文和密文的长度
size_t myfwrite(char * str, size_t Size, size_t Count, FILE * File)
{
	//printf("%s\n", str);
	if (crack.testEncodeResult((bfbyte*)str))
	{//调用testEncodeResult，str就是加密结果
	 //如果返回true，说明爆破出了正确答案，这个时候弹出当前输入
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
	//get当前的Input，并且塞到缓冲区里面
	return buffer;
}

DECLARE_EMPTY_FUNC(emptyfunc, 0)
DECLARE_EMPTY_FUNC(emptyfunc2, 4)
int main()
{
	HMODULE pBase = LoadLibraryA("Encrypt messages.dll");
	//Encrypt messages is an exe crack me in one CTF competition
	//change the characteristic to dll, and set the entry address to 0, it can become a dll
	if (pBase == NULL)
	{
		MessageBoxA(NULL, "cannot open dll!", "error", MB_OK);
		return -1;
	}
	hookIAT(pBase, GETS_IAT, (func_p_t)&mygets);
	hookIAT(pBase, FWRITE_IAT, (func_p_t)&myfwrite);
	hookIAT(pBase, SYSTEM_IAT, (func_p_t)&emptyfunc);
	hookIAT(pBase, FOPEN_IAT, (func_p_t)&emptyfunc);
	hookIAT(pBase, FCLOSE_IAT, (func_p_t)&emptyfunc);
	hookIAT(pBase, COUT_IAT, (func_p_t)&emptyfunc2);
	hookE8Call(pBase, OUTPUT_STR_E8ARG, (func_p_t)&emptyfunc);
	//hook IAT 和 e8 call
	hismain = (main_t)((PBYTE)pBase + MAIN_DISPL);
	//算出Main函数地址
	crack.setDoEnc(hismain);
	crack.startCrack();
	//调用startCrack，开始爆破
	return 0;
}