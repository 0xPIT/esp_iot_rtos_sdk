/*! 
    \file     monitor.c
    \author   Original Code: J.C. Wren
              Modified By: K. Townsend (microBuilder.eu)
    \date     23 November 2009
    \version  1.00
*/

#include "esp_libc.h"
#include "monitor.h"
#include "args.h"
#include "argsdispatch.h"

#define arrsizeof(x) ((sizeof(x)) / (sizeof(x[0])))

static int monitorHelp(int argc, char** argv);
static int monitorVersion(int argc, char** argv);
static int monitorReset(int argc, char** argv);
static int monitorADCRead(int argc, char** argv);

extern int vfdReset(int argc, char** argv);
extern int vfdTest(int argc, char** argv);

static const commandList_t commandListVFD [] = {
  { "help",     0,  0, CMDTYPE_FUNCTION,  { monitorHelp        }, "This help list",               "" },
  { "reset",    0,  0, CMDTYPE_FUNCTION,  { vfdReset           }, "Reset the VFD",                "" },
  { "test",     0,  0, CMDTYPE_FUNCTION,  { vfdTest            }, "Init and test VFD",            "" },
  { NULL,       0,  0, CMDTYPE_FUNCTION,  { NULL               }, NULL,                           NULL },
};

static const commandList_t commandListAnalog [] = {
  { "help",     0,  0, CMDTYPE_FUNCTION,  { monitorHelp        }, "This help list",               "" },
  { "read",     0,  0, CMDTYPE_FUNCTION,  { monitorADCRead     }, "Read current ADC value",       "'read' has no parameters" },
  { NULL,       0,  0, CMDTYPE_FUNCTION,  { NULL               }, NULL,                           NULL },
};

static const commandList_t commandList [] = {
  { "help",     0,  0, CMDTYPE_FUNCTION,  { monitorHelp        }, "This help list",                 "" },
  { "version",  0,  0, CMDTYPE_FUNCTION,  { monitorVersion     }, "Display version information",    "'version' has no parameters" },
  { "reset",    0,  0, CMDTYPE_FUNCTION,  { monitorReset       }, "Resets the ESP8266",             "'reset' has no parameters" },
  { "adc",      1,  0, CMDTYPE_CMDLIST,   { commandListAnalog  }, "Analog commands",                "'adc help' for help list" },
  { "vfd",      1,  0, CMDTYPE_CMDLIST,   { commandListVFD     }, "VFD Test commands",              "'vfd help' for help list" },
  { NULL,       0,  0, CMDTYPE_FUNCTION,  { NULL               }, NULL,                             NULL },
};

commandList_t *activeCommandList = NULL;

// Determines whether the supplied string is a decimal value
//
static int ICACHE_FLASH_ATTR isDecimalString (char *s) {
  while (*s)
    if (!isdigit (*s++))
      return 0;

  return 1;
}

// Converts a string representation of a number to an integer
//
static int ICACHE_FLASH_ATTR getNumber (char *s, unsigned int *result) {
  unsigned int value;
  unsigned int mustBeHex = FALSE;
  int sgn = 1;
  const unsigned char hexToDec [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255, 255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15};

  if (!s)
    return 0;

  if ((strlen (s) > 2) && (!strncmp (s, "0x", 2) || !strncmp (s, "0X", 2)))
  {
    mustBeHex = TRUE;
    s += 2;
  }

  if (!mustBeHex && *s && (*s == '-'))
  {
    sgn = -1;
    s++;
  }

  for (value = 0; *s; s++)
  {
    if (mustBeHex && isxdigit (*s))
      value = (value << 4) | hexToDec [toupper (*s) - '0'];
    else if (isdigit (*s))
      value = (value * 10) + (*s - '0');
    else
    {
      printf ("Malformed number.  Must be decimal number, or hex value preceeded by '0x'\r\n");
      return 0;
    }
  }

  if (!mustBeHex)
    value *= sgn;

  *result = value;

  return 1;
}

// Dumps the memory contents of the supplied address
//
static int ICACHE_FLASH_ATTR monitorDumpMemory(unsigned int displayAddress, unsigned int mask, unsigned int address, int length)
{
  unsigned char *buffer;
  int i;

  if (!length)
  {
    printf ("Error: monitorDumpMemory() passed 0 for length\r\n");
    return address;
  }

  for (buffer = (unsigned char *) address, i = 0; i < length; i += 16)
  {
    unsigned int l;
    unsigned int j;

    if (i)
      printf ("\r\n");

    printf ("%08x: ", (displayAddress + i) & mask);

    if ((length - i) < 16)
      l = length & 15;
    else
      l = 16;

    for (j = 0; j < 16; j++)
    {
      if (j < l)
        printf ("%02x ", buffer [i+j]);
      else
        printf ("   ");
    }

    printf ("  ");

    for (j = 0; j < l; j++)
    {
      unsigned char c = buffer [i+j];

      if (c < 32 || c > 127)
        c = '.';

      printf ("%c", c);
    }
  }

  printf ("\r\n");

  address += length;

  return address;
}

// Restart the CPU
//
int ICACHE_FLASH_ATTR monitorReset(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
  printf ("Resetting the ESP8266...\r\n");
  system_restart();
  return 0;
}

int ICACHE_FLASH_ATTR monitorADCRead(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
  uint16_t value = system_adc_read();
  printf("Analog Value: %d\n", value);
  return 0;
}


// Monitor help menu
//
static int ICACHE_FLASH_ATTR monitorHelp(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
  unsigned int i;
  int t;
  int longestCmd;
  char spaces[32];

  memset(spaces, ' ', sizeof (spaces));

  for (longestCmd = 0, i = 0; activeCommandList[i].command; i++) {
    if ((t = strlen(activeCommandList[i].command)) > longestCmd) {
      longestCmd = t;
    }
  }

  spaces[longestCmd] = '\0';

  for (i = 0; activeCommandList[i].command; i++) {
    const commandList_t* cl = &activeCommandList[i];
    printf("%s%s -- %s\r\n", cl->command, &spaces[strlen(cl->command)], cl->description);
  }

  printf("\r\nUse '<command> ?' for details on parameters to command\r\n");

  return 0;
}


// Display the monitor version and copyright information
//
static int ICACHE_FLASH_ATTR monitorVersion(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
  printf ("ESPShell with RTOS SDK %s.%d,\r\n", system_get_sdk_version());
  printf ("(c) karl@pitrich.com\r\n\r\n");

  printf("Sys Time: %d\r\n", system_get_time());
  printf("RTC Time: %d\r\n", system_get_rtc_time());
  printf("VDD: %d\r\n", system_get_vdd33());

  return 0;
}


extern void GetTcpRxQueue(xQueueHandle *qh);

void vMonitorTask(void* pvParameters)
{
  static xQueueHandle rxQueue = NULL;
  static uint8_t buffer[256];
  static char* argv[34];
  int argc;

  GetTcpRxQueue(&rxQueue);
  //UART0_GetRxQueue(&rxQueue);
  
  monitorVersion(0, NULL);

  for (;;) {
    int l;

    if ((l = argsGetLine(rxQueue, buffer, sizeof(buffer), "> "))) {
      if (argsParse((char *)buffer, argv, sizeof(argv), &argc)) {
        printf("Too many arguments (max %ld)\r\n", arrsizeof(argv));
      }
      else if (argv [0]) {
        argsDispatch(commandList, argc, &argv [0]);
      }
    }
  }
}


static xTaskHandle xMonitorHandle = NULL;

void monitorTaskStop() {
  vTaskDelete(xMonitorHandle);
}

void monitorTaskStart() {
  xTaskCreate(vMonitorTask,
                      (const char* const)"mon",
                      1024,
                      NULL,
                      3,
                      &xMonitorHandle
                    );
  // return xTaskCreate(vMonitorTask,
  //                     (const signed char* const) "Monitor",
  //                     1024,
  //                     NULL,
  //                     (tskIDLE_PRIORITY + 3),
  //                     &taskHandles[TASKHANDLE_MONITOR]
  //                   );
}
