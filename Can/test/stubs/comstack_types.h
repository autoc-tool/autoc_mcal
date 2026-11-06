#ifndef COMSTACK_TYPES_H
#define COMSTACK_TYPES_H

#include "Std_Types.h"

typedef uint16 PduIdType;
typedef uint16 PduLengthType;

typedef struct {
	uint8 *SduDataPtr;
	uint8 *MetaDataPtr;
	PduLengthType SduLength;
} PduInfoType;

#endif /* COMSTACK_TYPES_H */
