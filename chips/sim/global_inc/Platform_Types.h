/*
 * Copyright (c) 2025 Li Auto Inc. and its affiliates
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H
#if !defined(ASSEMBLY)
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ghs__)
#include <stdint.h>
#endif

#define CPU_TYPE_8 8u
#define CPU_TYPE_16 16u
#define CPU_TYPE_32 32u
#define CPU_TYPE_64 64u

#define MSB_FIRST (0u) /* Big Endian bit ordering     */
#define LSB_FIRST (1u) /* Little Endian bit ordering  */
#define CPU_BIT_ORDER (LSB_FIRST)

#define HIGH_BYTE_FIRST (0u) /* Big Endian byte ordering    */
#define LOW_BYTE_FIRST (1u) /* Little Endian byte ordering */
#define CPU_BYTE_ORDER LOW_BYTE_FIRST /* Little Endian */

/**
  * @brief          Processor type
  */
#define CPU_TYPE (CPU_TYPE_32) /* 32 bit  */

#ifndef TRUE
#define TRUE (1u)
#endif
#ifndef FALSE
#define FALSE (0u)
#endif

#if (CPU_TYPE == CPU_TYPE_64)
typedef unsigned char boolean;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;
typedef signed long long sint64;

/* Unsigned integer at least 8 bit long. Range of at least 0 ..+255 (0x00..0xFF) - 8 bit */
typedef unsigned int uint8_least;
typedef unsigned int uint16_least;
typedef unsigned int uint32_least;

/**
 * Signed integer at least 8 bit long. Range - at least -128 ..+127.
 * At least 7 bit + 1 bit sign
 */
typedef signed int sint8_least;
typedef signed int sint16_least;
typedef signed int sint32_least;

typedef float float32;
typedef double float64;

#elif (CPU_TYPE == CPU_TYPE_32)
typedef unsigned char boolean;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
typedef signed char sint8;
typedef signed short sint16;
typedef signed long sint32;
typedef signed long long sint64;
typedef unsigned long uint8_least;
typedef unsigned long uint16_least;
typedef unsigned long uint32_least;
typedef signed long sint8_least;
typedef signed long sint16_least;
typedef signed long sint32_least;
typedef float float32;
typedef double float64;

#elif (CPU_TYPE == CPU_TYPE_16)
#error "This MCAL release has no CPU_TYPE_16 support."
#endif

typedef volatile uint32 addr_t;
typedef volatile uint32 vaddr_t;
typedef volatile uint32 paddr_t;

#ifdef __cplusplus
}
#endif
#endif
#endif /* #ifndef PLATFORM_TYPES_H */
