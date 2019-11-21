#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>

int pca9635Handle = -1;
int pca9635Address = 0x0f;

int outputEnablePin = 3;

enum color {
    RED, BLUE, GREEN, YELLOW, WHITE, _COLORS
};

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


int main(void)
{
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







    for (int led = 0; led < 16; ++led) {
        wiringPiI2CWriteReg8(pca9635Handle, 0x02 + led, maxBrightness[pinColor[led]]);
    }
    delay(500);

    for (int led = 0; led < 16; ++led) {
        wiringPiI2CWriteReg8(pca9635Handle, 0x02 + led, minBrightness[pinColor[led]]);
    }

    delay(500);



	while (true) {
        int brightness = 0;
        int led = 0;
        printf("."); fflush(stdout);
        for (brightness = 0; brightness < 100; brightness+=2) {
            for (led = 0; led < 16; ++led) {
                wiringPiI2CWriteReg8(pca9635Handle, 0x02 + led, getBrightness(led,brightness));
            }
            delay(2);
        }
        delay(500);

        for (brightness = 100; brightness > 0 ; brightness-=2) {
            for (led = 0; led < 16; ++led) {
                wiringPiI2CWriteReg8(pca9635Handle, 0x02 + led, getBrightness(led, brightness));
            }
        }
        delay(500);


        for (led = 0; led < 16; ++led) {
            printf("%x", led); fflush(stdout);
            for (brightness = 0; brightness < 100; brightness += 2) {
                wiringPiI2CWriteReg8(pca9635Handle, 0x02 + led, getBrightness(led, brightness));
            }
            delay(500);
            for (brightness = 100; brightness > 0; brightness -= 2) {
                wiringPiI2CWriteReg8(pca9635Handle, 0x02 + led, getBrightness(led, brightness));
            }
            delay(500);
        }
        printf("\n"); fflush(stdout);

	}
	return 0;
}