#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

// display interface
// 255*255 max resolution (limited by the parameter sizes)
// monochrome

struct DisplayPoint
{
  uint8_t x;
  uint8_t y;
};

class Display
{
public:
  virtual void moveCursor(uint8_t x, uint8_t y) = 0;
  virtual void moveCursorBuf(uint8_t x, uint8_t y) = 0;
  virtual void drawPixel(uint8_t x, uint8_t y) = 0;
  virtual void drawPixelBuffer(uint8_t x, uint8_t y) = 0;

  // flushes the buffer onto the screen
  virtual void refresh() = 0;
  virtual void refresh(DisplayPoint min, DisplayPoint max) = 0;
};


#endif