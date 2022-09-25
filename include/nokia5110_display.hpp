#ifndef NOKIA_DISPLAY_H_
#define NOKIA_DISPLAY_H_

#include "display.hpp"
#include "gpio.hpp"

// implements functionality for a nokia 5110 lcd
#define NOKIA_WIDTH 84
#define NOKIA_HEIGHT 48
#define NOKIA_SCREEN_SIZE WIDTH*HEIGHT

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
  NokiaDisplay(NokiaDisplayConfig* config);

  void init();
  void drawPixel(DisplayPoint p);
  void refresh(DisplayPoint min, DisplayPoint max);

  void refreshEntireScreen();

private:
  NokiaDisplayConfig* _config;

  void _send(uint8_t data);
  void _sendCommand(uint8_t command);
  void _sendData(uint8_t data);

  void _moveCursor(DisplayPoint p);
};


#endif