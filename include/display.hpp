#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

// display interface
// max 65535 * 65535 resolution
// monochrome

struct DisplayPoint
{
  uint16_t x;
  uint16_t y;
};

class Display
{
public:
  virtual void init() = 0;
  virtual void drawPixel(DisplayPoint p) = 0;
  virtual void refresh(DisplayPoint min, DisplayPoint max) = 0;
};


#endif