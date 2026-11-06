#include "Mcal_MemLibc.h"

Std_ReturnType Mcal_MemSet(void *dst, uint32 val, uint32 size)
{
	(void)dst;
	(void)val;
	(void)size;
	return E_OK;
}

Std_ReturnType Mcal_MemCpy(void *dst, const void *src, uint32 size)
{
	(void)dst;
	(void)src;
	(void)size;
	return E_OK;
}