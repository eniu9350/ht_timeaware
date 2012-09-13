#ifndef _HTTA_COMMON_H_
#define _HTTA_COMMON_H_

#include <stdlib.h>


#define MALLOC(n, type) ((type*)malloc((n)*sizeof(type)))

typedef char* key;

//------session flag
#define SESSION_FLAG_INIT 0x0000
#define SESSION_FLAG_ADD 0x0001
#define SESSION_FLAG_DELETE 0x0010
#define SESSION_FLAG_FINISH 0x0011

#endif

