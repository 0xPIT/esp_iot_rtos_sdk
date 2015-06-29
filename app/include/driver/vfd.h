extern "C" {
  #include "esp_common.h"
  #include "driver/ardio.h"
}


#define SCK 14
#define DO  13
#define SS  15

class VFD {
public:
  const uint8_t CD_PIN;
  const uint8_t RESET_PIN;
  const uint8_t w;
  const uint8_t h;

  typedef enum Command_e {
    ClearAll = 0x5f,

    LayersOff = 0x2000, // 0010 0000 0000 0000
    LayersAnd = 0x2048, // 0010 0000 0100 1000
    LayersXor = 0x2044, // 0010 0000 0100 0100
    LayersOr  = 0x2c40, // 0010 1100 0100 0000
    Layer1    = 0x2840, // 0010 1000 0100 0000
    Layer0    = 0x2440, // 0010 0100 0100 0000
    
    Brightest  = 0x40,
    Bright100  = 0x40,
    Bright75   = 0x44,
    Bright50   = 0x48,
    Bright25   = 0x4c,
    Darkest    = 0x4f,

    DisplayAreaSet = 0x62,
    // [0x00-0x07]
    DisplayGRAM = 0xff,

    WritePosX = 0x64, // 0 + 7bit
    WritePosY = 0x60, // 0000 + 4 bit

    HShift = 0x70, // + 8bit (0x00 - 0x7f) display start address

    VShiftUp1 = 0xbc,
    VShiftUp2 = 0xbe,
    VShiftUp8 = 0xba,
    VShiftDn1 = 0xb4,
    VShiftDn2 = 0xb6,
    VShiftDn8 = 0xb2,

    FixXY = 0x80,
    IncXY = 0x86,
    FixXIncY = 0x82,
    IncYFixX = 0x82,
    FixYIncX = 0x84,
    IncXFixY = 0x84,

    AddrRead = 0xd4

  } Command_t;

public:
  VFD(uint8_t datamode = 0 /* = SPI_MODE0*/,
    uint8_t bitorder = 0 /*= MSBFIRST*/,
    uint32_t speed = 1e6L,
    uint8_t pinCmdData = 5,
    uint8_t pinReset = 4,
    uint8_t width = 128,
    uint8_t height = 32);

private:
  void initBitbang();
  void bitbang(uint8_t data, bool command) const;
  void cmd(uint8_t data) const;
  void data(uint8_t data) const;

public:
  void initialize() const;
  void set_start_play(uint8_t x, uint8_t y) const;
  void set_position(uint8_t x, uint8_t y) const;
  void set_position_mode(uint8_t mode) const;
  void brightness(uint8_t level) const;
  void setCursor(uint8_t x, uint8_t y) const;
  void test() const;
};
