#ifndef NOKIA_DISPLAY_H_
#define NOKIA_DISPLAY_H_

#include "display.hpp"
#include "gpio.hpp"

// implements functionality for a nokia 5110 lcd
#define WIDTH 84
#define HEIGHT 48
#define SCREEN_SIZE WIDTH*HEIGHT

struct NokiaDisplayConfig 
{
  // reset
  GpioPin rst;
  // chip enable
  GpioPin ce;
  // data/command
  GpioPin dc;
  // data
  GpioPin din;
  // clock
  GpioPin clk;
};

class NokiaDisplay : Display
{
public:

  NokiaDisplay(NokiaDisplayConfig config);

  void moveCursor(DisplayPoint p) = 0;
  void moveCursorBuffer(DisplayPoint p) = 0;
  void drawPixel(DisplayPoint p) = 0;
  void drawPixelBuffer(DisplayPoint p) = 0;

  // flushes the buffer onto the screen
  void refresh() = 0;
  void refresh(DisplayPoint min, DisplayPoint max) = 0;

private:

  // buffer
  uint8_t _buffer[SCREEN_SIZE];
};


#endif