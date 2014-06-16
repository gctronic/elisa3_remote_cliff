#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "elisa3-lib.h"

#ifdef _WIN32
    #include "windows.h"
#endif

#define CLIFF_THR 400
#define LEFT_ROT  1
#define RIGHT_ROT 2
#define GROUND_RIGHT 0
#define GROUND_CENTER_RIGHT 1
#define GROUND_CENTER_LEFT 2
#define GROUND_LEFT 3

// received from robot
int robotAddress;
unsigned int robGround[8] = {0};

// sent to robot
char robLSpeed=0, robRSpeed=0;
char robRedLed=0, robGreenLed=0, robBlueLed=0;

// various
unsigned int i;
unsigned int minGroundValue = 0;
unsigned int minGround = GROUND_LEFT;
unsigned char prevRot = 0;

int main(void) {

    srand ( time(NULL) );

    printf("\r\nInsert the robot address: ");
    scanf("%d", &robotAddress);

    // init the communication with the RF module and thus with the robot
    openRobotComm();

    // set the address of the robot to control; if more robots (max of 4 per packet) need to be controlled
    // the function need to be called more times with different ids (0..3) and addresses
    setRobotAddress(0, robotAddress);

    resetFlagTX(robotAddress);  // no onboard obstacle avoidance

    i=0;

    while(1) {

        i = (i+1)%100;  // use to change the rgb leds
        if(i==0) {
            robRedLed= rand()%100;
            robGreenLed = rand()%100;
            robBlueLed = rand()%100;
        }
        setRed(robotAddress, robRedLed);
        setGreen(robotAddress, robGreenLed);
        setBlue(robotAddress, robBlueLed);

        getAllGround(robotAddress, robGround);

		minGroundValue = robGround[0];
		minGround = GROUND_LEFT;
		if(robGround[1] < minGroundValue) {
			minGroundValue = robGround[1];
			minGround = GROUND_CENTER_LEFT;
		}
		if(robGround[2] < minGroundValue) {
			minGroundValue = robGround[2];
			minGround = GROUND_CENTER_RIGHT;
		}
		if(robGround[3] < minGroundValue) {
			minGroundValue = robGround[3];
			minGround = GROUND_RIGHT;
		}

        if(minGroundValue <= CLIFF_THR) {

            robLSpeed = 0;
            robRSpeed = 0;
            setLeftSpeed(robotAddress, robLSpeed);  // modify the packet to send to the robot
            setRightSpeed(robotAddress, robRSpeed);
            resetTxFlag(robotAddress);              // reset the flag indicating the transmission is completed
            while(messageIsSent(robotAddress)==0);  // wait for the message to be sent to the robot

            robLSpeed = -20;
            robRSpeed = -20;
            setLeftSpeed(robotAddress, robLSpeed);
            setRightSpeed(robotAddress, robRSpeed);
            resetTxFlag(robotAddress);
            while(messageIsSent(robotAddress)==0);

			#ifdef _WIN32
            Sleep(500);
			#endif
			#if defined(__linux__) || defined(__APPLE__)
			usleep(500000);
			#endif

            robLSpeed = 0;
            robRSpeed = 0;
            setLeftSpeed(robotAddress, robLSpeed);
            setRightSpeed(robotAddress, robRSpeed);
            resetTxFlag(robotAddress);
            while(messageIsSent(robotAddress)==0);

			switch(minGround) {

				case GROUND_RIGHT:	// turn left
                    robLSpeed = -20;
                    robRSpeed = 20;
                    setLeftSpeed(robotAddress, robLSpeed);
                    setRightSpeed(robotAddress, robRSpeed);
                    resetTxFlag(robotAddress);
                    while(messageIsSent(robotAddress)==0);
					prevRot = LEFT_ROT;
					#ifdef _WIN32
            		Sleep(250);
					#endif
					#if defined(__linux__) || defined(__APPLE__)
					usleep(250000);
					#endif
					break;

				case GROUND_CENTER_RIGHT:	// center sensor, continue same rotation
				case GROUND_CENTER_LEFT:
					if(prevRot == LEFT_ROT) {
                        robLSpeed = -20;
                        robRSpeed = 20;
                        setLeftSpeed(robotAddress, robLSpeed);
                        setRightSpeed(robotAddress, robRSpeed);
                        resetTxFlag(robotAddress);
                        while(messageIsSent(robotAddress)==0);
						#ifdef _WIN32
            			Sleep(250);
						#endif
						#if defined(__linux__) || defined(__APPLE__)
						usleep(250000);
						#endif
					} else if(prevRot == RIGHT_ROT) {
                        robRSpeed = -20;
                        robLSpeed = 20;
                        setLeftSpeed(robotAddress, robLSpeed);
                        setRightSpeed(robotAddress, robRSpeed);
                        resetTxFlag(robotAddress);
                        while(messageIsSent(robotAddress)==0);
						#ifdef _WIN32
            			Sleep(250);
						#endif
						#if defined(__linux__) || defined(__APPLE__)
						usleep(250000);
						#endif
					} else {	// turn right by default
                        robRSpeed = -20;
                        robLSpeed = 20;
                        setLeftSpeed(robotAddress, robLSpeed);
                        setRightSpeed(robotAddress, robRSpeed);
                        resetTxFlag(robotAddress);
                        while(messageIsSent(robotAddress)==0);
						#ifdef _WIN32
            			Sleep(250);
						#endif
						#if defined(__linux__) || defined(__APPLE__)
						usleep(250000);
						#endif
						prevRot = RIGHT_ROT;
					}
					break;

				case GROUND_LEFT: // turn right
                    robRSpeed = -20;
                    robLSpeed = 20;
                    setLeftSpeed(robotAddress, robLSpeed);
                    setRightSpeed(robotAddress, robRSpeed);
                    resetTxFlag(robotAddress);
                    while(messageIsSent(robotAddress)==0);
					#ifdef _WIN32
            		Sleep(250);
					#endif
					#if defined(__linux__) || defined(__APPLE__)
					usleep(250000);
					#endif
					break;

			}

			robLSpeed = 30;
			robRSpeed = 30;
            setLeftSpeed(robotAddress, robLSpeed);
            setRightSpeed(robotAddress, robRSpeed);
            resetTxFlag(robotAddress);
            while(messageIsSent(robotAddress)==0);

        }

    }

	return 0;

}

