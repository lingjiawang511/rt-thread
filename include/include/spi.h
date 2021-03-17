#ifndef __SPI_H
#define __SPI_H

#include "HeadType.h"


#define Set_SPI_SCK()         gpio_set_value(jc_board_gpio_pin.msc_sclk,1)
#define Set_SPI_MOSI()        gpio_set_value(jc_board_gpio_pin.msc_mosi,1)
#define Reset_SPI_SCK()       gpio_set_value(jc_board_gpio_pin.msc_sclk,0)
#define Reset_SPI_MOSI()      gpio_set_value(jc_board_gpio_pin.msc_mosi,0)

/* 定义IO口 */
#define SD_CS_SET             gpio_set_value(jc_board_gpio_pin.msc_cs, 1)
#define SD_CS_CLR             gpio_set_value(jc_board_gpio_pin.msc_cs, 0)

#define Read_SPI_MISO         gpio_get_value(jc_board_gpio_pin.msc_miso)

/* 定义全局变量 */
void SPI2_Config(void);
void SPI2_SetSpeed(uint8_t Speed);
uint8_t SPI2_WriteReadData(uint8_t dat);

void SPI1_Config(void);
void SPI1_SetSpeed(uint8_t speed);
uint8_t SPI1_WriteReadData(uint8_t dat);

#endif
