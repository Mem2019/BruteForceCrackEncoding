#pragma once
#include <windows.h>
#include <exception>
typedef unsigned char* func_p_t;
typedef unsigned char* pins_t;
func_p_t hookIAT(HMODULE module, size_t iatDisplacement, func_p_t newFunc);
func_p_t hookIAT(func_p_t* iat, func_p_t newVal);
//return the error code, if success, return ERROR_SUCCESS

func_p_t hookE8Call(HMODULE module, size_t e8argDisplacement, func_p_t newFunc);
func_p_t hookE8Call(pins_t e8ArgAddr, func_p_t newFunc);
//return the previous function

class HookException : public std::exception
{
};//todo

#define DECLARE_EMPTY_FUNC(emptyFunc, argSize)\
__declspec(naked) void emptyFunc()\
{\
	_asm\
	{\
		retn argSize\
	}\
}