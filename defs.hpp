#ifndef DEFS_HPP
#define DEFS_HPP


#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
using namespace cv;
using namespace std;


#ifdef GPIO_UART_CPP

//Discrete Inputs/Outputs:
//NOTE: THESE USE BROADCOM NUMBERS SINCE WiringPi DID NOT MAP THEM CORRECTLY
//NOTE: Output at end of variable means "real-world output" 
int breakBeamInput = 2;						//GPIO pin 27 input from break beam (garage-door-like sensor)
int shutdownPiSwitchInput = 5;					//GPIO pin 24 input to run script to nicely power off RPi2 PowerLED

//Input at end of variable means "real-world input"
int breakBeamLEDOutput = 0;					//GPIO pin 17 output a test output for the Break Beam
int shootPinOutput = 1;						//GPIO pin 18 output controls the solenoid discrete output
int controllerConnectedLEDOutput = 3;				//GPIO pin 22 output controls the solenoid discrete output
int outputEnableAndGate = 4;					//GPIO pin 23 output controls the solenoid discrete output

#endif //GPIO_UART_CPP


#ifdef XBOX360CONTROLLER

//Discrete Inputs/Outputs:
//NOTE: THESE USE BROADCOM NUMBERS SINCE WiringPi DID NOT MAP THEM CORRECTLY
//NOTE: Output at end of variable means "real-world output" 
int breakBeamInput = 2;						//GPIO pin 27 input from break beam (garage-door-like sensor)
int shutdownPiSwitchInput = 5;					//GPIO pin 24 input to run script to nicely power off RPi2 PowerLED

//Input at end of variable means "real-world input"
int breakBeamLEDOutput = 0;					//GPIO pin 17 output a test output for the Break Beam
int shootPinOutput = 1;						//GPIO pin 18 output controls the solenoid discrete output
int controllerConnectedLEDOutput = 3;				//GPIO pin 22 output controls the solenoid discrete output
int outputEnableAndGate = 4;					//GPIO pin 23 output controls the solenoid discrete output

#endif //XBOX360CONTROLLER


#define RaspberryPi2Used 1
#define PRINT_CONTROLLER_DATA 1
#define PRINT_SERIAL_DATA 1
#define SOFTWARE_EMERGENCY_STOP 1
#define PERMIT_SHUTDOWN_PI_USING_GPIO_OR_CONTROLLER 1
//#define ShowDetectedObjects 1
#define USING_WEBCAM 1 //flag that is set to control whether the user uses the camera for input or a still picture as an input
#define CALIBRATION_MODE 1 //calibrate the HSV filter for a specific color
#define CAMERA_NUMBER 0 //flag to set source of video: "camera 0" is the builtin laptop webcam, "camera 1" is usb webcam
#define MAX_NUM_OBJECTS 30 // Program will only track 30 objects at a time (this is just in case noise becomes a problem)
//#define MIN_OBJECT_AREA 400 //Only allow larger objects //200*200
#define MIN_OBJECT_AREA 1000 //Only allow larger objects
#define JOYSTICK_DEADZONE 10000 //only care about joystick values outside of the deadzone + & - around 0

//default display capture window frame width and height (640x480 window)
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;


#define BLUE (Scalar(255, 0, 0)) //BGR, not RGB (I do not know why colors are flipped for OPENCV
#define GREEN (Scalar(0, 255, 0))
#define RED (Scalar(0, 0, 255))
#define YELLOW (Scalar(0, 255, 255))
#define PURPLE (Scalar(255, 0, 255))
#define WHITE (Scalar(255, 255, 255))
#define BLACK (Scalar(0, 0, 0))

#endif /* DEFS_HPP */