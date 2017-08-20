/*******************************************************************************
   Nokia 1616 Display Driver
   Controller used for this display is SPFD54124 with id --
   Author: Ajeet Singh
   Date: 08/13/2017
   References:
 ********************************************************************************/

#include "AADNokia1616LCDDriver.h"

unsigned int AADNokia1616LCDDriver::getMaxHeight(void){
	return this->MAX_Y;
}

unsigned int AADNokia1616LCDDriver::getMaxWidth(void){
	return this->MAX_X;
}

void AADNokia1616LCDDriver::_set_pin_mode(void) {
  pinMode(DATA, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(CLOCK, OUTPUT);
}

void AADNokia1616LCDDriver::_begin() {
  CS_SET;
}

void AADNokia1616LCDDriver::_end() {
  CS_RESET;
}

void AADNokia1616LCDDriver::_delay_ms_(uint32_t ms) {
  uint32_t nCount;
  nCount = (F_CPU / 10000) * ms;
  for (; nCount != 0; nCount--)
  {

    __asm ("nop");
    __asm ("nop");
    __asm ("nop");
    __asm ("nop");
  }
}

void AADNokia1616LCDDriver::_lcd_write16(unsigned int dat){
  _lcd_SendByte(DAT, dat>>8);
  _lcd_SendByte(DAT, dat);
}

void AADNokia1616LCDDriver::_lcd_SendByte(char mode, unsigned char c) {
  CS_RESET;
  CLK_RESET;

  if (mode)
    SDA_SET
  else
    SDA_RESET;

  CLK_SET;

  for (unsigned char i = 0; i < 8; i++)
  {
    CLK_RESET;

    if (c & 0x80)
      SDA_SET
    else
      SDA_RESET;

    CLK_SET;
    c <<= 1;
  }
  CS_SET;
}

void AADNokia1616LCDDriver::_lcd_reset(){
  RST_SET;
  CS_SET;
  CLK_SET;
  _delay_ms_(1);
  RST_RESET;
  CS_RESET;
  _delay_ms_(20);
  RST_SET;
  _delay_ms_(20);
}

void AADNokia1616LCDDriver::_lcd_init(){
  _lcd_reset();
  _lcd_SendByte(CMD,SWRESET);
  _delay_ms_(120);

  _lcd_SendByte(CMD,SLPOUT);
  _delay_ms_(20);

  _lcd_SendByte(CMD,INVOFF);
  _lcd_SendByte(CMD,IDMOFF);
  _lcd_SendByte(CMD,NORON);

  _lcd_SendByte(CMD,COLMOD);
  _lcd_SendByte(DAT,0x05);         //COLMOD pixel format: 0x04=12bit; 0x05=16bit; 0x06=18bit
  _delay_ms_(10);

  _lcd_SendByte(CMD,MADCTL);
  _lcd_SendByte(DAT,_orientation);
  _delay_ms_(10);

  _lcd_SendByte(CMD,DISPON);
  _delay_ms_(10);

  clear();
}

void AADNokia1616LCDDriver::rotate(ORIENTATION val){

  _orientation = val;
  if(val == AT_270 || val == AT_90){
    MAX_X=161;
    MAX_Y=131;
  }

  if(val == AT_180 || val == AT_0){
    MAX_X=131;
    MAX_Y=161;
  }

  _lcd_SendByte(CMD, MADCTL);
  _lcd_SendByte(DAT, val);
}

void AADNokia1616LCDDriver::_lcd_window(byte x, byte y, byte w, byte h){
  _lcd_SendByte(CMD, CASET);
  _lcd_write16(x+2);
  _lcd_write16(x+w+1);
  _lcd_SendByte(CMD, RASET);
  _lcd_write16(y+1);
  _lcd_write16(y+h+1);
  _lcd_SendByte(CMD, RAMWR);
}

void AADNokia1616LCDDriver::_lcd_fill_all(unsigned int color){
  _lcd_window(0, 0, MAX_X, MAX_Y);
  byte i=MAX_Y;
  byte j;
  while(i--){
    j=MAX_X;
    while(j--){
      _lcd_write16((color));
    }
  }
}

AADNokia1616LCDDriver::AADNokia1616LCDDriver(){
  _begin();
  _set_pin_mode();
  _lcd_init();

  //init default font
  _default_font = CONF_DEFAULT_FONT;
  this->CHAR_W = _default_font->getFontWidth();
  this->CHAR_H = _default_font->getFontHeight();
}

AADNokia1616LCDDriver::~AADNokia1616LCDDriver(){
  _end();
}

void AADNokia1616LCDDriver::clear(void){
  _lcd_fill_all(_bgcolor);
}

void AADNokia1616LCDDriver::set_bgcolor(int color){
  _bgcolor = color;
  clear();
}

void AADNokia1616LCDDriver::goto_xy(byte x, byte y){
  _lcd_SendByte(CMD, CASET);
  _lcd_write16(x+2);
  _lcd_SendByte(CMD, RASET);
  _lcd_write16(y+1);
}

void AADNokia1616LCDDriver::pixel(byte x, byte y, int color){
  goto_xy(x,y);
  _lcd_SendByte(CMD, RAMWR);// WR_MEM
  _lcd_write16(color);
}

void AADNokia1616LCDDriver::print_char(byte x, byte y, unsigned char c, unsigned int color){
  byte ch;
  byte mask = 0x80;

  _lcd_window(x, y, _default_font->getFontWidth(), _default_font->getFontHeight());

  for (byte h=0; h<_default_font->getFontHeight(); h++){ // every column of the character
    //ch=pgm_read_byte(&(FONT_MS_SANS_SERIF_8x12[((c-32)*CHAR_H)+h]));
    //ch=pgm_read_byte(&(FONT8x15[c-32][h]));
	  ch=_default_font->getRowByteForCharacter(c, h);
    mask = 0x80;
    for (byte p=0; p<_default_font->getFontWidth(); p++){  // write the pixels
      (ch & mask)? _lcd_write16(color) : _lcd_write16(_bgcolor);
      mask >>= 1;
    }

  }
}

void AADNokia1616LCDDriver::print_string(byte x, byte y, char *str, unsigned int color){
  unsigned char j;
  j=0;
  while (j<strlen(str)){
    print_char((x+j*CHAR_W),y,str[j], color);
    j++;
  }
}

void AADNokia1616LCDDriver::line(byte x0, byte y0, byte x1, byte y1, unsigned int color){
  int dy = y1 - y0;
  int dx = x1 - x0;
  int stepx, stepy;
  if (dy < 0){
    dy = -dy;
    stepy = -1;
  }
  else stepy = 1;
  if (dx < 0){
    dx = -dx;
    stepx = -1;
  }
  else stepx = 1;

  dy <<= 1;                              // dy is now 2*dy
  dx <<= 1;                              // dx is now 2*dx

  pixel(x0, y0, color);

  if (dx > dy) {
    int fraction = dy - (dx >> 1);     // same as 2*dy - dx
    while (x0 != x1){
      if (fraction >= 0) {
        y0 += stepy;
        fraction -= dx;            // same as fraction -= 2*dx
      }
      x0 += stepx;
      fraction += dy;                // same as fraction -= 2*dy
      pixel(x0, y0, color);
    }
  }
  else{
    int fraction = dx - (dy >> 1);
    while (y0 != y1){
      if (fraction >= 0){
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      pixel(x0, y0, color);
    }
  }
}

void AADNokia1616LCDDriver::circle(byte x0, byte y0, byte r, unsigned int color){
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  pixel(x0  , y0+r, color);
  pixel(x0  , y0-r, color);
  pixel(x0+r, y0  , color);
  pixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    pixel(x0 + x, y0 + y, color);
    pixel(x0 - x, y0 + y, color);
    pixel(x0 + x, y0 - y, color);
    pixel(x0 - x, y0 - y, color);
    pixel(x0 + y, y0 + x, color);
    pixel(x0 - y, y0 + x, color);
    pixel(x0 + y, y0 - x, color);
    pixel(x0 - y, y0 - x, color);
  }
}

void AADNokia1616LCDDriver::v_line(byte x, byte y, byte h, unsigned int color){
  line(x, y, x, y+h-1, color);
}

void AADNokia1616LCDDriver::h_line(byte x, byte y, byte w, unsigned int color){
  line(x, y, x+w-1, y, color);
}

void AADNokia1616LCDDriver::rectangle(byte x, byte y, byte w, byte h, unsigned int color){
  h_line(x, y, w, color);
  h_line(x, y+h-1, w, color);
  v_line(x, y, h, color);
  v_line(x+w-1, y, h, color);
}

void AADNokia1616LCDDriver::bitmap(byte x, byte y, const byte *bitmap, byte w, byte h, unsigned int color){
  byte i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        pixel(x+i, y+j, color);
      }
    }
  }
}
