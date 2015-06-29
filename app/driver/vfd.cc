#include "driver/vfd.h"


ICACHE_FLASH_ATTR VFD::VFD(uint8_t datamode,
  uint8_t bitorder,
  uint32_t speed,
  uint8_t pinCmdData,
  uint8_t pinReset,
  uint8_t width,
  uint8_t height) 
  : CD_PIN(pinCmdData), RESET_PIN(pinReset), w(width), h(height)
{
  //printf("vfd ctor %d %d\n", CD_PIN, RESET_PIN);
  pinMode(CD_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);

  initBitbang();
}

void ICACHE_FLASH_ATTR VFD::initBitbang() {
  pinMode(SCK, OUTPUT);
  pinMode(DO, OUTPUT);
  pinMode(SS, OUTPUT);
}

// all delays in bitbang() can be omitted
void ICACHE_FLASH_ATTR VFD::bitbang(uint8_t data, bool command) const {
  digitalWrite(CD_PIN, command);
  digitalWrite(SS, LOW);
  for (uint8_t i = 0x80; i > 0; i >>= 1) {
    digitalWrite(SCK, LOW); //os_delay_us(1);
    digitalWrite(DO, (data & i) ? HIGH : LOW);
    digitalWrite(SCK, HIGH); //os_delay_us(1);
  }
  //os_delay_us(1);
  digitalWrite(SS, HIGH);
}

void ICACHE_FLASH_ATTR VFD::cmd(uint8_t data) const {
  bitbang(data, true);
}

void ICACHE_FLASH_ATTR VFD::data(uint8_t data) const {
  bitbang(data, false);
}

void ICACHE_FLASH_ATTR VFD::initialize() const {
  digitalWrite(RESET_PIN, LOW);
  os_delay_us(50000);
  digitalWrite(RESET_PIN, HIGH);
  os_delay_us(50000);

  cmd(ClearAll);
  os_delay_us(60000);

  for (uint8_t n = 0; n < 8; n++) { 
    cmd(DisplayAreaSet);
    cmd(n);
    data(0xff);
  }

//    brightness(0);   
//    set_start_play(0, 2);  

  //display_on();   
  cmd(0x24);
  cmd(0x40);

//    set_position_mode(2);  // IncXFixY
  cmd(IncXFixY);

//    cmd(0x5f); //display_clear();
//    delay(3);

//    set_position(0,0);

}

void ICACHE_FLASH_ATTR VFD::set_start_play(uint8_t x, uint8_t y) const {
  cmd(0x70); // hshift 
  cmd(x);    // 0x00
  cmd(16*11+y*2);  // 0xB4 = 1011 0100 
}

void ICACHE_FLASH_ATTR VFD::set_position(uint8_t x, uint8_t y) const {   
  cmd(0x64);
  cmd(x);
  cmd(0x60);
  cmd(y);
}

void ICACHE_FLASH_ATTR VFD::set_position_mode(uint8_t mode) const {
  cmd(16 * 8 + mode * 2);      
}

void ICACHE_FLASH_ATTR VFD::brightness(uint8_t level) const {       
  cmd(16 * 4 + level);   
}


// void ICACHE_FLASH_ATTR setShift(uint8_t x, uint8_t y) const {
//   cmd(HShift);
//   delayMicroseconds(3);
//   cmd(x & 0x7f);

//   //cmd2(WritePosY, y & 0x0f); // 32
//   cmd(VShiftDn8);
//   delayMicroseconds(3);
// }

// void init_vfd(void)  {   
//    U8 n;   
//    //write_data(0x1b);   
//    //write_data(0x40);   
//    write_cmd(0x5f);   
//    del(100); 
//    for(n=0;n<8;n++)   
//    {   
//        write_cmd(0x62);   
//        write_cmd(n);   
//        write_data(0xff);   
//    }

//    brightness_control(0);   
//    set_start_play(0,2);   
//    display_on();   
//    set_position_mode(2);   
//    display_clear();   
//    del(50);   
//    set_position(0,0);   
// }    

void ICACHE_FLASH_ATTR VFD::setCursor(uint8_t x, uint8_t y) const {
  //cmd2(WritePosX, x & 0x7f); // 128
  cmd(WritePosX);
  cmd(x & 0x7f);

  //cmd2(WritePosY, y & 0x0f); // 32
  cmd(WritePosY);
  cmd(y & 0x0f);
}


void ICACHE_FLASH_ATTR VFD::test() const {
  setCursor(0, 0);

  for (uint8_t y = 0; y < 4; y++) {
      cmd(WritePosY);
      cmd(y & 0x0f);
      for(uint8_t x = 0; x < 128; x += 4) {
          cmd(WritePosX);
          cmd(x & 0x7f);
          data(0x55);
      }
  }
}
