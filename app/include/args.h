/*! 
    \file     args.h
    \author   Original Code: J.C. Wren
              Modified By: K. Townsend (microBuilder.eu)
    \date     23 November 2009
    \version  1.00
*/

#ifndef _ARGS_H_
#define _ARGS_H_

#include "c_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

int argsGetLine(xQueueHandle *qh, uint8_t *buffer, int bufferLength, const char *prompt);
int argsParse(char *cmd, char **argv, int sizeofArgv, int *argc);

#endif
