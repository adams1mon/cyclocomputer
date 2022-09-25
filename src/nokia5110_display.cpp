#ifndef NOKIA5110_DISPLAY_H_
#define NOKIA5110_DISPLAY_H_

#include "nokia5110_display.hpp"

#define MIN(X, Y) ((X < Y) ? (X) : (Y))
#define MAX(X, Y) ((X > Y) ? (X) : (Y))

// init settings
#define NOKIA_EXTENDED_COMMAND_MODE 0x21
#define NOKIA_CONTRAST 0xAE
#define NOKIA_TEMP_COEFFICIENT 0x04
#define NOKIA_BIAS_MODE 0x14
#define NOKIA_BASIC_COMMAND_MODE 0x20
#define NOKIA_NORMAL_DISPLAY_MODE 0x0C

NokiaDisplay::NokiaDisplay(NokiaDisplayConfig* config)
{
  this->_config = config;
}

void NokiaDisplay::init()
{
  _config->rst.clearPin();
  _config->rst.writePin();

  _sendCommand(NOKIA_EXTENDED_COMMAND_MODE);
  _sendCommand(NOKIA_CONTRAST);
  _sendCommand(NOKIA_TEMP_COEFFICIENT);
  _sendCommand(NOKIA_BIAS_MODE);
  _sendCommand(NOKIA_BASIC_COMMAND_MODE);
  _sendCommand(NOKIA_NORMAL_DISPLAY_MODE);

  refreshEntireScreen();
}

void NokiaDisplay::refreshEntireScreen()
{
  DisplayPoint min = {0, 0};
  DisplayPoint max = {NOKIA_WIDTH-1, NOKIA_HEIGHT-1};
  refresh(min, max);
}

void NokiaDisplay::_moveCursor(DisplayPoint p)
{
  _sendCommand(0x80 | p.x);
  _sendCommand(0x40 | p.y);
}

// side-effect: cursor position won't be the same as before calling the function
void NokiaDisplay::refresh(DisplayPoint min, DisplayPoint max)
{
  uint16_t x_min = MIN(min.x, max.x);
  uint16_t x_max = MAX(min.x, max.x);
  uint16_t y_min = MIN(min.y, max.y);
  uint16_t y_max = MAX(min.y, max.y);

  uint16_t x = x_min;
  uint16_t y = y_min;
  for (; y <= y_max; ++y)
  {
    for (; x <= x_max; ++x)
    {
      DisplayPoint drawPoint = {x, y};
      _moveCursor(drawPoint);
      drawPixel(drawPoint);
    }
  }
}

void NokiaDisplay::drawPixel(DisplayPoint p)
{
  _moveCursor(p);
  _sendData(0xff);
}

void NokiaDisplay::_send(uint8_t data)
{
  // send each bit of the 'data', from front to back
  for (uint8_t i = 7; i >= 0; --i)
  {
    data & (1 << i) ? _config->din.writePin() : _config->din.clearPin();

    // send the bit set on the 'din' line
    _config->clk.writePin();
    _config->clk.clearPin();
  }
}

void NokiaDisplay::_sendCommand(uint8_t command)
{
  _config->dc.clearPin();
  _config->ce.clearPin();
  _send(command);
  _config->ce.writePin();
}

void NokiaDisplay::_sendData(uint8_t data)
{
  _config->dc.writePin();
  _config->ce.clearPin();
  _send(data);
  _config->ce.writePin();
}


#endif