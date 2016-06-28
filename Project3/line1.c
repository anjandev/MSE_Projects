#pragma config(Sensor, S1,     light,          sensorEV3_Color)
#pragma config(Motor,  motorA,          innerMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          outerMotor,    tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

float const L1 = 6.5; // everything in cm
float const L2 = 13;


#define FORWARD 60

float sqd(float num2Square){
	return num2Square *	num2Square;
}

float atan2d(float y, float x){
	// received function from online http://www.robotc.net/forums/viewtopic.php?p=6061	

  /*float phi;   //phi=radians;

  if (x>0) {phi=atan(y/x);}
  else if ((x<0)&&(y>=0))  {phi=PI+atan(y/x);}
  else if ((x<0)&&(y<0))   {phi=-PI+atan(y/x);}
  else if ((x==0)&&(y>0))  {phi=PI/2;}
  else if ((x==0)&&(y<0))  {phi=-PI/2;}
  else if ((x==0)&&(y==0)) {phi=0;}*/

	return radiansToDegrees(atan2(y, x));
}

float thetaMultiple(float theta){
	if(theta > 0){
		return 1;	
	}	
	else{
		return -1;	
	}
}


void moveToCoords (float x, float y){
	float YForTheta2_Nosqd = (sqd(x)+sqd(y)-sqd(L1)-sqd(L2))/(2*L1*L2);
	float YForTheta2 = sqrt(1-sqd(YForTheta2_Nosqd));
	float XForTheta2 = (sqd(x)+sqd(y)-sqd(L1)-sqd(L2))/(2*L1*L2);
	float theta2 = atan2d(YForTheta2, XForTheta2);
	
  float k1 = L2 * cosDegrees(theta2) + L1;
  float k2 = L2 * sinDegrees(theta2);
  float theta1 = atan2d(y,x) - atan2d(k2, k1);
  
  float const INNER_GEAR_RATIO_MULTIPLE = 40;
  float const OUTER_GEAR_RATIO_MULTIPLE = 5;
  
  moveMotorTarget(innerMotor, abs(theta1) * INNER_GEAR_RATIO_MULTIPLE, FORWARD * thetaMultiple(theta1));
  moveMotorTarget(outerMotor, abs(theta2) * OUTER_GEAR_RATIO_MULTIPLE, FORWARD * thetaMultiple(theta2));
	
  
}

task main()
{

	// move arm into intial position
	float const INITIAL_X = 14;
	float const INITIAL_Y = 10;
	
	moveToCoords (INITIAL_X, INITIAL_Y);
	repeatUntil(getMotorMoving(innerMotor) == 0 && getMotorMoving(outerMotor) == 0){
		
	}
	displayCenteredTextLine(4, "DONE");
		
	float const X_DECREMENT = 0.0001;
	float const X_MIN = -INITIAL_X;
	
	
	for(float x = INITIAL_X; x > X_MIN; x -= X_DECREMENT){
		moveToCoords (x, INITIAL_Y);
		repeatUntil(getMotorMoving(innerMotor) == 0 && getMotorMoving(outerMotor) == 0){
			// store data
		}
		displayCenteredTextLine(5, "%d", x);
	}
	
	
  
}