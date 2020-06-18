utility: utility.cpp
	gcc -lwiringPi -lwiringPiPCA9635rpi utility.cpp -o pca9635

flashlight: main.cpp
	gcc -lwiringPi -lwiringPiPCA9635rpi main.cpp -o flashlight

clean:
	rm flashlight pca9635

install: utility
	sudo cp pca9635 /usr/local/bin
