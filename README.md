# Dagu-Rover-5-with-Joystick
Dagu Rover 5
This project is using Dagu Rover (https://www.pololu.com/product/1551) together with a Beaglebone Black (BBB) and joystick to control it.
Installation:
1.	BBB setup
You have to prepare to update the BBB OS to have the latest OS inside. Download the latest OS image from https://beagleboard.org/latest-images. 
You will get the compressed image file. Decompress it first and get .img file. 
On Windows:
Download and install Image Writer for Windows. Be sure to download the binary distribution. Connect the SD Card to your computer and using the image writer utility write the img file to the SD Card. 
On Linux:
Run fdisk –l command and remember what hardware will be shown there.
Connect SD Card to your computer and find out where it is connected using fdisk -l command. You will notice a new disk there. We assume it is /dev/sdb. Run the following command to write image to SD Card. 

```
dd if=/path/to/img/file of=/dev/sdb
```

After finishing of the writing image process, open the SD Card in your computer. 
In /boot/uEnv.txt find the following lines:

```
#cmdline=init=/opt/scripts/tools/eMMC/init-eMMC-flasher-v3.sh
```

Change to:

```
cmdline=init=/opt/scripts/tools/eMMC/init-eMMC-flasher-v3.sh
```

Now your SD Card is ready. Put the SD Card into BBB, push the “BOOT” button on it and power the BBB (or restart it). After some time the LEDs start blinking in sequence. Wait until this process is finished (most probably after it finishes the BBB will power off). 
After it finishes, be sure to get the SD Card out from BBB, and only after that power on your BBB. Now you are ready to use BBB with latest OS.

2.	Connection of Dagu Rover with BBB
To control the Dagu Rover you need to use the Dual Motor Driver. We are using the Pololu driver: https://www.pololu.com/product/2130.

![pololu chip](https://image.ibb.co/djmT35/polulu.png) 

There are 4 input pins (blue) on the driver chip and 4 output pins (green) on the image above. The red ones are used for external power supply. The motors require maximum about 7.2V. So you have to connect such a battery to the GND and VIN pins of this chip. The input pins should be connected to BBB. Please follow the picture below for how to connect the input pins of Pololu with BBB:

![bbb pinout](https://image.ibb.co/kncqGQ/bbbpinout.png) 

The red rectangle in the picture above goes to BIN1 and BIN2 of the Pololu driver chip. GPIO_69 goes to BIN1, GPIO_68 to BIN2 pin. AIN1 goes to GPIO_45 and AIN2 goes to GPIO_44. 
After correct connection of those pins next step is to connect the output pins to the motors of the Dagu Rover. 
Identify the motor wires of Dagu Rover, you can see them in the picture below in red rectangles:
 
![Rover](https://image.ibb.co/fsseAk/dagu.png) 

There are 4 motors there, so be sure to select two of them which are from different sides of the Dagu Rover, we are not using all 4 motors. Use the red wire of Dagu Motor to connect it with AOUT1 and the black one with AOUT2, do the same connection of the second motor wires with BOUT1 and BOUT2. 

Now you have to connect the battery to the Pololu driver. To make it, we used the 7.2V batteries. You have to connect Battery ground to the GND of the Pololu driver Chip and the Voltage of battery goes to VIN pin of Pololu driver chip. In case if you have more powerful battery (higher than 7.2V), you can use voltage regulator to reduce the voltage to the desired level (less than or equal to 7.2V). You can use the following picture to use voltage regulator:

![Voltage regulator](https://image.ibb.co/fuAXVk/vr.png) 
 
It’s input should be connected with battery voltage, ground with battery ground and output should produce the needed voltage, which you connect with VIN of Pololu driver chip, and battery ground goes to GND of Pololu driver chip.

3.	Joystick
We are using ACME gamepad joystick. The OS of BBB used in our case is Debian 8.7 2017-03-19 4GB SD LXQT. Previously we used older version and it didn’t provide enough power for using joystick and also it had bugs that made it impossible to use the joystick after some time. Use the latest OS to be able to use joystick without additional power. Joystick should be connected through USB port of BBB. If you still can’t use it because it doesn’t provide enough power, you have to supply BBB power from DC 5V, not from USB or from external batteries. You might want to test if the gamepad is working or not by installing the jstest on your BBB OS:
sudo apt install joystick
Then you can run 
jstest /dev/input/js0 
and check if its working. The joystick should appear in the /dev/input/ folder. Usually it is seen as js0 file. If you successfully tested joystick and you see the js0 file in /dev/input, then you are ready to run the program. 
Joystick data and protocol of it’s data sent can be seen in joystick.h and joystick.c files.

4.	Compiling code and running
Download the repository code make the following steps to run it. 

```
./compile.sh #use it to compile the program. It uses main.c and joystick.c.
```

Then you have to run the 

```
./run.sh
```
Use it to actually start the program. The script will first unexport the pins used, then export them. And finally start the program.

5.	Code explanation 
We are using 4 input pins for controlling the motors using the Pololu driver chip. They are considered as digital input pins for Pololu driver chip. For example to control the motor A (first motor) you have to send to AIN1 value 1 (HIGH) and to AIN2 value 0 (LOW) or vice versa (0 and 1) to make the motor rotate in either forward or backward direction. To stop the motor A you have to send either 0 or 1 to both of inputs (AIN1 and AIN2). Same with motor B (BIN1 and BIN2). In the program you have to prepare the GPIO pins for this. First of all you have to export pins using the following command (see the exportPins function in main.c): 

```
FILE *handle = fopen("/sys/class/gpio/export", "ab");
fwrite("44", sizeof(char), 2, handle);
```

This commands will open the file in /sys/class/gpio/export and write there the value 44 (2 bytes). (remember the picture above on BBB pinout map, we are using GPIO 44, 45, 68, 69). The same procedure is done for other 3 pins. And finally you close the handle:

```
fclose(handle);
```

Now you have to set the direction of data for these pins (see the setDirections function in main.c). To set the direction you have to open the direction directory:
```
FILE *handle = fopen("/sys/class/gpio/gpio44/direction", "rb+");
```
Now you have to set the direction, our pins on BBB are sending the data, so the direction should be “out”:
```
fwrite("out", sizeof(char), 3, handle);
fclose(handle);
```
Do the same operations for pins with numbers 45, 68 and 69. Now preparation of pins is finished. The program in main.c goes into loop:

```
while (done == 0) {
        operateMotor();
        usleep(1000); // wait 
}
```

The operateMotor function checks the two global variables, statusA and statusB. They represent the status of the motors. They can have values 0, -1 and 1. statusA and statusB value 1 means that the appropriate motor (A or B) should go forward, -1 means going backward and finally 0 value means stop. The joystick input should change the values of statusA and statusB which will be explained later. motorStop, motorA and motorB function in main.c are used to control the two motors. To stop the motor A you have to call motorStop(0, 0); To stop motor B call the motorStop(1, 0); So use for the first argument values 0 or 1 depending on which motor you want to stop (0 for A, 1 for B). The second argument is not important (it’s the way to stop the motor. Remember above we explained that to stop the motor you have to write 0 or 1 to both input pins of the selected motor, second argument is the value 0 or 1 to be written). 
To start the motor in some direction use the motorA or motorB function. It has one argument, if it is 0 then motor should go backwards, and 1 for forward. All of these functions use common motor(int type, int value) function. In order to write LOW value (as an example) to the GPIO pin you have to use the following code:

```
FILE *handle = fopen("/sys/class/gpio/gpio44/value", "rb+");
fwrite("0", sizeof(char), 1, handle);
fclose(handle);
```

In the example above we have written value 0 (LOW) to GPIO pin 44. You have to change the 44 value to write to a different pin (45, 68 or 69). 
Here is the code of the motorA and motorB functions:
```
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
```


The motorStop function is like this:
```
int motorStop(int motorType, int stopType) { // motorType: 0-A, 1-B
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
```

As you see from all of these function above in order to control the motor you have to write to the two of it’s pins (44 and 45 or 68 and 69) some values. To write them these functions are using the motor function:

```
int motor(int type, int val) {
        FILE *handleFile = NULL;
        if (type == 0) {
                if ((handleFile = fopen(gpioValueA1, "rb+")) == NULL){
                        return 1;
                }
        } else if (type == 1) {
                if ((handleFile = fopen(gpioValueA2, "rb+")) == NULL){
                        return 1;
                }
        } else if (type == 2) {
                if ((handleFile = fopen(gpioValueB1, "rb+")) == NULL){
                        return 1;
                }
        } else if (type == 3) {
                if ((handleFile = fopen(gpioValueB2, "rb+")) == NULL){
                        return 1;
                }
        }
        if (val == 0) {
                strcpy(setValue, "0"); // Set value low
        } else if (val == 1) {
                strcpy(setValue, "1"); // Set value high
        }
        fwrite(setValue, sizeof(char), 1, handleFile);
        fclose(handleFile);
        return 0;
}
```

In the code above the gpioValueA1, A2, B1 and B2 are having the following format:
"/sys/class/gpio/gpio44/value" as it was described above. 

To get input data from joystick in the main.c we ran the separate thread which uses the open_joystick function and read_joystick_event functions from joystick.{c, h} files. If joystick sends some data the checkJsInput will analyze it and change the statusA and statusB variables accordingly (as it was explained above, the statusA and statusB variables are used to reflect the motors behavior). 
The open_joystick function uses the following call to open the /dev/input/js0 in non blocking mode:

```
int joystick_fd = open(joystick_device, O_RDWR | O_NONBLOCK);
```

The reading from joystick is done in the following function:
```
int read_joystick_event(struct js_event *jse)
{
        int bytes;
        bytes = read(joystick_fd, jse, sizeof(*jse));
        if (bytes == -1)
                return 0;
        if (bytes > 0)
                return 1;
        printf("Unexpected bytes from joystick:%d\n", bytes);
        return -1;
}
```

If no data present (no button yet pressed) it will return -1, if something is pressed the joystick will send the data in the following format: 
```
struct js_event {
        unsigned int time;      /* event timestamp in milliseconds */
        short value;            /* value */
        unsigned char type;     /* event type */
        unsigned char number;   /* axis/button number */
};
```

In our case, we are using (with our acme gamepad) buttons with type equal to 2 and number equal to 1 or 3 which corresponds to the two joystick buttons on the gamepad, with the direction of up and down (left and right are using different number code and not used in our program). Up means going forward and down means going backward. After running the program you will see those values after pressing the buttons. When you start the program by some reason it reads some trash values from joystick. Be aware of that. It does it only at the start (as if several random buttons were pressed) then it doesn’t send trash values anymore. 
