#ifndef MRAM_IP_H
#define MRAM_IP_H

#include <stdint.h>
#include <stdbool.h>

#define MRAM_ECC_ERROR_ADDRESS_MAX (8u)
#define MRAM_RDBUFFER_IGNOR_IDENT (0xFFFFu)

typedef unsigned int Mram_BitFieldType;

typedef enum {
	MRAM_INIT_RST_AUTO_WAKEUP = 0u,
	MRAM_INIT_SW_AUTO_WAKEUP,
	MRAM_INIT_BYPASS,
	MRAM_INIT_UNTRIMMED_SW_WAKEUP,
} Mram_InitialModeType;

typedef enum { MRAM_UNINIT = 0, MRAM_READY, MRAM_DEFECTIVE } Mram_StatusType;

typedef enum {
	MRAM_IMG_CRC_CHK_INVALID = 0,
	MRAM_IMG_CRC_CHK_STOP,
	MRAM_IMG_CRC_CHK_ONESHOT,
	MRAM_IMG_CRC_CHK_LOOP,
} Mram_CrcCheckModeType;

typedef void Mram_EventListenerType(void *arg, uint32_t event);

typedef struct {
	uint32_t apbBase;
	uint32_t axiRdBase;
	uint32_t axiWrBase;
	uint32_t availableSize;
	uint32_t offset;
	uint32_t irq;
	Mram_InitialModeType initialMode;
	bool interleaveDis;
} Mram_HostStaticConfigType;

typedef struct {
	Mram_HostStaticConfigType config;
	uint32_t aclkHz;
	uint32_t tmcClkHz;
	uint8_t wrIntervalUs;
	bool wrBufferEnable;
} Mram_HostConfigType;

typedef struct {
	uint8_t channelId;
	uint16_t masterId;
	uint16_t arid;
	bool enable;
} Mram_RdBufferCfgType;

typedef struct {
	uint8_t phaseRevision;
	uint8_t minorRevision;
	uint8_t majorRevision;
	uint8_t mramVersion;
	uint32_t totalSize;
} Mram_InfoType;

typedef union {
	uint32_t data;
	struct {
		Mram_BitFieldType wordAddress : 20;
		Mram_BitFieldType reserved : 12;
	} address;
	struct {
		Mram_BitFieldType yAddress : 5;
		Mram_BitFieldType xAddress3To0 : 4;
		Mram_BitFieldType xAddress9To4 : 6;
		Mram_BitFieldType bAddress : 3;
		Mram_BitFieldType reserved : 14;
	} original32MAddress;
	struct {
		Mram_BitFieldType yAddress : 5;
		Mram_BitFieldType xAddressBit0 : 1;
		Mram_BitFieldType xAddress9To1 : 9;
		Mram_BitFieldType reserved : 17;
	} original4MAddress;
	struct {
		Mram_BitFieldType yAddress : 5;
		Mram_BitFieldType xAddress9To4 : 6;
		Mram_BitFieldType bAddress : 3;
		Mram_BitFieldType xAddress3To0 : 4;
		Mram_BitFieldType reserved : 14;
	} interleaved32MAddress;
	struct {
		Mram_BitFieldType yAddress : 5;
		Mram_BitFieldType xAddress9To1 : 9;
		Mram_BitFieldType xAddressBit0 : 1;
		Mram_BitFieldType reserved : 17;
	} interleaved4MAddress;
} Mram_WordAddressType;

typedef struct {
	bool invalidFlag;
	Mram_WordAddressType mramWordAddress;
} Mram_EccErrorAddressType;

typedef struct Mram_Host {
	Mram_HostStaticConfigType config;
	bool wrBufferEnable;
	bool useInterrupt;
	uint8_t index;
	uint32_t base;
	uint64_t pglkPassword;
	uint8_t wrIntervalUs;
	uint32_t aclkHz;
	uint32_t tmcClkHz;
	uint32_t dmaMaxSize;
	Mram_InfoType info;
	Mram_StatusType status;
	Mram_EventListenerType *eventListener;
	void *eventListenerArg;
	Mram_EccErrorAddressType eccErrAddrTable[MRAM_ECC_ERROR_ADDRESS_MAX];
	uint8_t eccErrAddrNum;
	void *parent;
} Mram_HostType;

#endif