/*! 
    \file     monitor.h
    \author   Original Code: J.C. Wren
              Modified By: K. Townsend (microBuilder.eu)
    \date     23 November 2009
    \version  1.00
*/

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

typedef enum {
  CMDTYPE_CMDLIST = 0,
  CMDTYPE_FUNCTION
} cmdType_e;

typedef struct commandList_s {
  const char* command;
  char minArgs;
  char maxArgs;
  cmdType_e cmdType;
  union {
    const void* trickGCC;
    int (*handler)(int argc, char** argv);
    struct commandList_s* commandList;
  };
  const char* description;
  const char* parameters;
} commandList_t;

void monitorTaskStart();
void monitorTaskStop();

#endif
