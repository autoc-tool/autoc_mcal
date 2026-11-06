#ifndef STD_TYPES_H
#define STD_TYPES_H

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef uint8 boolean;
typedef uint8 Std_ReturnType;

#define TRUE ((boolean)1u)
#define FALSE ((boolean)0u)
#define E_OK ((Std_ReturnType)0u)
#define E_NOT_OK ((Std_ReturnType)1u)
#define STD_ON 1u
#define STD_OFF 0u

#endif /* STD_TYPES_H */
