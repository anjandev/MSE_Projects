#pragma config(Sensor, S1,     distance,       sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          leftDrive,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorD,          rightDrive,    tmotorEV3_Large, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define PRESENT 1
float const DIST_BETWEEN_BOT_AND_WALL = 7.6;
// Define directions using numbers
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

typedef struct{
	int NWall;
	int SWall;
	int EWall;
	int WWall;
	int Visited;
	int entryDir;
}cell;

// ZERO INDEXED
int const START_ROW = 1;
int const START_COL = 0;
int const END_ROW = 2;
int const END_COL = 2;

int currentRow = START_ROW;
int currentCol = START_COL;

int const MILI_TO_BEEP_FOR = 200;
int const FREQUENCY = 300;

// CHANGE THESE IF ANYTHING MECHANICAL. MAKE SURE YOU TEST THESE
int const SPEED_ON_MOTOR_DIF = 10;
float const UNCERTAINTY_STRAIGHT = 20;
float const UNCERTAINTY_ROT = 27;

// Movement Variabels defined
float const ONE_ROTATION = 360 + UNCERTAINTY_STRAIGHT;
float const QUARTER_ROTATION = 180 + UNCERTAINTY_ROT;


float const SIZE_OF_ONE_CELL = 22.5425; //cm
float const DRIVE_GEAR_RATIO = 5;
float const DIAMETER_OF_WHEEL = 5.5; // cm
float const CIRCUMFERENCE_OF_WHEEL = PI * DIAMETER_OF_WHEEL;

// Speed Variable
int const FORWARD = -100;
int const BACKWARD = -FORWARD;

// MAZE VARIABLES
int const MAZE_WIDTH = 4;
int const MAZE_HEIGHT = 6;
int const LAST_MAZE_HEIGHT_INDEX = MAZE_HEIGHT - 1;
int const LAST_MAZE_WIDTH_INDEX = MAZE_WIDTH - 1;
cell Maze[MAZE_HEIGHT][MAZE_WIDTH];

int entered[MAZE_WIDTH*MAZE_HEIGHT];
int lastEnteredIdx = 0;

// MISC Constants
int const MILLISECS_TO_DRIVE_INTO_WALL = 2000;

// Call functions
void goFwdCell(int direction);
int Turn90CW(int direction);
int Turn90CCW(int direction);
int MovementWithSensor(int direction);
void reverseDirection();
void deleteDuplicates();
int goingBackFastestRoute(int direction);


void drawInfo(int direction, int going){
	//displayCenteredTextLine(3, "NorthWall: %d", Maze[currentRow][currentCol].NWall);
	//displayCenteredTextLine(4, "EastWall: %d", Maze[currentRow][currentCol].EWall);
	//displayCenteredTextLine(5, "SouthWall: %d", Maze[currentRow][currentCol].SWall);
	//displayCenteredTextLine(6, "WestWall: %d", Maze[currentRow][currentCol].WWall);
	//displayCenteredTextLine(7, "row: %d", currentRow);
	//displayCenteredTextLine(8, "col: %d", currentCol);
	//displayCenteredTextLine(9, "Direction: %d", direction);
	displayCenteredTextLine(8, "going to: %d", going);
	displayCenteredTextLine(9, "Direction: %d", direction);
}

task main(){
	for (int c = 0; c < MAZE_WIDTH; c++){
		for (int r = 0; r < MAZE_HEIGHT; r++){
			Maze[r][c].Visited = false;
			Maze[r][c].NWall = PRESENT;
			Maze[r][c].SWall = PRESENT;
			Maze[r][c].EWall = PRESENT;
			Maze[r][c].WWall = PRESENT;
		}
	}

	// Assigning walls [row][col]
	for (int c = 0; c < MAZE_WIDTH; c++){
		Maze[0][c].SWall = PRESENT;
		Maze[LAST_MAZE_HEIGHT_INDEX][c].NWall = PRESENT;
	}

	for (int r = 0; r < MAZE_HEIGHT; r++){
		Maze[r][0].WWall = PRESENT;
		Maze[r][LAST_MAZE_WIDTH_INDEX].EWall = PRESENT;

	}

	int direction = NORTH;
	Maze[currentRow][currentCol].entryDir = direction;
	Maze[currentRow][currentCol].Visited = true;

	while(currentRow != END_ROW || currentCol != END_COL){
		direction = MovementWithSensor(direction);
		entered[lastEnteredIdx] = direction;
		lastEnteredIdx++;
	}

	playTone(FREQUENCY, MILI_TO_BEEP_FOR);
	
	
	deleteDuplicates();
	
	sleep(MILI_TO_BEEP_FOR * 10);
	//drawInfo(direction, 0);
	
	reverseDirection();	
	direction = goingBackFastestRoute(direction);
}



void deleteDuplicates(){
	int idx = -1;
	

	while(idx < lastEnteredIdx){
		idx++;

		if(abs(entered[idx] - entered[idx + 1]) == 2){
			for(int moveOGTo = idx; moveOGTo <= lastEnteredIdx - 2; moveOGTo++){
				entered[moveOGTo] = entered[moveOGTo + 2];
			}

			lastEnteredIdx = lastEnteredIdx - 2;
			idx = -1;
		}

	}
}

void reverseDirection(){
	for(int idx = 0; idx <= lastEnteredIdx; idx++){
		if(entered[idx]==EAST){
			entered[idx] = WEST;
		}
		else if(entered[idx]==SOUTH){
			entered[idx] = NORTH;
		}
		else if(entered[idx]==WEST){
			entered[idx] = EAST;
		}
		else if(entered[idx]==NORTH){
			entered[idx] = SOUTH;
		}
	}
}

int goingBackFastestRoute(int direction){
	for(int idx = lastEnteredIdx; idx >= 0; idx--){
		displayCenteredTextLine(idx, "%d", entered[idx]);
		displayCenteredTextLine(7, "%d", direction);
	}
	
	sleep(5000);
	eraseDisplay();
	
	for(int idx = lastEnteredIdx; idx > 0; idx--){
		// TODO: MAKE IT FAST. TURN WITH FASTEST SOLUTION
		int temp = entered[idx];
		while(direction != temp){
			//drawInfo(direction, entered[idx]);
			//sleep(1000);
			direction = Turn90CW(direction);
			//drawInfo(direction);
		}
		displayCenteredTextLine(3, "%d", entered[idx]);
		displayCenteredTextLine(5, "%d", direction);
		displayCenteredTextLine(7, "%d", idx);
		//drawInfo(direction, entered[idx]);
		sleep(3000);
		goFwdCell(direction);
	}
	
	return direction;
}

void goFwdCell(int direction){
	moveMotorTarget(leftDrive, (SIZE_OF_ONE_CELL / CIRCUMFERENCE_OF_WHEEL)*DRIVE_GEAR_RATIO * ONE_ROTATION, FORWARD);
	moveMotorTarget(rightDrive, (SIZE_OF_ONE_CELL / CIRCUMFERENCE_OF_WHEEL)*DRIVE_GEAR_RATIO * ONE_ROTATION, FORWARD + SPEED_ON_MOTOR_DIF);
	repeatUntil(!getMotorRunning(leftDrive) && !getMotorRunning(rightDrive)){

	}

	if (direction == NORTH){
		Maze[currentRow][currentCol].NWall = false;
		currentRow++;
		Maze[currentRow][currentCol].SWall = false;
	}
	else if (direction == SOUTH){
		Maze[currentRow][currentCol].SWall = false;
		currentRow--;
		Maze[currentRow][currentCol].NWall = false;
	}
	else if (direction == EAST){
		Maze[currentRow][currentCol].EWall = false;
		currentCol++;
		Maze[currentRow][currentCol].WWall = false;
	}
	else if (direction == WEST){
		Maze[currentRow][currentCol].WWall = false;
		currentCol--;
		Maze[currentRow][currentCol].EWall = false;
	}

	Maze[currentRow][currentCol].entryDir = direction;
	Maze[currentRow][currentCol].Visited = true;

}

int Turn90CCW(int direction){
	moveMotorTarget(leftDrive, QUARTER_ROTATION * DRIVE_GEAR_RATIO, BACKWARD);
	moveMotorTarget(rightDrive, QUARTER_ROTATION * DRIVE_GEAR_RATIO, FORWARD);
	repeatUntil(!getMotorRunning(leftDrive) && !getMotorRunning(rightDrive)){

	}
	if(direction == 0){
		direction = 3;
	}
	else{
		direction--;
	}

	return direction;

}

int Turn90CW(int direction){
	moveMotorTarget(leftDrive, QUARTER_ROTATION * DRIVE_GEAR_RATIO, FORWARD);
	moveMotorTarget(rightDrive, QUARTER_ROTATION * DRIVE_GEAR_RATIO, BACKWARD);

	repeatUntil(!getMotorRunning(leftDrive) && !getMotorRunning(rightDrive)){

	}

	if(direction < 3){
		direction++;
	}
	else{
		direction = NORTH;
	}


	return direction;
}

int thereIsWall(){
	if(getUSDistance(distance)<=DIST_BETWEEN_BOT_AND_WALL || getUSDistance(distance)==255){
		return 1;
	}

	return 0;
}

// Checking order, North(0), East(1), West(3) then South(2)
int MovementWithSensor(int direction){

	int enteringDirectionWall = thereIsWall();

	// turn to check if wall is right
	direction = Turn90CW(direction);

	if(!thereIsWall()){
		goFwdCell(direction);
		return direction;
	}

	if(thereIsWall() && !enteringDirectionWall){
		direction = Turn90CCW(direction);
		goFwdCell(direction);
		return direction;
	}

	direction = Turn90CW(direction);

	direction = Turn90CW(direction);

	if(!thereIsWall()){
		goFwdCell(direction);
		return direction;
	}

	direction =	Turn90CCW(direction);

	goFwdCell(direction);

	return direction;
}