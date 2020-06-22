#pragma once

#define pca9635_MaxDevices 126

#ifdef __cplusplus
  extern "C"
{
#endif

  typedef unsigned int  PCA9635_ADDRESS;
  typedef unsigned int  PCA9635_LED;
  typedef unsigned int  PCA9635_TYPE;     // (ADDRESS << 8) | ( (0x0f & COLOR) << 4 ) | (0x0F & LED)

  enum PCA9635_COLOR  {
//  0    1     2      3       4 
    RED, BLUE, GREEN, YELLOW, WHITE, _COLORS
  };


  int  pca9635Setup(int address);
  int  pca9635SetBrightness(int handle, int led, int color, int percent);
  void pca9635DigitalWrite(int handle, int pin, int value);
  int  pca9635GetLED();

  PCA9635_TYPE pca9635_getTypeFromENV(const char* var);
  PCA9635_TYPE pca9635_getType(PCA9635_ADDRESS address, enum PCA9635_COLOR color, PCA9635_LED pin);

  PCA9635_ADDRESS pca9635_getAddress(PCA9635_TYPE pin);
  enum PCA9635_COLOR   pca9635_getColor(PCA9635_TYPE pin);
  PCA9635_LED     pca9635_getLED(PCA9635_TYPE pin);

#ifdef __cplusplus
}
#endif
