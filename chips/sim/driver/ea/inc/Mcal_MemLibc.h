#ifndef MCAL_MEMLIBC_H
#define MCAL_MEMLIBC_H

#include "Std_Types.h"

Std_ReturnType Mcal_MemSet(void *dst, uint32 val, uint32 size);
Std_ReturnType Mcal_MemCpy(void *dst, const void *src, uint32 size);

#endif