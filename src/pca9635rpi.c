#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "pca9635rpi.h"

#include <stdlib.h>
#include <string.h>

static int debug=0;

static int minBrightness[_COLORS] = {
    3, 4, 16, 2, 3
};

// a value of 60 burned out a yellow LED
//  RED, BLUE, GREEN, YELLOW, WHITE, _COLORS
static int maxBrightness[_COLORS] = {
    10,  10,   40,    10,     22
};

static int deviceMapInitialized = FALSE;
static int deviceMap[pca9635_MaxDevices];
static int outputControl[pca9635_MaxDevices][16];

int setDeviceId(int handle) {
    if (!deviceMapInitialized) {
        for (int i = 0; i < pca9635_MaxDevices; ++i) {
            deviceMap[i] = -1;
        }
        deviceMapInitialized = TRUE;
    }
    for (int i = 0; i < pca9635_MaxDevices; ++i) {
        if (deviceMap[i] == -1) {
            deviceMap[i] = handle;
            return i;
        }
    }
    return -1;
}

int getDeviceId(int handle) {
    for (int i = 0; i < pca9635_MaxDevices; ++i) {
        if (deviceMap[i] == handle) {
            return i;
        }
    }
    return -1;
}


int pca9635SetBrightness(int handle, int led, int color, int percent) {
    int pin;

    if (led<16) {
      pin=led;
    } else {
      pin=pca9635_getLED(led);
    }

    int min = minBrightness[color];
    int max = maxBrightness[color];

    int delta = max - min;

    int brightness = min + (percent / 100.0) * delta;

    if (percent == 0) {
        return wiringPiI2CWriteReg8(handle, 0x02 + pin, 0);
    } else {
        return wiringPiI2CWriteReg8(handle, 0x02 + pin, brightness);
    }
}


void pca9635DigitalWrite(int handle, int led, int value) {

    int deviceId = getDeviceId(handle);
    int pin;

    if (led<16) {
      pin=led;
    } else {
      pin=pca9635_getLED(led);
    }

    outputControl[deviceId][pin] = !value;

    int regValue    = 0;
    int pinGroup    = pin / 4;
    int pinBase     = pinGroup * 4;
    int regAddr     = pinGroup + 0x14;

    regValue |= (0x03 & outputControl[deviceId][pinBase + 3]) << 6;
    regValue |= (0x03 & outputControl[deviceId][pinBase + 2]) << 4;
    regValue |= (0x03 & outputControl[deviceId][pinBase + 1]) << 2;
    regValue |= (0x03 & outputControl[deviceId][pinBase + 0]) << 0;

    wiringPiI2CWriteReg8(handle, regAddr, regValue);
}


int pca9635Setup(int address) {
    int handle = wiringPiI2CSetup(address);
    
    if (handle < 0) {
        return handle;
    }

    int deviceId = setDeviceId(handle);
    if (deviceId < 0) {
        return -1;
    }

    int mode1 = 0x01;
    int mode2 = 0x04;

    wiringPiI2CWriteReg8(handle, 0x00, mode1);
    wiringPiI2CWriteReg8(handle, 0x01, mode2);
    wiringPiI2CWriteReg8(handle, 0x14, 0xff);
    wiringPiI2CWriteReg8(handle, 0x15, 0xff);
    wiringPiI2CWriteReg8(handle, 0x16, 0xff);
    wiringPiI2CWriteReg8(handle, 0x17, 0xff);

    for (int i = 0; i < 16; ++i) {
        outputControl[deviceId][i] = 0x03;
    }

    int cmode1 = wiringPiI2CReadReg8(handle, 0x00);
    int cmode2 = wiringPiI2CReadReg8(handle, 0x01);

    if (cmode1 != mode1 || cmode2 != mode2) {
        return -1;
    }

    delayMicroseconds(500);  // mandatroy 500 us delay when enabling pca9635 oscillator 

    return handle;
}

PCA9635_TYPE pca9635_getType(PCA9635_ADDRESS address, PCA9635_COLOR color, PCA9635_LED led) {
    return (address << 8) | ((0x01 & color) << 4) | (0x0F & led);
}


PCA9635_TYPE pca9635_getTypeFromENV(const char* var) {
    if (!var) {
        fprintf(stderr, "Could not locate NULL in the environment variables\n");
        exit(EXIT_FAILURE);
    }
    if (getenv(var)) {
        int address;
        int color;
        int led;
        int offset = 0;

        if (strncmp(getenv(var), "0x", 2) == 0) {
            offset = 2;
        }

        sscanf(getenv(var) + offset, "%x_%d_%d", &address, &color, &led);
        
        if (debug) {
          printf("setting up pca9635 pin on address=%02x, color=%c, led=%d\n", address, color, led);
        }

        PCA9635_TYPE value = pca9635_getType(address, color, led);

        return value;
    }
    else {
        fprintf(stderr, "Could not locate '%s' in the environment variables\n", var);
        exit(EXIT_FAILURE);
    }
}

PCA9635_ADDRESS pca9635_getAddress(PCA9635_TYPE pin) {
    return pin >> 8;
}

PCA9635_COLOR pca9635_getColor(PCA9635_TYPE pin) {
    return (0x0010 & pin) >> 4;
}

PCA9635_LED pca9635_getLED(PCA9635_TYPE pin) {
    return 0x000F & pin;
}

