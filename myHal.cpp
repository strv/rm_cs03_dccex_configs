#if !defined(IO_NO_HAL)

#include "IODevice.h"
#include "myIO_SerialLED.h"

#define SLED_PIN (27)

void halSetup() {
  SerialLED::create(SLED_PIN);
}

#endif
