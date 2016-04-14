//File utilizes some of the I/O on the pin headers for the RaspberryPi2
//The pins used are discrete I/O and UART (TX only, not RX)

#include "defs.hpp"

#ifdef RaspberryPi2Used //only use this file if the "RaspberryPi2Used" flag is set


#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include "GPIO_UART.hpp"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <errno.h>

#define bool _Bool //I had to use booleans ("bool"), but Linux uses "_Bool" for boolean variables
#define JOY_DEV "/dev/input/js0" //Define the device that the controller data is pulled from
#define PRINT_CONTROLLER_DATA 1
#define PRINT_SERIAL_DATA 1


//initialize the GPIO and UART pins for the Raspberry Pi 2
int initGPIO_Uart(void) {

	system("gpio mode 1 out"); //set GPIO pin 1 to output
	system("gpio mode 2 in"); //set GPIO pin 2 to input

	//Initialize the Wiring Pi Libary
	pinMode(breakBeamInput, INPUT);
	pullUpDnControl(breakBeamInput, PUD_DOWN); // Enable pull-down resistor on button
	system("gpio mode 2 in"); //force shootPin to input, configure pin using terminal command
	pinMode(ShutdownPiSwitchInput, INPUT);
	pullUpDnControl(ShutdownPiSwitchInput, PUD_DOWN); // Enable pull-down resistor on button
	pinMode(breakBeamLEDOutput, OUTPUT);
	pinMode(shootPinOutput, OUTPUT);
	system("gpio mode 1 out"); //force shootPin to output, configure pin using terminal command
	
	//initialize the UART @ 19200 BAUD
	if ((UART_ID = serialOpen("/dev/ttyAMA0", 19200)) < 0) {
		fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
		return 0;
	}

	//Initialize WiringPi -- using Broadcom processor pin numbers
	wiringPiSetupGpio();



	usleep(2000000); //wait 2 seconds (in microseconds) to act as a power up delay for the Sabertooth Motor Controller
	serialPutchar(UART_ID, 0xAA); //Send the autobauding character to Sabertooth first to stop motors from twitching!
	usleep(100000); //wait 100ms (in microseconds) before commanding motors


	if (wiringPiSetup() == -1) {
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
		return 0;
	}

	printf("Initialized GPIO and UART!\r\n");
	//if return(0), something did not get initialized correctly
}



//Feed Xbox controller Joystick (16-bit integer using built-in xpad driver in Linux) as an input
//and send the scaled output to the "Sabertooth 2x25 Motor Controller V2.00" as a Simplified Serial Input
void sendMotorControllerSpeedBytes(int UART_PORT_ID, int LeftYvalueControllerInput, int RightYvalueControllerInput) {
	//Left Motor:		0: Full reverse			64: Neutral		127: Full Forward
	//Right Motor:		128: Full reverse		192: Neutral		255: Full Forward

	static int maxControllerJoystickInput = 32767;
	static int DEADZONE = 16384; //uses only half of the joystick range as usable values (32768/2 == 2^14 == 16384)
	unsigned char RightMotorSerialOutput = 0, LeftMotorSerialOutput = 0;
	static int Ldelta = 127 - 63; //will have actual slope of ((127-64)/(2^14)) or ((127-64)>>14), but that will be later using fixed point and bit shifting
	static int Rdelta = 255 - 191; //will have actual slope of ((255-192)/(2^14)) or ((255-192)>>14), but that will be later using fixed point and bit shifting
	//millis() values for running parts of the code routinely, but allow the code to be "unblocked" without using delays
	int nextMilliSecondCountLeftUART = 0, nextMilliSecondCountRightUART = 0;

	//check if the controller is outside the y-axis dead zone for each joystick
	if (abs(LeftYvalueControllerInput) > DEADZONE) {

		bool negLeftInput = 0;

		if (LeftYvalueControllerInput < 0) { //set negative input flag and convert to positive value
			LeftYvalueControllerInput = (unsigned int)LeftYvalueControllerInput; //make joystick positive
			negLeftInput = 1;
		}
		//input is positive
		if (negLeftInput == 0) { //input is positive
			LeftMotorSerialOutput = (unsigned char)(64 + (((LeftYvalueControllerInput - DEADZONE)*Ldelta) >> 14)); //64 is motr controller offset for Left Motor Neutral
		}
		//input is negative
		if (negLeftInput == 1) { //input is negative
			LeftMotorSerialOutput = (unsigned char)(-(64 - (((LeftYvalueControllerInput - DEADZONE)*Ldelta) >> 14))); //64 is motr controller offset for Left Motor Neutral
		}

		//clip the maximum allowed magnitude for y-axis joystick at their expected maximum values
		if (LeftMotorSerialOutput > 126) LeftMotorSerialOutput = 127;

		//clip the minimum allowed magnitude for y-axis joystick at their expected minimum values
		if (LeftMotorSerialOutput < 2) LeftMotorSerialOutput = 1;
	}

	//if controller y-values are within DEADZONE, command the left motor to neutral (off)
	else {
		LeftMotorSerialOutput = 64;
	}

	//check if the controller is outside the y-axis dead zone for each joystick
	if (abs(RightYvalueControllerInput) > DEADZONE) {

		bool negRightInput = 0;

		if (RightYvalueControllerInput < 0) { //set negative input flag and convert to positive value
			RightYvalueControllerInput = (unsigned int)RightYvalueControllerInput; //make joystick positive
			negRightInput = 1;
		}

		if (negRightInput == 0) { //input is positive
			RightMotorSerialOutput = (unsigned char)(192 + (((RightYvalueControllerInput - DEADZONE)*Rdelta) >> 14)); //192 is motr controller offset for Right Motor Neutral
		}

		if (negRightInput == 1) { //input is negative
			RightMotorSerialOutput = (unsigned char)(-(192 - (((RightYvalueControllerInput - DEADZONE)*Rdelta) >> 14))); //192 is motr controller offset for Right Motor Neutral
		}

		//clip the maximum allowed magnitude for y-axis joystick at their expected maximum values
		if (RightMotorSerialOutput > 254) RightMotorSerialOutput = 255;

		//clip the minimum allowed magnitude for y-axis joystick at their expected minimum values
		if (RightMotorSerialOutput < 129) RightMotorSerialOutput = 128;
	}

	//if controller y-values are within DEADZONE, command the right motor to neutral (off)
	else {
		RightMotorSerialOutput = 192;
	}


#ifdef PRINT_SERIAL_DATA
	printf("Serial_L:%d\r\n", LeftMotorSerialOutput);
	printf("Serial_R:%d\r\n", RightMotorSerialOutput);
#endif //PRINT_SERIAL_DATA	


	if (millis() > nextMilliSecondCountLeftUART) {
		serialPutchar(UART_PORT_ID, LeftMotorSerialOutput);
		nextMilliSecondCountLeftUART += 120; //run this "if() statement in 120 milliseconds
	}

	if (millis() > nextMilliSecondCountRightUART) {
		serialPutchar(UART_PORT_ID, RightMotorSerialOutput);
		nextMilliSecondCountRightUART += 80; //run this "if() statement in 80 milliseconds
	}
}




int gpioPinOperations(void) {

	int nextMilliSecondCountLEDS = 0;
	int nextMilliSecondCountGPIO = 0;
	int nextMilliSecondCountBreakBeam = 0;
	static short shutdownCount = 0;

	/*Now read the input GPIO pins for control necessary pins that need a higher update rate*/
	if (millis() > nextMilliSecondCountLEDS) {

		//goodData not set, so controller not connected
		if (goodData == 0) {
			printf("BadData\r\n");
			digitalWrite(controllerConnectedLEDOutput, LOW);
		}

		//detect whether a puck is in the launching cavity. If so, permit the user to use the shooting mechanism
		if ( digitalRead(breakBeamInput) == 1) {
			printf("BreakBeam ON\r\n");
			digitalWrite(breakBeamLEDOutput, HIGH);
			shootPermissive = 1;
		}
		else {
			digitalWrite(breakBeamLEDOutput, LOW);
			printf("BreakBeam OFF\r\n");
		}
		nextMilliSecondCountLEDS += 300;
	}


#ifdef PERMIT_SHUTDOWN_PI_USING_GPIO_OR_CONTROLLER
	/*Now read the input GPIO pins for things that dont need a high update rate*/
	if (millis() > nextMilliSecondCountGPIO) {
		if (digitalRead(shutdownPiSwitchInput) == 1) {
			//if shutdown counter reaches 5 seconds, shutdown the Pi
			if (shutdownCount > 5) {
				//system("sudo shutdown -k now"); //send test shutdown message
				system("sudo shutdown -P now"); //shutdown Pi and power off immediately
			}
			shutdownCount++;
		}
		else {
			shutdownCount = 0; //reset shutdown counter
		}

		nextMilliSecondCountGPIO += 1000; //check every 1000ms (1 time every second)
	}
#endif //PERMIT_SHUTDOWN_PI_USING_GPIO_OR_CONTROLLER


	return 1;
}

#endif