#pragma once

#define pca9635_MaxDevices 126

#ifdef __cplusplus
  extern "C"
{
#endif

  enum pca9635LEDColors {
    RED, BLUE, GREEN, YELLOW, WHITE, _COLORS
  };


  int  pca9635Setup(int address);
  int  pca9635SetBrightness(int handle, int led, int color, int percent);
  void pca9635DigitalWrite(int handle, int pin, int value);

#ifdef __cplusplus
}
#endif
