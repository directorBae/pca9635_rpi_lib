#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include <pca9635rpi.h>

int userLED = 0;
int userBrightness = 0;

int pca9635Handle = -1;
int pca9635Address = 0x0f;

int outputEnablePin = 3;

int minBrightness[_COLORS] = {
    3,4,16,2,3
};

int maxBrightness[_COLORS] = {
    60,75,150,75,22
};



int pinColor[16] = {
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    RED,
    RED,
    RED,
    RED,
    BLUE,
    BLUE,
    BLUE,
    BLUE,
    GREEN,
    GREEN,
    GREEN,
    GREEN
};

int getBrightness(int led, int percent) {
    int min = minBrightness[pinColor[led]];
    int max = maxBrightness[pinColor[led]];

    int delta = max - min;

    int brightness = (percent / 100.0) * delta;

    return min + brightness;
}

bool usage() {
    fprintf(stderr, "usage: pca9635 [-a address] -p PIN -b brightness\n");
    fprintf(stderr, "a = address of pca9635\n");

    return false;
}


bool commandLineOptions(int argc, char** argv) {
    int c, index;

    if (argc < 2) {
        return usage();
    }

    while ((c = getopt(argc, argv, "a:b:p:")) != -1)
        switch (c) {
        case 'a':
            sscanf(optarg, "%x", &pca9635Address);
            break;
        case 'p':
            sscanf(optarg, "%d", &userLED);
            break;
        case 'b':
            sscanf(optarg, "%d", &userBrightness);
            break;
        case '?':
            if (optopt == 'm' || optopt == 't')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character \\x%x.\n", optopt);

            return usage();
        default:
            abort();
        }


    //	for (int index = optind; index < argc; index++)
    //		printf("Non-option argument %s\n", argv[index]);
    return true;
}

int main(int argc, char **argv)
{
    
    if (!commandLineOptions(argc, argv)) {
        return 1;
    }


    if (wiringPiSetup() != 0) {
        printf("wiringPiSetup failed\n");
        return 2;
    }

    pinMode(outputEnablePin,OUTPUT);
    digitalWrite(outputEnablePin, LOW);

    pca9635Handle = wiringPiI2CSetup(pca9635Address);

    if (pca9635Handle < 0) {
        printf("Cannot initialize pca9635 on address 0x%02x\n",pca9635Address);
        return 2;
    }

    int mode1 = 0x01;
    int mode2 = 0x04;

    wiringPiI2CWriteReg8(pca9635Handle, 0x00, mode1);
    wiringPiI2CWriteReg8(pca9635Handle, 0x01, mode2);

    wiringPiI2CWriteReg8(pca9635Handle, 0x14, 0xaa);
    wiringPiI2CWriteReg8(pca9635Handle, 0x15, 0xaa);
    wiringPiI2CWriteReg8(pca9635Handle, 0x16, 0xaa);
    wiringPiI2CWriteReg8(pca9635Handle, 0x17, 0xaa);
    delay(1);  // mandatroy 500 us delay when enabling pca9635 oscillator 

    int cmode1 = wiringPiI2CReadReg8(pca9635Handle, 0x00);
    int cmode2 = wiringPiI2CReadReg8(pca9635Handle, 0x01);



    if (cmode1 != mode1 || cmode2 != mode2) {
        printf("pca9635 initialization failed\n");
        return 2;
    }


    printf("initialization complete, pca9635 handle=%d\n",pca9635Handle);




    wiringPiI2CWriteReg8(pca9635Handle, 0x02 + userLED, userBrightness );

    return 0;
}
