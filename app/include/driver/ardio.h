#ifndef __AIO_H__
#define __AIO_H__

#define HIGH 0x1
#define LOW  0x0

#define PWMRANGE 1023

//GPIO FUNCTIONS
#define INPUT             0x00
#define INPUT_PULLUP      0x02
#define INPUT_PULLDOWN    0x04
#define OUTPUT            0x01
#define OUTPUT_OPEN_DRAIN 0x03
#define SPECIAL           0xF8 //defaults to the usable BUSes uart0rx/tx uart1tx and hspi
#define FUNCTION_0        0x08
#define FUNCTION_1        0x18
#define FUNCTION_2        0x28
#define FUNCTION_3        0x38
#define FUNCTION_4        0x48

#define ESP8266_REG(addr)  *((volatile uint32_t *)(0x60000000+(addr)))
#define ESP8266_DREG(addr) *((volatile uint32_t *)(0x3FF00000+(addr)))

//GPIO (0-15) Control Registers
#define GPO    ESP8266_REG(0x300) //GPIO_OUT R/W (Output Level)
#define GPOS   ESP8266_REG(0x304) //GPIO_OUT_SET WO
#define GPOC   ESP8266_REG(0x308) //GPIO_OUT_CLR WO
#define GPE    ESP8266_REG(0x30C) //GPIO_ENABLE R/W (Enable)
#define GPES   ESP8266_REG(0x310) //GPIO_ENABLE_SET WO
#define GPEC   ESP8266_REG(0x314) //GPIO_ENABLE_CLR WO
#define GPI    ESP8266_REG(0x318) //GPIO_IN RO (Read Input Level)
#define GPIE   ESP8266_REG(0x31C) //GPIO_STATUS R/W (Interrupt Enable)
#define GPIES  ESP8266_REG(0x320) //GPIO_STATUS_SET WO
#define GPIEC  ESP8266_REG(0x324) //GPIO_STATUS_CLR WO

#define GPOP(p) ((GPO & (1 << ((p) & 0xF))) != 0)
#define GPEP(p) ((GPE & (1 << ((p) & 0xF))) != 0)
#define GPIP(p) ((GPI & (1 << ((p) & 0xF))) != 0)
#define GPIEP(p) ((GPIE & (1 << ((p) & 0xF))) != 0)

//GPIO (0-15) PIN Control Registers
#define GPC(p) ESP8266_REG(0x328 + ((p & 0xF) * 4))
#define GPC0   ESP8266_REG(0x328) //GPIO_PIN0
#define GPC1   ESP8266_REG(0x32C) //GPIO_PIN1
#define GPC2   ESP8266_REG(0x330) //GPIO_PIN2
#define GPC3   ESP8266_REG(0x334) //GPIO_PIN3
#define GPC4   ESP8266_REG(0x338) //GPIO_PIN4
#define GPC5   ESP8266_REG(0x33C) //GPIO_PIN5
#define GPC6   ESP8266_REG(0x340) //GPIO_PIN6
#define GPC7   ESP8266_REG(0x344) //GPIO_PIN7
#define GPC8   ESP8266_REG(0x348) //GPIO_PIN8
#define GPC9   ESP8266_REG(0x34C) //GPIO_PIN9
#define GPC10  ESP8266_REG(0x350) //GPIO_PIN10
#define GPC11  ESP8266_REG(0x354) //GPIO_PIN11
#define GPC12  ESP8266_REG(0x358) //GPIO_PIN12
#define GPC13  ESP8266_REG(0x35C) //GPIO_PIN13
#define GPC14  ESP8266_REG(0x360) //GPIO_PIN14
#define GPC15  ESP8266_REG(0x364) //GPIO_PIN15

//GPIO (0-15) PIN Control Bits
#define GPCWE  10 //WAKEUP_ENABLE (can be 1 only when INT_TYPE is high or low)
#define GPCI   7  //INT_TYPE (3bits) 0:disable,1:rising,2:falling,3:change,4:low,5:high
#define GPCD   2  //DRIVER 0:normal,1:open drain
#define GPCS   0  //SOURCE 0:GPIO_DATA,1:SigmaDelta

static uint8_t esp8266_gpioToFn[16] = {0x34, 0x18, 0x38, 0x14, 0x3C, 0x40, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x04, 0x08, 0x0C, 0x10};
#define GPF(p) ESP8266_REG(0x800 + esp8266_gpioToFn[(p & 0xF)])

#define GPMUX  ESP8266_REG(0x800)
//GPIO (0-15) PIN Function Registers
#define GPF0   ESP8266_REG(0x834)
#define GPF1   ESP8266_REG(0x818)
#define GPF2   ESP8266_REG(0x838)
#define GPF3   ESP8266_REG(0x814)
#define GPF4   ESP8266_REG(0x83C)
#define GPF5   ESP8266_REG(0x840)
#define GPF6   ESP8266_REG(0x81C)
#define GPF7   ESP8266_REG(0x820)
#define GPF8   ESP8266_REG(0x824)
#define GPF9   ESP8266_REG(0x828)
#define GPF10  ESP8266_REG(0x82C)
#define GPF11  ESP8266_REG(0x830)
#define GPF12  ESP8266_REG(0x804)
#define GPF13  ESP8266_REG(0x808)
#define GPF14  ESP8266_REG(0x80C)
#define GPF15  ESP8266_REG(0x810)

//GPIO (0-15) PIN Function Bits
#define GPFSOE 0 //Sleep OE
#define GPFSS  1 //Sleep Sel
#define GPFSPD 2 //Sleep Pulldown
#define GPFSPU 3 //Sleep Pullup
#define GPFFS0 4 //Function Select bit 0
#define GPFFS1 5 //Function Select bit 1
#define GPFPD  6 //Pulldown
#define GPFPU  7 //Pullup
#define GPFFS2 8 //Function Select bit 2
#define GPFFS(f) (((((f) & 4) != 0) << GPFFS2) | ((((f) & 2) != 0) << GPFFS1) | ((((f) & 1) != 0) << GPFFS0))
#define GPFFS_GPIO(p) (((p)==0||(p)==2||(p)==4||(p)==5)?0:((p)==16)?1:3)
#define GPFFS_BUS(p) (((p)==1||(p)==3)?0:((p)==2||(p)==12||(p)==13||(p)==14||(p)==15)?2:((p)==0)?4:1)

//GPIO 16 Control Registers
#define GP16O  ESP8266_REG(0x768)
#define GP16E  ESP8266_REG(0x774)
#define GP16I  ESP8266_REG(0x78C)

//GPIO 16 PIN Control Register
#define GP16C  ESP8266_REG(0x790)
#define GPC16  GP16C

//GPIO 16 PIN Function Register
#define GP16F  ESP8266_REG(0x7A0)
#define GPF16  GP16F

//GPIO 16 PIN Function Bits
#define GP16FFS0 0 //Function Select bit 0
#define GP16FFS1 1 //Function Select bit 1
#define GP16FPD  3 //Pulldown
#define GP16FSPD 5 //Sleep Pulldown
#define GP16FFS2 6 //Function Select bit 2
#define GP16FFS(f) (((f) & 0x03) | (((f) & 0x04) << 4))

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

#endif