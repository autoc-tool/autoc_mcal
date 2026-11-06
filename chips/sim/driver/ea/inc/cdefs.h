#ifndef CDEFS_H
#define CDEFS_H

#ifndef ROUNDUP
#define ROUNDUP(a, b) ((uint32)(((uint32)(a) + (uint32)((uint32)(b)-1)) & ((uint32)~((uint32)(b)-1))))
#endif

#define _MIN(a, b) (((a) < (b)) ? (a) : (b))
#define _MAX(a, b) (((a) > (b)) ? (a) : (b))

#endif