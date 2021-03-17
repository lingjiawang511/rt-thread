typedef struct {
    rt_uint8_t type;
    rt_uint16_t val;
} lcd_config_0x9341;

static const lcd_config_0x9341 lcd_config_data_0x9341[] = {
    {LCD_WR_REG_TYPE, 0xCF},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0xC1},
    {LCD_WR_DAT_TYPE, 0X30},
    {LCD_WR_REG_TYPE, 0xED},
    {LCD_WR_DAT_TYPE, 0x64},
    {LCD_WR_DAT_TYPE, 0x03},
    {LCD_WR_DAT_TYPE, 0X12},
    {LCD_WR_DAT_TYPE, 0X81},
    {LCD_WR_REG_TYPE, 0xE8},
    {LCD_WR_DAT_TYPE, 0x85},
    {LCD_WR_DAT_TYPE, 0x10},
    {LCD_WR_DAT_TYPE, 0x7A},
    {LCD_WR_REG_TYPE, 0xCB},
    {LCD_WR_DAT_TYPE, 0x39},
    {LCD_WR_DAT_TYPE, 0x2C},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x34},
    {LCD_WR_DAT_TYPE, 0x02},
    {LCD_WR_REG_TYPE, 0xF7},
    {LCD_WR_DAT_TYPE, 0x20},
    {LCD_WR_REG_TYPE, 0xEA},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_REG_TYPE, 0xC0},   //Power control
    {LCD_WR_DAT_TYPE, 0x1B},  //VRH[5:0]
    {LCD_WR_REG_TYPE, 0xC1},   //Power control
    {LCD_WR_DAT_TYPE, 0x01},  //SAP[2:0];BT[3:0]
    {LCD_WR_REG_TYPE, 0xC5},   //VCM control
    {LCD_WR_DAT_TYPE, 0x30},  //3F
    {LCD_WR_DAT_TYPE, 0x30},  //3C
    {LCD_WR_REG_TYPE, 0xC7},   //VCM control2
    {LCD_WR_DAT_TYPE, 0XB7},
    {LCD_WR_REG_TYPE, 0x36},   // Memory Access Control
    {LCD_WR_DAT_TYPE, 0x48},
    {LCD_WR_REG_TYPE, 0x3A},
    {LCD_WR_DAT_TYPE, 0x55},
    {LCD_WR_REG_TYPE, 0xB1},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x1A},
    {LCD_WR_REG_TYPE, 0xB6},   // Display Function Control
    {LCD_WR_DAT_TYPE, 0x0A},
    {LCD_WR_DAT_TYPE, 0xA2},
    {LCD_WR_REG_TYPE, 0xF2},   // 3Gamma Function Disable
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_REG_TYPE, 0x26},   //Gamma curve selected
    {LCD_WR_DAT_TYPE, 0x01},
    {LCD_WR_REG_TYPE, 0xE0},   //Set Gamma
    {LCD_WR_DAT_TYPE, 0x0F},
    {LCD_WR_DAT_TYPE, 0x2A},
    {LCD_WR_DAT_TYPE, 0x28},
    {LCD_WR_DAT_TYPE, 0x08},
    {LCD_WR_DAT_TYPE, 0x0E},
    {LCD_WR_DAT_TYPE, 0x08},
    {LCD_WR_DAT_TYPE, 0x54},
    {LCD_WR_DAT_TYPE, 0XA9},
    {LCD_WR_DAT_TYPE, 0x43},
    {LCD_WR_DAT_TYPE, 0x0A},
    {LCD_WR_DAT_TYPE, 0x0F},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_REG_TYPE, 0XE1},   //Set Gamma
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x15},
    {LCD_WR_DAT_TYPE, 0x17},
    {LCD_WR_DAT_TYPE, 0x07},
    {LCD_WR_DAT_TYPE, 0x11},
    {LCD_WR_DAT_TYPE, 0x06},
    {LCD_WR_DAT_TYPE, 0x2B},
    {LCD_WR_DAT_TYPE, 0x56},
    {LCD_WR_DAT_TYPE, 0x3C},
    {LCD_WR_DAT_TYPE, 0x05},
    {LCD_WR_DAT_TYPE, 0x10},
    {LCD_WR_DAT_TYPE, 0x0F},
    {LCD_WR_DAT_TYPE, 0x3F},
    {LCD_WR_DAT_TYPE, 0x3F},
    {LCD_WR_DAT_TYPE, 0x0F},
    {LCD_WR_REG_TYPE, 0x2B},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x01},
    {LCD_WR_DAT_TYPE, 0x3f},
    {LCD_WR_REG_TYPE, 0x2A},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0xef},
    {LCD_WR_REG_TYPE, 0x11}, //Exit Sleep
    {LCD_WR_DLY_TYPE, 12},
    {LCD_WR_REG_TYPE, 0x29} //display on
};
