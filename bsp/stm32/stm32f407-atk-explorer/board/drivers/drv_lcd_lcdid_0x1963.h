typedef struct {
    rt_uint8_t type;
    rt_uint16_t val;
} lcd_config_0x1963;

static const lcd_config_0x1963 lcd_config_data_0x1963[] = {
    {LCD_WR_REG_TYPE, 0xE2},      //Set PLL with OSC = 10MHz (hardware),  Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
    {LCD_WR_DAT_TYPE, 0x1D},     //²ÎÊý1
    {LCD_WR_DAT_TYPE, 0x02},     //²ÎÊý2 Divider M = 2, PLL = 300/(M+1) = 100MHz
    {LCD_WR_DAT_TYPE, 0x04},     //²ÎÊý3 Validate M and N values
    {LCD_WR_DLY_TYPE, 100},
    {LCD_WR_REG_TYPE, 0xE0},      // Start PLL command
    {LCD_WR_DAT_TYPE, 0x01},     // enable PLL
    {LCD_WR_DLY_TYPE, 1},
    {LCD_WR_REG_TYPE, 0xE0},      // Start PLL command again
    {LCD_WR_DAT_TYPE, 0x03},     // now, use PLL output as system clock
    {LCD_WR_DLY_TYPE, 1},
    {LCD_WR_REG_TYPE, 0x01},      //Èí¸´Î»
    {LCD_WR_DLY_TYPE, 1},
    {LCD_WR_REG_TYPE, 0xE6},      //ÉèÖÃÏñËØÆµÂÊ,33Mhz
    {LCD_WR_DAT_TYPE, 0x2F},
    {LCD_WR_DAT_TYPE, 0xFF},
    {LCD_WR_DAT_TYPE, 0xFF},
    {LCD_WR_REG_TYPE, 0xB0},      //ÉèÖÃLCDÄ£Ê½
    {LCD_WR_DAT_TYPE, 0x20},     //24Î»Ä£Ê½
    {LCD_WR_DAT_TYPE, 0x00},     //TFT Ä£Ê½
    {LCD_WR_DAT_TYPE, (SSD_HOR_RESOLUTION - 1) >> 8}, //ÉèÖÃLCDË®Æ½ÏñËØ
    {LCD_WR_DAT_TYPE, SSD_HOR_RESOLUTION - 1},
    {LCD_WR_DAT_TYPE, (SSD_VER_RESOLUTION - 1) >> 8}, //ÉèÖÃLCD´¹Ö±ÏñËØ
    {LCD_WR_DAT_TYPE, SSD_VER_RESOLUTION - 1},
    {LCD_WR_DAT_TYPE, 0x00},     //RGBÐòÁÐ
    {LCD_WR_REG_TYPE, 0xB4},      //Set horizontal period
    {LCD_WR_DAT_TYPE, (SSD_HT - 1) >> 8},
    {LCD_WR_DAT_TYPE, SSD_HT - 1},
    {LCD_WR_DAT_TYPE, SSD_HPS >> 8},
    {LCD_WR_DAT_TYPE, SSD_HPS},
    {LCD_WR_DAT_TYPE, SSD_HOR_PULSE_WIDTH - 1},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_REG_TYPE, 0xB6},      //Set vertical period
    {LCD_WR_DAT_TYPE, (SSD_VT - 1) >> 8},
    {LCD_WR_DAT_TYPE, SSD_VT - 1},
    {LCD_WR_DAT_TYPE, SSD_VPS >> 8},
    {LCD_WR_DAT_TYPE, SSD_VPS},
    {LCD_WR_DAT_TYPE, SSD_VER_FRONT_PORCH - 1},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_DAT_TYPE, 0x00},
    {LCD_WR_REG_TYPE, 0xF0},  //ÉèÖÃSSD1963ÓëCPU½Ó¿ÚÎª16bit
    {LCD_WR_DAT_TYPE, 0x03}, //16-bit(565 format) data for 16bpp
    {LCD_WR_REG_TYPE, 0x29},  //¿ªÆôÏÔÊ¾
//ÉèÖÃPWMÊä³ö  ±³¹âÍ¨¹ýÕ¼¿Õ±È¿Éµ÷
    {LCD_WR_REG_TYPE, 0xD0},  //ÉèÖÃ×Ô¶¯°×Æ½ºâDBC
    {LCD_WR_DAT_TYPE, 0x00}, //disable
    {LCD_WR_REG_TYPE, 0xBE},  //ÅäÖÃPWMÊä³ö
    {LCD_WR_DAT_TYPE, 0x05}, //1ÉèÖÃPWMÆµÂÊ
    {LCD_WR_DAT_TYPE, 0xFE}, //2ÉèÖÃPWMÕ¼¿Õ±È
    {LCD_WR_DAT_TYPE, 0x01}, //3ÉèÖÃC
    {LCD_WR_DAT_TYPE, 0x00}, //4ÉèÖÃD
    {LCD_WR_DAT_TYPE, 0x00}, //5ÉèÖÃE
    {LCD_WR_DAT_TYPE, 0x00}, //6ÉèÖÃF
    {LCD_WR_REG_TYPE, 0xB8},  //ÉèÖÃGPIOÅäÖÃ
    {LCD_WR_DAT_TYPE, 0x03}, //2¸öIO¿ÚÉèÖÃ³ÉÊä³ö
    {LCD_WR_DAT_TYPE, 0x01}, //GPIOÊ¹ÓÃÕý³£µÄIO¹¦ÄÜ
    {LCD_WR_REG_TYPE, 0xBA},
    {LCD_WR_DAT_TYPE, 0X01} //GPIO[1:0]=01,¿ØÖÆLCD·½Ïò
};
