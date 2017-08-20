/*******************************************************************************
 * Nokia 1616 Display Driver
 * Controller used for this display is SPFD54124 with id --
 * Author: Ajeet Singh
 * Date: 08/13/2017
 * References:
 ********************************************************************************/
#ifndef NOKIA1616DRIVER_H
#define NOKIA1616DRIVER_H

#include <Arduino.h>
#include <AADDriversInterface.h>
#include <AADFontsLib.h>
#include <AADConfigLib.h>
#include <AADFontsLibDefault8x15.h>
#include <AADColorsLib.h>

//#define MAX_X         131
//#define MAX_Y         161
#define CMD   0
#define DAT   1

//#define CHAR_W 8
//#define CHAR_H 15

#define SWRESET      (0x01) // Software reset
#define SLPOUT      (0x11) //Sleep Out
#define NORON      (0x13) //Normal On
#define INVOFF      (0x20) //Invert Off
#define DISPON      (0x29) //Display On
#define CASET      (0x2A) //
#define RASET      (0x2B) //
#define RAMWR      (0x2C) //RAM Write
#define MADCTL      (0x36) //
#define IDMOFF      (0x38) //
#define COLMOD      (0x3A) //Column Mode

#define DATA 11
#define CLOCK 13
#define CS 10
#define RESET PC6

#define RST_SET digitalWrite(RESET, 1);
#define RST_RESET digitalWrite(RESET, 0);
#define CS_SET digitalWrite(CS, 1);
#define CS_RESET digitalWrite(CS, 0);
#define SDA_SET digitalWrite(DATA, 1);
#define SDA_RESET digitalWrite(DATA, 0);
#define CLK_SET digitalWrite(CLOCK, 1);
#define CLK_RESET digitalWrite(CLOCK, 0);

//#define F_CPU 16000000

class AADNokia1616LCDDriver : public AADLCDDriversInterface
{
  private:
    unsigned int MAX_X=161; //Max width
    unsigned int MAX_Y=131; //Max height
    byte CHAR_W = 8; //Default font width
    byte CHAR_H = 15; //Default font height
    unsigned int _bgcolor=COL_GREEN;
    AADFontsLibDefault8x15 *_default_font;

    void _lcd_init();
    void _set_pin_mode(void);
    void _begin();
    void _end();
    void _delay_ms_(uint32_t ms);
    void _lcd_reset();
    void _lcd_SendByte(char mode,unsigned char c);
    void _lcd_write16(unsigned int dat);
    void _lcd_window(byte x, byte y, byte w, byte h);
    void _lcd_fill_all(unsigned int color);

  public:
    AADNokia1616LCDDriver();
    virtual ~AADNokia1616LCDDriver();

    virtual unsigned int getMaxHeight(void);
    virtual unsigned int getMaxWidth(void);
    virtual void clear(void);
    virtual void set_bgcolor(int color);
    virtual void goto_xy(byte x, byte y);
    virtual void pixel(byte x, byte y, int color);
    virtual void print_char(byte x, byte y, unsigned char c, unsigned int color);
    virtual void print_string(byte x, byte y, char *str, unsigned int color);
    virtual void line(byte x0, byte y0, byte x1, byte y1, unsigned int color);
    virtual void circle(byte x0, byte y0, byte r, unsigned int color);
    virtual void v_line(byte x, byte y, byte h, unsigned int color);
    virtual void h_line(byte x, byte y, byte w, unsigned int color);
    virtual void rectangle(byte x, byte y, byte w, byte h, unsigned int color);
    virtual void bitmap(byte x, byte y, const byte *bitmap, byte w, byte h, unsigned int color);


    enum ORIENTATION {
      AT_0 = 0x08,
      AT_90 = 0x68,
      AT_180 = 0xC8,
      AT_270 = 0xA8
    } _orientation = AT_270;

    void rotate(ORIENTATION val);

};

#endif
