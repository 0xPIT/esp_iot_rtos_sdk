#ifndef __C_PLUS_PLUS_H__
#define __C_PLUS_PLUS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_common.h"

#ifdef __cplusplus
}
#endif

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

void cppInitialize();

#endif
