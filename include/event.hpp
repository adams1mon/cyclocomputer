#ifndef EVENT_H_
#define EVENT_H_

#include <stdint.h>

#define MAX_CALLBACKS_PER_EVENT 16

// the function type to which the callback addresses are converted
#define CALLBACK_TYPE void(*)(void)

class Event
{

public: 
  Event();
  ~Event();
  uint8_t addListener(uint32_t callback);
  void removeListeners();
  void emit();

private: 
  uint32_t _callbacks[MAX_CALLBACKS_PER_EVENT];
  uint8_t _endIndex;

  void _clearAllCallbacks();
  void _clearCallbacks();
};

#endif /* EVENT_H_ */