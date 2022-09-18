#include "event.hpp"

Event::Event()
{
  _clearAllCallbacks();
}

Event::~Event() {
  _clearCallbacks();
}

uint8_t Event::addListener(uint32_t callback)
{
  if (_endIndex < MAX_CALLBACKS_PER_EVENT)
  {
    uint8_t callbackInd = _endIndex;
    _callbacks[_endIndex++] = callback;
    return callbackInd;
  }
  // error
  return MAX_CALLBACKS_PER_EVENT;
}

void Event::removeListeners()
{
  _clearCallbacks();
}

void Event::emit()
{
  for (uint8_t i = 0; i < _endIndex; ++i)
  {
    // convert the function address to a callable void function and call it
    ((CALLBACK_TYPE)_callbacks[i])();
  }
}

void Event::_clearAllCallbacks()
{
  for (uint8_t i = 0; i < MAX_CALLBACKS_PER_EVENT; ++i)
  {
    _callbacks[i] = 0x00000000;
  }
  _endIndex = 0;
}

void Event::_clearCallbacks()
{
  for (uint8_t i = 0; i < _endIndex; ++i)
  {
    _callbacks[i] = 0x00000000;
  }
  _endIndex = 0;
}
