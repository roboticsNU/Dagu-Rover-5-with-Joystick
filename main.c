#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "joystick.h"

using namespace std;

int a1 = 44, a2 = 45;
int b1 = 69, b2 = 68;

#define GPIOStringA1 "44"
#define GPIOStringA2 "45"
#define GPIOStringB1 "69"
#define GPIOStringB2 "68"

#define gpioValueA1 "/sys/class/gpio/gpio" GPIOStringA1 "/value"
#define gpioValueA2 "/sys/class/gpio/gpio" GPIOStringA2 "/value"
#define gpioValueB1 "/sys/class/gpio/gpio" GPIOStringB1 "/value"
#define gpioValueB2 "/sys/class/gpio/gpio" GPIOStringB2 "/value"

//#define gpioValueEncA "/sys/class/gpio/gpio44/value"
//#define gpioValueEncB "/sys/class/gpio/gpio45/value"


#define gpioDirA1 "/sys/class/gpio/gpio" GPIOStringA1 "/direction"
#define gpioDirA2 "/sys/class/gpio/gpio" GPIOStringA2 "/direction"
#define gpioDirB1 "/sys/class/gpio/gpio" GPIOStringB1 "/direction"
#define gpioDirB2 "/sys/class/gpio/gpio" GPIOStringB2 "/direction"

//#define gpioDirEncA "/sys/class/gpio/gpio44/direction"
//#define gpioDirEncB "/sys/class/gpio/gpio45/direction"

//#define GPIOStringEncA "44"
//#define GPIOStringEncB "45"

char setValue[4];
int done = 0;

volatile int statusA = 0, statusB = 0;

int motor(int type, int val);
int motorStop(int motorType, int stopType) { // 0-A, 1-B
        int v1 = 0, v2 = 0;
        if (stopType == 0) {
        } else {
                v1 = 1;
                v2 = 1;
        }

        if (motorType == 0) { // A
                motor(0, v1);
                motor(1, v2);
        } else {
                motor(2, v1);
                motor(3, v2);
        }
}
int motorA(int forward) {
        if (forward == 0) {
                motor(0, 1);
                motor(1, 0);
        } else {
                motor(0, 0);
                motor(1, 1);
        }
}

int motorB(int forward) {
        if (forward == 0) {
                motor(2, 0);
                motor(3, 1);
        } else {
                motor(2, 1);
                motor(3, 0);

        }
}

int motor(int type, int val) {
        FILE *handleFile = NULL;

        if (type == 0) {
                if ((handleFile = fopen(gpioValueA1, "rb+")) == NULL){
                        //printf("motor: Unable to open value handle\n");
                        return 1;
                }
        } else if (type == 1) {
                if ((handleFile = fopen(gpioValueA2, "rb+")) == NULL){
                        //printf("motor: Unable to open value handle\n");
                        return 1;
                }
        } else if (type == 2) {
                if ((handleFile = fopen(gpioValueB1, "rb+")) == NULL){
                        //printf("motor: Unable to open value handle\n");
                        return 1;
                }
        } else if (type == 3) {
                if ((handleFile = fopen(gpioValueB2, "rb+")) == NULL){
                        //printf("motor: Unable to open value handle\n");
                        return 1;
                }
        }



        if (val == 0) {
                strcpy(setValue, "0"); // Set value high
        } else if (val == 1) {
                strcpy(setValue, "1"); // Set value high
        }
    	fwrite(setValue, sizeof(char), 1, handleFile);
    	fclose(handleFile);
    	return 0;
}

int exportPins() {
    // Export the pin
    FILE *handleFile = NULL;
    printf("export pins\n");
    if ((handleFile = fopen("/sys/class/gpio/export", "ab")) == NULL){
        printf("Unable to export GPIO pin\n");
        return 1;
    }

    strcpy(setValue, GPIOStringA1);
    fwrite(setValue, sizeof(char), 2, handleFile);

    strcpy(setValue, GPIOStringA2);
    fwrite(setValue, sizeof(char), 2, handleFile);

    strcpy(setValue, GPIOStringB1);
    fwrite(setValue, sizeof(char), 2, handleFile);

    strcpy(setValue, GPIOStringB2);
    fwrite(setValue, sizeof(char), 2, handleFile);

/*    strcpy(setValue, GPIOStringEncA);
    fwrite(&setValue, sizeof(char), 2, handleFile);
    strcpy(setValue, GPIOStringEncB);
    fwrite(&setValue, sizeof(char), 2, handleFile);*/

    fclose(handleFile);
    return 0;
}

int setDirections() {
    // Set direction of the pin to an output
    FILE *handleFile = NULL;
    printf("set directions\n");
    if ((handleFile = fopen(gpioDirA1, "rb+")) == NULL){
        printf("Unable to open direction handle a1\n");
        return 1;
    }
    strcpy(setValue,"out");
    fwrite(setValue, sizeof(char), 3, handleFile);
    fclose(handleFile);

    if ((handleFile = fopen(gpioDirA2, "rb+")) == NULL){
        printf("Unable to open direction handle a2\n");
        return 1;
    }
    strcpy(setValue,"out");
    fwrite(setValue, sizeof(char), 3, handleFile);
    fclose(handleFile);

    if ((handleFile = fopen(gpioDirB1, "rb+")) == NULL){
        printf("Unable to open direction handle b1\n");
        return 1;
    }
    strcpy(setValue,"out");
    fwrite(setValue, sizeof(char), 3, handleFile);
    fclose(handleFile);

    if ((handleFile = fopen(gpioDirB2, "rb+")) == NULL){
        printf("Unable to open direction handle b2\n");
        return 1;
    }
    strcpy(setValue,"out");
    fwrite(setValue, sizeof(char), 3, handleFile);
    fclose(handleFile);

/*
    if ((handleFile = fopen(gpioDirEncA, "rb+")) == NULL){
        printf("Unable to open direction handle\n");
        return 1;
    }
    strcpy(setValue,"in");
    fwrite(&setValue, sizeof(char), 2, handleFile);
    fclose(handleFile);

    if ((handleFile = fopen(gpioDirEncB, "rb+")) == NULL){
        printf("Unable to open direction handle\n");
        return 1;
    }
    strcpy(setValue,"in");
    fwrite(&setValue, sizeof(char), 2, handleFile);
    fclose(handleFile);*/

    return 0;
}

void operateMotor() {
//	motorB(1);
//	motorStop(0,0);
//	return;
	if (statusA == 0) {
		//printf("motor stop A\n");
		motorStop(0, 0);
	} else if (statusA == 1) {
		//printf("motor FW A\n");
		motorA(1);
	} else if (statusA == -1) {
		//printf("motor BW A\n");
		motorA(0);
	}

	if (statusB == 0) {
		//printf("motor stop B\n");
                motorStop(1, 0);
        } else if (statusB == 1) {
		//printf("motor FW B\n");
                motorB(1);
        } else if (statusB == -1) {
		//printf("motor BW B\n");
                motorB(0);
        }

}

void *joystickThread(void *arg);
int main(int argc, char** argv) {

    FILE *handleFile;
    pthread_t jstick;
    pthread_create(&jstick, NULL, joystickThread, NULL);

    printf("%s\n", gpioDirA1);
    exportPins();
    setDirections();

    while (done == 0) {
        // Set output to high
	operateMotor();
        usleep(1000); // wait for 1 sec
    }

    motorStop(0, 0);
    motorStop(1, 0);

    // Unexport the pin
    if ((handleFile = fopen("/sys/class/gpio/unexport", "ab")) == NULL) {
        printf("Unable to unexport GPIO pin\n");
        return 1;
    }

    strcpy(setValue, GPIOStringA1);
    fwrite(setValue, sizeof(char), 2, handleFile);
    strcpy(setValue, GPIOStringA2);
    fwrite(setValue, sizeof(char), 2, handleFile);
    strcpy(setValue, GPIOStringB1);
    fwrite(setValue, sizeof(char), 2, handleFile);
    strcpy(setValue, GPIOStringB2);
    fwrite(setValue, sizeof(char), 2, handleFile);

    fclose(handleFile);
    printf("\nCompleted GPIO output program\n");

    return 0;
}
void checkJsInput(short value, unsigned char number, unsigned char type) {
	if (number == 0 && type == 1) {
		// stop button
		//done = 1;
	} else if (type == 2) {
		/* Button with number 1 - is first motor A, number 3 - motor B
		   value < 0 - forward, value > 0 - backward, value == 0 - stop
		*/
		if (number == 1) {
			// A
			if (value < 0) {
				// forward
				statusA = 1;
			} else if (value > 0) {
				// backward
				statusA = -1;
			} else {
				// stop
				statusA = 0;
			}
		} else if (number == 3) {
			if (value < 0) {
                                // forward
                                statusB = 1;
                        } else if (value > 0) {
				// backward
                                statusB = -1;
                        } else {
				// stop
                                statusB = 0;
                        }
		}
	}
}

void *joystickThread(void *arg) {
        int fd, rc;

        struct js_event jse;

        if (arg != NULL) {
                fd = open_joystick((char *)arg);
        } else {
                fd = open_joystick(NULL);
        }
        if (fd < 0) {
                printf("open failed.\n");
                return NULL;
        }

	int counter = 0;
        while (!done) {
                rc = read_joystick_event(&jse);
                if (rc == 1) {
			if (counter > 5) {
	                        printf("Event: time %8u, value %8hd, type: %3u, axis/button: %u\n",
        	                        jse.time, jse.value, jse.type, jse.number);
				checkJsInput(jse.value, jse.number, jse.type);
				printf("Status A,B: %d, %d\n", statusA, statusB);
			} else {
				++counter;
			}
                } else {
                }
                usleep(1);
        }
        pthread_exit(0);
}
