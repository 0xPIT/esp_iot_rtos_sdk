/*! 
    \file     args.c
    \author   Original Code: J.C. Wren
              Modified By: K. Townsend (microBuilder.eu)
    \date     23 November 2009
    \version  1.00
*/

#include "esp_libc.h"
#include "ctype.h"
#include "args.h"

static char ICACHE_FLASH_ATTR *strtrim(char *s)
{
  char *t = s + strlen (s) - 1;

  while (t >= s && *t && isspace (*t))
    *t-- = '\0';

  while (*s && isspace (*s))
    s++;

  return s;
}


int ICACHE_FLASH_ATTR argsGetLine(xQueueHandle *qh, uint8_t *buffer, int bufferLength, const char *prompt)
{
  uint8_t *p;

  p = buffer;
  *p = '\0';

  printf ("%s", prompt);

  while (1)
  {
    uint8_t c;
    
    if (qh)
    {
      if (xQueueReceive(qh, &c, (portTickType)portMAX_DELAY))
      {
        switch (c)
        {
          case '\n' :
          case '\r' :
            printf ("\r\n");
            return strlen ((char *) buffer);
  
          case '\b' :
            if (p > buffer)
              *--p = '\0';
            printf ("\b \b");
            break;
  
          case 0x15 : // CTRL-U
            while (p != buffer)
            {
              printf ("\b \b");
              --p;
            }
            *p = '\0';
            break;
  
          case 0xfe :
          case 0xff :
            *buffer++ = c;
            *buffer = '\0';
            return 1;
  
          default : 
            if (p < buffer + bufferLength - 1 && c >= ' ' && c < 0x7f)
            { 
              *p++ = c;
              *p = '\0';
              printf ("%c", c); 
            }
            else
              printf ("%c", c); 
  
            break;
        }
      }
    }
  }

  return 0;
}

typedef enum {
  P_EATWHITESPACE = 0,
  P_GETCHARFIRST,
  P_GETCHAR,
  P_QUOTEDGETCHAR
} PSTATE;

int ICACHE_FLASH_ATTR argsParse(char *cmd, char **argv, int sizeofArgv, int *argc)
{
  int maxArgs = (sizeofArgv / sizeof (argv [0])) - 1;
  char *s = strtrim (cmd);
  PSTATE pstate = P_EATWHITESPACE;

  *argc = 0;
  memset (argv, 0, sizeofArgv);

  while (*s) {
    switch (pstate) {
      case P_EATWHITESPACE :
        {
          if (!isspace (*s))
            pstate = P_GETCHARFIRST;
          else
            s++;
        }
        break;

      case P_GETCHARFIRST :
        {
          *argv++ = s;

          if (++*argc == maxArgs)
            return 1;
          if (*s == '"')
            pstate = P_QUOTEDGETCHAR;
          else
            pstate = P_GETCHAR;

          s++;
        }
        break;

      case P_GETCHAR :
        {
          if (isspace (*s)) {
            pstate = P_EATWHITESPACE;
            *s = '\0';
          } 
          else if (*s == '"')
            pstate = P_QUOTEDGETCHAR;

          s++;
        }
        break;

      case P_QUOTEDGETCHAR :
        {
          if (*s == '"')
            pstate = P_GETCHAR;

          s++;
        }
        break;
    }
  }

  return 0;
}
