

/* Includes ------------------------------------------------------------------*/
#include "tft.h"



static void writecmddata(uint16_t cmd, uint16_t dat);

void WriteCmdData(uint16_t cmd, uint16_t dat) { writecmddata(cmd, dat); }

static inline void WriteCmdParam4(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);

static void init_table(const void *table, int16_t size);

static void WriteCmdParamN(uint16_t cmd, int8_t N, uint8_t * block);

void pushCommand(uint16_t cmd, uint8_t * block, int8_t N) { WriteCmdParamN(cmd, N, block); }

static uint16_t read16bits(void);

#define write8(x)     { write_8(x); WRITE_DELAY; WR_STROBE; WR_IDLE; } //
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; READ_DELAY; dst = read_8(); RD_IDLE; RD_IDLE; } // read 250ns after RD_ACTIVE goes low
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define CTL_INIT()   { RD_OUTPUT; WR_OUTPUT; CD_OUTPUT; CS_OUTPUT; RESET_OUTPUT; }
#define WriteCmd(x)  { CD_COMMAND; write16(x); CD_DATA; }
#define WriteData(x) { write16(x); }
#define SUPPORT_9488_555          //costs +230 bytes, 0.03s / 0.19s
#define SUPPORT_B509_7793         //R61509, ST7793 +244 bytes
#define OFFSET_9327 32            //costs about 103 bytes, 0.08s



/************************************************************************************************************/

uint16_t _width    = WIDTH;
uint16_t _height   = HEIGHT;

uint16_t width(void)
{ return _width; }

uint16_t height(void)
{ return _height; }


void setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1);
int16_t readGRAM(int16_t x, int16_t y, uint16_t * block, int16_t w, int16_t h);


static uint8_t done_reset, is8347;


uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3); }
uint16_t readPixel(int16_t x, int16_t y) { uint16_t color; readGRAM(x, y, &color, 1, 1); return color; }


uint32_t cursor_y  =0, cursor_x    = 0;
uint8_t textsize  = 1;
uint16_t textcolor =0xffff,  textbgcolor = 0xFFFF;
uint8_t wrap      = true;
uint8_t _cp437    = false;
uint8_t rotation  = 1;

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }

uint16_t  _lcd_xor, _lcd_capable;

uint16_t _lcd_ID, _lcd_rev, _lcd_madctl, _lcd_drivOut, _MC, _MP, _MW, _SC, _EC, _SP, _EP;




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




static void writecmddata(uint16_t cmd, uint16_t dat)
{
    CS_ACTIVE;
    WriteCmd(cmd);
    WriteData(dat);
    CS_IDLE;
}

static void WriteCmdParamN(uint16_t cmd, int8_t N, uint8_t * block)
{
    CS_ACTIVE;
    WriteCmd(cmd);
    while (N-- > 0) {
        uint8_t u8 = *block++;
        write8(u8);
        if (N && is8347) {
            cmd++;
            WriteCmd(cmd);
        }
    }
    CS_IDLE;
}

static inline void WriteCmdParam4(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
{
    uint8_t d[4];
    d[0] = d1, d[1] = d2, d[2] = d3, d[3] = d4;
    WriteCmdParamN(cmd, 4, d);
}

#define TFTLCD_DELAY 0xFFFF
#define TFTLCD_DELAY8 0x7F
static void init_table(const void *table, int16_t size)
{

    uint8_t *p = (uint8_t *) table, dat[24];            //R61526 has GAMMA[22]

    while (size > 0)
    {
        uint8_t cmd = pgm_read_byte(p++);
        uint8_t len = pgm_read_byte(p++);
        if (cmd == TFTLCD_DELAY8)
        {
            delay_f(len);
            len = 0;
        }
        else
        {
            for (uint8_t i = 0; i < len; i++)
                dat[i] = pgm_read_byte(p++);
            WriteCmdParamN(cmd, len, dat);
        }
        size -= len + 2;
    }
}


void reset(void)
{
    done_reset = 1;
    setWriteDir();
    CTL_INIT();
    CS_IDLE;
    RD_IDLE;
    WR_IDLE;
    RESET_IDLE;
    delay_f(50);
    RESET_ACTIVE;
    delay_f(100);
    RESET_IDLE;
    delay_f(100);
	WriteCmdData(0xB0, 0x0000);   //R61520 needs this to read ID
}

static uint16_t read16bits(void)
{
    uint16_t ret;
    uint8_t lo;
    READ_8(ret);
    READ_8(lo);
    return (ret << 8) | lo;
}

uint16_t readReg(uint16_t reg, int8_t index)
{
    uint16_t ret;

    if (!done_reset)
        reset();
    CS_ACTIVE;
    WriteCmd(reg);
    setReadDir();
    delay_f(1);    //1us should be adequate
    //    READ_16(ret);
    do { ret = read16bits(); }while (--index >= 0);  //need to test with SSD1963
    RD_IDLE;
    CS_IDLE;
    setWriteDir();
    return ret;
}


uint32_t readReg40(uint16_t reg)
{
    uint16_t h = readReg(reg, 0);
    uint16_t m = readReg(reg, 1);
    uint16_t l = readReg(reg, 2);
    return ((uint32_t) h << 24) | (m << 8) | (l >> 8);
}

void tft_init(uint16_t ID)
{

    int16_t *p16;               //so we can "write" to a const protected variable.
    const uint8_t *table8_ads = NULL;
    int16_t table_size;
    _lcd_xor = 0;
    _lcd_ID = ID;

	_lcd_capable = AUTO_READINC | MIPI_DCS_REV1 | MV_AXIS;
	static const uint8_t RM68140_regValues_max[]  = {        //
		0x3A, 1, 0x55,      //Pixel format .kbv my Mega Shield
	};
	table8_ads = RM68140_regValues_max, table_size = sizeof(RM68140_regValues_max);
	p16 = (int16_t *) & height;
	*p16 = 480;
	p16 = (int16_t *) & width;
	*p16 = 320;

    _lcd_rev = ((_lcd_capable & REV_SCREEN) != 0);
    if (table8_ads != NULL) {
        static const uint8_t reset_off[]  = {
            0x01, 0,            //Soft Reset
            TFTLCD_DELAY8, 150,  // .kbv will power up with ONLY reset, sleep out, display on
            0x28, 0,            //Display Off
            0x3A, 1, 0x55,      //Pixel read=565, write=565.
        };
        static const uint8_t wake_on[]  = {
			0x11, 0,            //Sleep Out
            TFTLCD_DELAY8, 150,
            0x29, 0,            //Display On
        };
		init_table(&reset_off, sizeof(reset_off));
	    init_table(table8_ads, table_size);   //can change PIXFMT
		init_table(&wake_on, sizeof(wake_on));
    }
    setRotation(0);             //PORTRAIT
    invertDisplay(false);

}

void invertDisplay(uint8_t i)
{
    uint8_t val;
    _lcd_rev = ((_lcd_capable & REV_SCREEN) != 0) ^ i;

	if (is8347) {
		// HX8347D: 0x36 Panel Characteristic. REV_Panel
		// HX8347A: 0x36 is Display Control 10
		if (_lcd_ID == 0x8347 || _lcd_ID == 0x5252) // HX8347-A, HX5352-A
			val = _lcd_rev ? 6 : 2;       //INVON id bit#2,  NORON=bit#1
		else val = _lcd_rev ? 8 : 10;     //HX8347-D, G, I: SCROLLON=bit3, INVON=bit1
		// HX8347: 0x01 Display Mode has diff bit mapping for A, D
		WriteCmdParamN(0x01, 1, &val);
	} else
		WriteCmdParamN(_lcd_rev ? 0x21 : 0x20, 0, NULL);


}

uint16_t readID(void)
{
    uint16_t ret;

	ret = readReg40(0xBF);

    if (ret == 0x1400)          //?RM68140:[xx FF 68 14 00] not tested yet
        return 0x6814;
    return 0x6814;
}


void setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1)
{

   WriteCmdParam4(_SC, x >> 8, x, x1 >> 8, x1);   //Start column instead of _MC
   WriteCmdParam4(_SP, y >> 8, y, y1 >> 8, y1);   //
   uint8_t d[2];
   d[0] = x >> 8; d[1] = x;
   WriteCmdParamN(_MC, 2, d);                 //allows !MV_AXIS to work
   d[0] = y >> 8; d[1] = y;
   WriteCmdParamN(_MP, 2, d);

}

void setRotation(uint8_t r)
{
   uint16_t GS, SS_v;
   uint8_t val, d[3];
   rotation = r & 3;           // just perform the operation ourselves on the protected variables
   _width = (rotation & 1) ? HEIGHT : WIDTH;
   _height = (rotation & 1) ? WIDTH : HEIGHT;
   switch (rotation) {
   case 0:                    //PORTRAIT:
       val = 0x48;             //MY=0, MX=1, MV=0, ML=0, BGR=1
       break;
   case 1:                    //LANDSCAPE: 90 degrees
       val = 0x28;             //MY=0, MX=0, MV=1, ML=0, BGR=1
       break;
   case 2:                    //PORTRAIT_REV: 180 degrees
       val = 0x98;             //MY=1, MX=0, MV=0, ML=1, BGR=1
       break;
   case 3:                    //LANDSCAPE_REV: 270 degrees
       val = 0xF8;             //MY=1, MX=1, MV=1, ML=1, BGR=1
       break;
   }

   if (_lcd_capable & MIPI_DCS_REV1) {
       if (_lcd_ID == 0x6814) {  //.kbv my weird 0x9486 might be 68140
           GS = (val & 0x80) ? (1 << 6) : 0;   //MY
           SS_v = (val & 0x40) ? (1 << 5) : 0;   //MX
           val &= 0x28;        //keep MV, BGR, MY=0, MX=0, ML=0
           d[0] = 0;
           d[1] = GS | SS_v | 0x02;      //MY, MX
           d[2] = 0x3B;
           WriteCmdParamN(0xB6, 3, d);

       }

       _MC = 0x2A, _MP = 0x2B, _MW = 0x2C, _SC = 0x2A, _EC = 0x2A, _SP = 0x2B, _EP = 0x2B;

       WriteCmdParamN(is8347 ? 0x16 : 0x36, 1, &val);
       _lcd_madctl = val;

   }

   if ((rotation & 1) && ((_lcd_capable & MV_AXIS) == 0)) {
       uint16_t x;
       x = _MC, _MC = _MP, _MP = x;
       x = _SC, _SC = _SP, _SP = x;    //.kbv check 0139
       x = _EC, _EC = _EP, _EP = x;    //.kbv check 0139
   }
   setAddrWindow(0, 0, width() - 1, height() - 1);

}



void drawRGBBitmap(uint16_t x, uint16_t y, uint16_t *bitmap, uint16_t w, uint16_t h){


	  	//int16_t end; // todo lo que esta comentado son protecciones de codigo. Las saco ya que parece que TouchGFX no le pifia y esto da mas velocidad

	    //if (w < 0) {
	    //    w = -w;
	    //    x -= w;
	    //}                           //+ve w
	    //end = x + w;
	    //if (x < 0)
	    //    x = 0;
	    //if (end > width())
	    //    end = width();
	    //w = end - x;
	    //if (h < 0) {
	    //    h = -h;
	    //    y -= h;
	    //}                           //+ve h
	    //end = y + h;
	    //if (y < 0)
	    //    y = 0;
	    //if (end > height())
	    //    end = height();
	    //h = end - y;
	    setAddrWindow(x, y, x + w - 1, y + h - 1); // ojo que aca me esta verificando que el bit map no sea mayor a los rango permisibles tener cuidado con el *bitmap
	    CS_ACTIVE;
	    WriteCmd(_MW);

	    //if (h > w) {
	    //    end = h;
	    //    h = w;
	    //    w = end;
	    //}
	    uint8_t hi , lo;
	    uint32_t cant;
	    const uint32_t total_data = h * w;
	    for(cant = 0;cant < total_data ;cant++) {
				hi = bitmap[cant] >> 8;
				lo = bitmap[cant] & 0xFF;
				write8(hi);
				write8(lo);
		}
	    CS_IDLE;

	    //if (!(_lcd_capable & MIPI_DCS_REV1) || ((_lcd_ID == 0x1526) && (rotation & 1)))
	    //    setAddrWindow(0, 0, width() - 1, height() - 1);
}


void delay_f (uint32_t time)
{

	HAL_Delay(time);
}
