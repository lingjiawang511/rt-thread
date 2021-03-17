#ifndef __24CXX_H
#define __24CXX_H
#include "stm32f4xx_hal.h"
#include "myType.h"

#define I2C_SCL_H   gpio_set_value(jc_board_gpio_pin.i2c_scl,1)
#define I2C_SCL_L   gpio_set_value(jc_board_gpio_pin.i2c_scl,0)

#define I2C_SDA_H   gpio_set_value(jc_board_gpio_pin.i2c_sda,1)
#define I2C_SDA_L   gpio_set_value(jc_board_gpio_pin.i2c_sda,0)

#define AT24C01  127
#define AT24C02  255
#define AT24C04  511
#define AT24C08  1023
#define AT24C16  2047
#define AT24C32  4095
#define AT24C64  8191
#define AT24C128 16383
#define AT24C256 32767

#define EE_TYPE  AT24C64

void delay_nms(uint32_t nms);
/* 声明全局函数 */
void I2C_INIT(void);
void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
u8   I2C_Wait_Ack(void);
void I2C_Send_Byte(u8 txd);
u8   I2C_Read_Byte(u8 ack);




/* 声明全局函数 */
void AT24CXX_Init(void);
u8 AT24CXX_ReadOneByte(u16 addr);
void AT24CXX_WriteOneByte(u16 addr, u8 dt);
u8 AT24CXX_Check(void);
void AT24CXX_WriteLenByte(u16 WriteAddr, u32 DataToWrite, u8 Len);
u32 AT24CXX_ReadLenByte(u16 ReadAddr, u8 Len);
void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead);
void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite);
#endif
















