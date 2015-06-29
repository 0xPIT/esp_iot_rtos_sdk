/* 
  ardio.c - wiring digital implementation for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "esp_common.h"
#include "driver/ardio.h"

void ICACHE_FLASH_ATTR pinMode(uint8_t pin, uint8_t mode) {
  if (pin < 16){
    if (mode == SPECIAL){
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPEC = (1 << pin); //Disable
      GPF(pin) = GPFFS(GPFFS_BUS(pin));//Set mode to BUS (RX0, TX0, TX1, SPI, HSPI or CLK depending in the pin)
      if (pin == 3) GPF(pin) |= (1 << GPFPU);//enable pullup on RX
    } 
    else if (mode & FUNCTION_0){
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPEC = (1 << pin); //Disable
      GPF(pin) = GPFFS((mode >> 4) & 0x07);
      if (pin == 13 && mode == FUNCTION_4) GPF(pin) |= (1 << GPFPU);//enable pullup on RX
    }  else if (mode == OUTPUT || mode == OUTPUT_OPEN_DRAIN){
      GPF(pin) = GPFFS(GPFFS_GPIO(pin));//Set mode to GPIO
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      if (mode == OUTPUT_OPEN_DRAIN) GPC(pin) |= (1 << GPCD);
      GPES = (1 << pin); //Enable
    } 
    else if (mode == INPUT || mode == INPUT_PULLUP || mode == INPUT_PULLDOWN){
      GPF(pin) = GPFFS(GPFFS_GPIO(pin));//Set mode to GPIO
      GPEC = (1 << pin); //Disable
      GPC(pin) = (GPC(pin) & (0xF << GPCI)) | (1 << GPCD); //SOURCE(GPIO) | DRIVER(OPEN_DRAIN) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      if (mode == INPUT_PULLUP) {
          GPF(pin) |= (1 << GPFPU);  // Enable  Pullup
      } 
      else if (mode == INPUT_PULLDOWN) {
          GPF(pin) |= (1 << GPFPD);  // Enable  Pulldown
      }
    }
  } 
  else if (pin == 16){
    GPF16 = GP16FFS(GPFFS_GPIO(pin));//Set mode to GPIO
    GPC16 = 0;
    if (mode == INPUT || mode == INPUT_PULLDOWN){
      if (mode == INPUT_PULLDOWN){
        GPF16 |= (1 << GP16FPD);//Enable Pulldown
      }
      GP16E &= ~1;
    } 
    else if (mode == OUTPUT){
      GP16E |= 1;
    }
  }
}

void ICACHE_FLASH_ATTR digitalWrite(uint8_t pin, uint8_t val) {
  val &= 0x01;
  if (pin < 16){
    if (val) GPOS = (1 << pin);
    else GPOC = (1 << pin);
  } 
  else if (pin == 16){
    if (val) GP16O |= 1;
    else GP16O &= ~1;
  }
}

int ICACHE_FLASH_ATTR digitalRead(uint8_t pin) {
  if (pin < 16){
    return GPIP(pin);
  } 
  else if (pin == 16){
    return GP16I & 0x01;
  }
  return 0;
}
