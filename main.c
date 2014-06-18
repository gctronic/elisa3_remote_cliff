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

int robotAddress[1];

unsigned char updateRGB(char *red, char *green, char *blue) {
    static unsigned int i=0;
    unsigned int rndNum;

    i = (i+1)%65000;  // use to change the rgb leds
    if(i==0) {
        rndNum = rand()%400;
        if(rndNum < 100) {
            *red = rand()%100;
            *green = rand()%100;
            *blue = 0;
        } else if(rndNum < 200) {
            *red = rand()%100;
            *green = 0;
            *blue = rand()%100;
        } else if(rndNum < 300) {
            *red = 0;
            *green = rand()%100;
            *blue = rand()%100;
        } else {
            *red = rand()%100;
            *green = rand()%100;
            *blue = rand()%100;
        }
        return 1;
    }
    return 0;
}

void stopAndComeBack() {

    setLeftSpeed(robotAddress[0], 0);
    setRightSpeed(robotAddress[0], 0);
    waitForUpdate(robotAddress[0], 100000); // wait for at most 100 ms

    setLeftSpeed(robotAddress[0], -20);
    setRightSpeed(robotAddress[0], -20);
    waitForUpdate(robotAddress[0], 100000); // wait for at most 100 ms
    #ifdef _WIN32
    Sleep(500);
	#endif
	#if defined(__linux__) || defined(__APPLE__)
	usleep(500000);
	#endif

    setLeftSpeed(robotAddress[0], 0);
    setRightSpeed(robotAddress[0], 0);
    waitForUpdate(robotAddress[0], 100000); // wait for at most 100 ms

}

void turnLeft() {
    setLeftSpeed(robotAddress[0], -20);
    setRightSpeed(robotAddress[0], 20);
    waitForUpdate(robotAddress[0], 100000); // wait for at most 100 ms
    #ifdef _WIN32
    Sleep(250);
	#endif
	#if defined(__linux__) || defined(__APPLE__)
	usleep(250000);
	#endif
}

void turnRight() {
    setLeftSpeed(robotAddress[0], 20);
    setRightSpeed(robotAddress[0], -20);
    waitForUpdate(robotAddress[0], 100000); // wait for at most 100 ms
	#ifdef _WIN32
    Sleep(250);
	#endif
    #if defined(__linux__) || defined(__APPLE__)
	usleep(250000);
	#endif
}

void goAhead() {
    setLeftSpeed(robotAddress[0], 30);
    setRightSpeed(robotAddress[0], 30);
    waitForUpdate(robotAddress[0], 100000); // wait for at most 100 ms
}

void avoidCliff(unsigned int *ground) {
    unsigned int minGroundValue = 0;
    unsigned int minGround = GROUND_LEFT;
    unsigned char prevRot = 0;

    // check for the ground with minimum value
    minGroundValue = ground[0];
	minGround = GROUND_LEFT;
	if(ground[1] < minGroundValue) {
        minGroundValue = ground[1];
		minGround = GROUND_CENTER_LEFT;
    }
	if(ground[2] < minGroundValue) {
		minGroundValue = ground[2];
		minGround = GROUND_CENTER_RIGHT;
	}
	if(ground[3] < minGroundValue) {
		minGroundValue = ground[3];
		minGround = GROUND_RIGHT;
	}

    // enable cliff avoidance only under a certain threshold
    if(minGroundValue <= CLIFF_THR) {

        stopAndComeBack();

        switch(minGround) {

            case GROUND_RIGHT:	// turn left
                turnLeft();
                prevRot = LEFT_ROT;
                break;

            case GROUND_CENTER_RIGHT:	// center sensor, continue same rotation
			case GROUND_CENTER_LEFT:
                if(prevRot == LEFT_ROT) {
                    turnLeft();
                } else if(prevRot == RIGHT_ROT) {
                    turnRight();
                } else {	// turn right by default
                    turnRight();
                    prevRot = RIGHT_ROT;
                }
                break;

            case GROUND_LEFT: // turn right
                turnRight();
                prevRot = RIGHT_ROT;
                break;

        }

        goAhead();

    }
}

int main(void) {

    // received from robot
    unsigned int robGround[8] = {0};
    // sent to robot
    char robRedLed=0, robGreenLed=0, robBlueLed=0;

    srand (time(NULL));

    printf("\r\nInsert the robot address: ");
    scanf("%d", &robotAddress[0]);

    // init the communication with the robots; set the addresses and number of the robots to control
    startCommunication(robotAddress, 1);

    while(1) {

        getAllGround(robotAddress[0], robGround);

        avoidCliff(robGround);

        if(updateRGB(&robRedLed, &robGreenLed, &robBlueLed)) {
            setRed(robotAddress[0], robRedLed);
            setGreen(robotAddress[0], robGreenLed);
            setBlue(robotAddress[0], robBlueLed);
        }

    }

	return 0;

}

