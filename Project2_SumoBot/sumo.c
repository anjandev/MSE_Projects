#pragma config(Sensor, S1,     eyes,           sensorEV3_Ultrasonic)
#pragma config(Sensor, S2,     light,          sensorEV3_Color, modeEV3Color_RGB_Raw)
#pragma config(Sensor, S3,     touch,          sensorEV3_Touch)
#pragma config(Sensor, S4,     touch2,         sensorEV3_Touch)
#pragma config(Motor,  motorA,          rightDrive,    tmotorEV3_Large, PIDControl, driveRight, encoder)
#pragma config(Motor,  motorB,          leftDrive,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// TODO REMEMBER TO ADD TOUCH
#define WHITE_THRESH	100
#define ROTATION 360
#define BACKWARDS 100
// Forwards means towards where eyes are seeing
#define FORWARDS -100
#define WHITE -1
#define BLACK 1

int const DIST_TO_CHARGE_CM = 30;

#define SEC_TO_MILLI 1000
#define SEC_TOGO_STRAIGHT 3
// constants to define whether the back of the bot is on the stage or off
#define BACKOFF 0
#define BACKON 1

int const FRAC_TO_ROT_360 = 17;
int const FRAC_TO_ROT_90 = FRAC_TO_ROT_360 / 4;
int const SEC_TO_TRY_GOING_FROM_LINE = 2;
int const BEGIN_DEG_BACK_LINE = 1000;

int TIME_TIL_ROT_CHECK = 3;

int botInFront(){
    if(getUSDistance(eyes) <= DIST_TO_CHARGE_CM){
        return 1;
    }
    else{
        return 0;
    }
}

int backOff(){
    if(SensorValue[touch] == BACKOFF || SensorValue[touch2] == BACKOFF){
        return 1;
    }
    else{
        return 0;
    }
}


int checkWhite(){
	if(SensorValue[light] > WHITE_THRESH){
		return -1;
	}
	else{
		// 1 means light is detecting black
		return 1;
	}
}

void rotateLeftOutside(float fraction){
  //motor[leftDrive] = 0;
  //motor[rightDrive] = 0;
	//inside pivot is right
	moveMotorTarget(rightDrive, ROTATION * fraction, BACKWARDS);
	moveMotorTarget(leftDrive, ROTATION * fraction, FORWARDS);

	//repeatUntil(checkWhite() != WHITE && !backOff() && getMotorMoving(leftDrive) == 0){
	repeatUntil(getMotorMoving(leftDrive) == 0 || backOff() || checkWhite() == WHITE || botInFront()){
		displayCenteredTextLine(4, "rotateLeftOutside");
	}
}

void rotateRightOutside(float fraction){
  //motor[leftDrive] = 0;
 // motor[rightDrive] = 0;
	// inside pivot is left
	moveMotorTarget(rightDrive, ROTATION * fraction, FORWARDS);
	moveMotorTarget(leftDrive, ROTATION * fraction, BACKWARDS);

	// 	repeatUntil(checkWhite() != WHITE && !backOff() && getMotorMoving(leftDrive) == 0){
	repeatUntil(getMotorMoving(leftDrive) == 0 || backOff() || checkWhite() == WHITE || botInFront()){
		displayCenteredTextLine(4, "rotateRightOutside");
	}
}

void getOffWhite(){
    if(checkWhite() == WHITE){
    	displayCenteredTextLine(4, "GET OFF WHITE");
        clearTimer(T1);
        motor[leftDrive] = BACKWARDS;
        motor[rightDrive] = BACKWARDS;
        delay(SEC_TO_TRY_GOING_FROM_LINE * SEC_TO_MILLI);
        rotateLeftOutside(FRAC_TO_ROT_90);
        motor[leftDrive] = FORWARDS;
        motor[rightDrive] = FORWARDS;

    }
}

void getBackOn(){
    if(backOff()){
    	displayCenteredTextLine(4, "GET BACK ON");
        clearTimer(T1);
        motor[leftDrive] = FORWARDS;
        motor[rightDrive] = FORWARDS;
        delay(SEC_TO_TRY_GOING_FROM_LINE * SEC_TO_MILLI);
        rotateRightOutside(FRAC_TO_ROT_90);
        motor[leftDrive] = FORWARDS;
        motor[rightDrive] = FORWARDS;

    }
}




void beginningTurn(){

	int const HALF_ROT_FRAC = FRAC_TO_ROT_360 / 2;
	displayCenteredTextLine(4, "Beggining turn");
	moveMotorTarget(leftDrive, BEGIN_DEG_BACK_LINE, BACKWARDS);
	moveMotorTarget(rightDrive, BEGIN_DEG_BACK_LINE, BACKWARDS);

	waitUntilMotorStop(leftDrive);

	moveMotorTarget(rightDrive, ROTATION * HALF_ROT_FRAC, BACKWARDS);
	moveMotorTarget(leftDrive, ROTATION * HALF_ROT_FRAC, FORWARDS);

	repeatUntil(getMotorMoving(leftDrive) == 0 || backOff() || checkWhite() == WHITE){

	}
}

//left go forward
// right go back



void charge(){

    while(botInFront() && checkWhite() != WHITE && !backOff()){
    	displayCenteredTextLine(4, "CHARGED");
        motor[leftDrive] = FORWARDS;
        motor[rightDrive] = FORWARDS;
    }
}

task main()
{
	int firstTurn = 1;

	while(firstTurn){
		motor[leftDrive] = FORWARDS;
		motor[rightDrive] = FORWARDS;

		if(checkWhite() == WHITE){
			beginningTurn();
			firstTurn = 0;
		}
	}



	motor[leftDrive] = 0;
	motor[rightDrive] = 0;
	clearTimer(T1);

	while(1){

        motor[leftDrive] = FORWARDS;
        motor[rightDrive] = FORWARDS;

        getOffWhite();
        getBackOn();

        charge();

        getOffWhite();
        getBackOn();

        if(time1[T1] > TIME_TIL_ROT_CHECK * SEC_TO_MILLI){
            clearTimer(T1);
            displayCenteredTextLine(4, "TIMING");

            int const TIME_TO_ROT_FOR = 3;

            while(!botInFront() && checkWhite() != WHITE && !backOff() && time1[T1] < TIME_TO_ROT_FOR * SEC_TO_MILLI){
                motor[leftDrive] = BACKWARDS;
                motor[rightDrive] = FORWARDS;
            }
            clearTimer(T1);
        }

        getOffWhite();
        getBackOn();

        charge();
	}

}
