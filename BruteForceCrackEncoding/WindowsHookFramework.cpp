#include "WindowsHookFramework.h"
#include <exception>
typedef DWORD opcode_z_t;
#define E8CALL_PREFIX_LEN 1
#define E8CALL_LEN 5


using namespace std;

func_p_t hookIAT(HMODULE module, size_t iatDisplacement, func_p_t newFunc)
{
	return hookIAT((func_p_t*)(((PBYTE)module) + iatDisplacement), newFunc);
}

func_p_t hookIAT(func_p_t* iat, func_p_t newFunc)
{
	DWORD oldProtect;
	func_p_t ret;
	if (!VirtualProtect(iat, sizeof(func_p_t), PAGE_EXECUTE_READWRITE, &oldProtect))
		throw HookException();
	ret = *iat;
	*iat = newFunc;
	if (!VirtualProtect(iat, sizeof(func_p_t), oldProtect, &oldProtect))
		throw HookException();
	return ret;
}

func_p_t hookE8Call(HMODULE module, size_t e8argDisplacement, func_p_t newFunc)
{
	return hookE8Call((pins_t)(((PBYTE)module) + e8argDisplacement), newFunc);
}

func_p_t hookE8Call(pins_t e8ArgAddr, func_p_t newFunc)
{
	DWORD oldProtect;
	func_p_t ret;
	pins_t callAddr = e8ArgAddr - E8CALL_PREFIX_LEN;
	if (!VirtualProtect(callAddr, E8CALL_LEN, PAGE_EXECUTE_READWRITE, &oldProtect))
		throw HookException();
	ret = (callAddr + E8CALL_LEN + *(opcode_z_t*)e8ArgAddr);
	*(opcode_z_t*)e8ArgAddr = newFunc - callAddr - E8CALL_LEN;
	if (!VirtualProtect(callAddr, E8CALL_LEN, oldProtect, &oldProtect))
		throw HookException();
	return ret;
}