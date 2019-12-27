//
//  main.c
//  Final Project CSC412
//
//  Created by Jean-Yves Hervé on 2019-12-12
//	This is public domain code.  By all means appropriate it and change is to your
//	heart's content.
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
//
#include "gl_frontEnd.h"

using namespace std;

//==================================================================================
//	Function prototypes
//==================================================================================
void displayGridPane(void);
void displayStatePane(void);
void initializeApplication(void);
void cleanupGridAndLists(void);

/**
 * Function to write the initial ouput
*/
void writeInitialPositions(void);

/*
 * Function to write commands the robot has executed
 * @param bot information on robot box pair
*/
void writeRobotexe(roboxInfo* bot);

/**
 * Function to add all the robot, box, and door on the grid
 * in random spots
*/
void addrobotboxdoor(void);

/**
 * Function to get the information on door
 * includes getting random cordinates and then adding it to global vector
 * @param i -the index of the door or door number
*/
void getDoorInfo(int i);

/**
 * Function to get all information on robot and box pairing
 * includes random coordinates for both, the door it is assigned to, etc
 * @param i -the index of the robot+number or the robot+box number
*/
void getRoboxInfo(int i);

/**
 * Function to check if the location of the coordinate 
 * is okay to use (when no box, door, or robot occupy the same space)
 * @param row the y coordinate
 * @param col the x coordinate
*/
bool locationOK(int row, int col);

/**
 * The function to operate each individual robot
 * @param bot The struct that holds all the information on robot and box
*/
void robotfunc(roboxInfo* bot);

/**
 * The function to move the robot, the move command
 * Use this to move the robot to the correct spot
 * @param bot The struct that holds info on robot and box
*/
void movecommand(roboxInfo* bot);

/*
 * Function to turn the robot around so that it does not go
 * over its designated box, this is part of the move command
 * @param bot struct to hold info on robot and box
 * @param desrow the designated row that the bot needs to move to
 * @param descol the designated col
*/
void turnbot(roboxInfo* bot,int desrow, int descol);

/**
 * The function to let the robot push the box, the push command
 * @param bot struct to hold info on robot and box
*/
void pushcommand(roboxInfo* bot);

//==================================================================================
//	Application-level global variables
//==================================================================================

//	Don't touch
extern int	GRID_PANE, STATE_PANE;
extern int 	GRID_PANE_WIDTH, GRID_PANE_HEIGHT;
extern int	gMainWindow, gSubwindow[2];

//	Don't rename any of these variables
//-------------------------------------
//	The state grid and its dimensions (arguments to the program)
int** grid;
int numRows = -1;	//	height of the grid
int numCols = -1;	//	width
int numBoxes = -1;	//	also the number of robots
int numDoors = -1;	//	The number of doors.

FILE* outText = fopen("robotSimulOut.txt","w"); //open the ouput text file to write in

//requires the coordinates of robot,box, and door to be global
//so can displayGridPane
vector<roboxInfo> robox; //robot + box pair information
vector<doorlocation> door; // information on door

char dirchar[] = {'N','W','S','E'}; // represents the character representing direction

int numLiveThreads = 0;		//	the number of live robot threads

//	robot sleep time between moves (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int robotSleepTime = 100000;

//	An array of C-string where you can store things you want displayed
//	in the state pane to display (for debugging purposes?)
//	Dont change the dimensions as this may break the front end
const int MAX_NUM_MESSAGES = 8;
const int MAX_LENGTH_MESSAGE = 32;
char** message;
time_t startTime;

//==================================================================================
//	These are the functions that tie the simulation with the rendering.
//	Some parts are "don't touch."  Other parts need your intervention
//	to make sure that access to critical section is properly synchronized
//==================================================================================


void displayGridPane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[GRID_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, GRID_PANE_HEIGHT, 0);
	glScalef(1.f, -1.f, 1.f);
	
	//-----------------------------
	//	CHANGE THIS
	//-----------------------------
	//	Here I hard-code myself some data for robots and doors.  Obviously this code
	//	this code must go away.  I just want to show you how to display the information
	//	about a robot-box pair or a door.
	//	Important here:  I don't think of the locations (robot/box/door) as x and y, but
	//	as row and column.  So, the first index is a row (y) coordinate, and the second
	//	index is a column (x) coordinate.

	for (int i=0; i<numBoxes; i++)
	{
		//	here I would test if the robot thread is still live
		//					row				column			row			column
		if(robox[i].stat!=ENDCOM){
			drawRobotAndBox(i, robox[i].rrow, robox[i].rcol, robox[i].brow, robox[i].bcol, robox[i].desDoor);
		}
	}

	for (int i=0; i<numDoors; i++)
	{
		//	here I would test if the robot thread is still alive
		//				row				column	
		drawDoor(i, door[i].row, door[i].col);
	}

	//	This call does nothing important. It only draws lines
	//	There is nothing to synchronize here
	drawGrid();

	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

void displayStatePane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[STATE_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	Here I hard-code a few messages that I want to see displayed
	//	in my state pane.  The number of live robot threads will
	//	always get displayed.  No need to pass a message about it.
	time_t currentTime = time(NULL);
	double deltaT = difftime(currentTime, startTime);

	int numMessages = 3;
	sprintf(message[0], "We have %d doors", numDoors);
	sprintf(message[1], "I like cheese");
	sprintf(message[2], "Run time is %4.0f", deltaT);

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	drawState(numMessages, message);
	
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupRobots(void)
{
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * robotSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		robotSleepTime = newSleepTime;
	}
}

void slowdownRobots(void)
{
	//	increase sleep time by 20%
	robotSleepTime = (12 * robotSleepTime) / 10;
}




//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function besides
//	the initialization of numRows, numCos, numDoors, numBoxes.
//------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//	We know that the arguments  of the program  are going
	//	to be the width (number of columns) and height (number of rows) of the
	//	grid, the number of boxes (and robots), and the number of doors.
	//	You are going to have to extract these.  For the time being,
	//	I hard code-some values
	numRows = atoi(argv[1]);
	numCols = atoi(argv[2]);
	numBoxes = atoi(argv[3]);
	numDoors = atoi(argv[4]);
	numLiveThreads = numBoxes;

	//	Even though we extracted the relevant information from the argument
	//	list, I still need to pass argc and argv to the front-end init
	//	function because that function passes them to glutInit, the required call
	//	to the initialization of the glut library.
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane);
	
	//	Now we can do application-level initialization
	initializeApplication();

	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	
	cleanupGridAndLists();
	
	//	This will probably never be executed (the exit point will be in one of the
	//	call back functions).
	return 0;
}

//---------------------------------------------------------------------
//	Free allocated resource before leaving (not absolutely needed, but
//	just nicer.  Also, if you crash there, you know something is wrong
//	in your code.
//---------------------------------------------------------------------
void cleanupGridAndLists(void)
{
	for (int i=0; i< numRows; i++)
		free(grid[i]);
	free(grid);
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		free(message[k]);
	free(message);
}


//==================================================================================
//
//	This is a part that you have to edit and add to.
//
//==================================================================================


void initializeApplication(void)
{
	//	Allocate the grid
	grid = (int**) malloc(numRows * sizeof(int*));
	for (int i=0; i<numRows; i++)
		grid[i] = (int*) malloc(numCols * sizeof(int));
	
	message = (char**) malloc(MAX_NUM_MESSAGES*sizeof(char*));
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		message[k] = (char*) malloc((MAX_LENGTH_MESSAGE+1)*sizeof(char));
		
	//---------------------------------------------------------------
	//	All the code below to be replaced/removed
	//	I initialize the grid's pixels to have something to look at
	//---------------------------------------------------------------
	//	Yes, I am using the C random generator after ranting in class that the C random
	//	generator was junk.  Here I am not using it to produce "serious" data (as in a
	//	simulation), only some color, in meant-to-be-thrown-away code
	
	//	seed the pseudo-random generator
	startTime = time(NULL);
	srand((unsigned int) startTime);

	addrobotboxdoor();
	writeInitialPositions();
	// cout<<numBoxes<<endl;
	for (int i = 0;i<numBoxes;i++){
		robotfunc(&robox[i]);
	}
	fclose(outText);
	

	//	normally, here I would initialize the location of my doors, boxes,
	//	and robots, and create threads (not necessarily in that order).
	//	For the handout I have nothing to do.
}

void writeInitialPositions(void){
	fprintf(outText,"Width: %d, Height: %d, # Boxes: %d, # Doors: %d\n",numCols,numRows,numBoxes,numDoors);
	fprintf(outText,"\n");

	int roboxsize=robox.size();
	int doorsize=door.size();

	for(int i=0;i<doorsize;i++){
		fprintf(outText,"Door %d: row %d col %d\n",i,door[i].row,door[i].col);
	}
	fprintf(outText,"\n");

	for(int i=0;i<roboxsize;i++){
		fprintf(outText,"Box %d: row %d col %d\n",i,robox[i].brow,robox[i].bcol);
	}
	fprintf(outText,"\n");

	for(int i=0;i<roboxsize;i++){
		fprintf(outText,"Robot %d: row %d col %d\n",i,robox[i].rrow,robox[i].rcol);
	}
	fprintf(outText,"\n");
}

void writeRobotexe(roboxInfo* bot){
	// check the current status of the bot
	// depending on status different types of print.
	switch(bot->stat){
		case MOVECOM:
				fprintf(outText,"robot %d move %c\n",bot->index,dirchar[bot->dir]);
				break;
		case PUSHCOM:
				fprintf(outText,"robot %d push %c\n",bot->index,dirchar[bot->dir]);
				break;
		default:
				fprintf(outText,"robot %d end\n",bot->index);
				break;
	}
}


void addrobotboxdoor(void){

	// create all the door
	for (int i=0;i<numDoors;i++){
		getDoorInfo(i);
	}

	//create all the robot and boxes
	for (int i = 0; i<numBoxes;i++){
		getRoboxInfo(i);
	}
}

void getRoboxInfo(int i){
	// coordinates for box, robot
	// and also the assigned door to the robot box pair
	// rowmoves and colmoves is going to be the amount of
	// moves necessary from the box location to the designated door
	int boxrow,boxcol,robotrow,robotcol,randomdoor,rowmoves,colmoves;

	// loop until gets a random coordinates that works
	while(true){

		// box cannot be placed on the edge
		// so this is getting the range from 1 to numRows/numCols
		boxrow = (rand()%(numRows-2)) + 1;
		boxcol = (rand()%(numCols-2)) + 1;

		// check if the coordinates is okay
		if(locationOK(boxrow,boxcol)){
			break;
		}
	}

	// same as the box except the robot can be placed anywhere on the grid
	while(true){
		robotrow = (rand()%numRows);
		robotcol = (rand()%numCols);

		if(locationOK(robotrow,robotcol)){
			break;
		}
	}

	// get the random door number
	randomdoor = (rand()%numDoors);

	// acquire rowmoves and colmoves by
	// subtracting box coordinates to the assigned door coordinate
	rowmoves = boxrow-door[randomdoor].row;
	colmoves = boxcol-door[randomdoor].col;

	// push all information aqcuire to the global vector
	robox.push_back({boxrow,boxcol,robotrow,robotcol,randomdoor,rowmoves,colmoves,NORTH,MOVECOM,i});

}


void getDoorInfo(int i){
	// the coordinate variables for door
	int rrow,rcol;

	// get a random door coordinate
	while (true){
		rrow = (rand()%numRows);
		rcol = (rand()%numCols);
		
		// check if location is ok
		if (locationOK(rrow,rcol)){
			break;
		}

	}
	doorlocation loc = {rrow,rcol,i};
	door.push_back(loc);

}

bool locationOK(int row, int col){
	// get the current length of the vector of robot box info and door vector
	int roboxsize = robox.size();
	int dosize = door.size();

	// check the vector to make sure row and col does not
	// equal to the coordinate of robot, box, or door
	for(int i=0; i<roboxsize;i++){
		if(row==robox[i].brow && col==robox[i].bcol){
			return false;
		}
		else if(row==robox[i].rrow && col==robox[i].rcol){
			return false;
		}
	}
	for (int i=0;i<dosize;i++){
		if(row==door[i].row && col==door[i].col){
			return false;
		}
	}

	return true;
}

void robotfunc(roboxInfo* bot){
	// create an vector of movelist
	vector<Direction> movelist;

	// check the rowmove eand colmove and 
	// add the direction to movelist depending 
	// if the value of rowmove and colmove is 
	// positive or negative
	if (bot->rowmove > 0)
		movelist.push_back(NORTH);
	else if (bot->rowmove < 0)
		movelist.push_back(SOUTH);
	if (bot->colmove < 0)
		movelist.push_back(EAST);
	else if (bot->colmove > 0)
		movelist.push_back(WEST);


	int movelistsize = movelist.size();
	// loop through the movelist vector, handling
	// one push direction at a time
	for(int i=0;i<movelistsize;i++){
		// change the current status of robot to
		// move command and direction to the current direction looking at in the movelist
		// then use the movecommand to move robot to correct position
		bot->stat=MOVECOM;
		bot->dir=movelist[i];
		movecommand(bot);

		// change status to push and use push command
		bot->stat=PUSHCOM;
		bot->dir=movelist[i];
		pushcommand(bot);
	}
	// subtract the numLiveThreads and 
	// change status of robot to ENDCOM 
	bot->stat=ENDCOM;
	writeRobotexe(bot);
	numLiveThreads--;
}
void movecommand(roboxInfo* bot){

	// get initial position of the box
	int desrow = bot->brow, descol=bot->bcol;

	//robot location based on the direction that the box needs to be pushed
	// i.e. if box needs to push NORTH then robot needs to pos. SOUTH of the box
	switch(bot->dir){
		case NORTH: desrow+=1;
					break;
		case SOUTH: desrow-=1;
					break;
		case EAST:	descol-=1;
					break;
		default:	descol+=1;
					break;
	}

	// set initial direction robot needs to go
	// to reach desination
	if (bot->rrow > desrow){
		bot->dir = NORTH;
	} else if (bot->rrow < desrow){
		bot->dir = SOUTH;
	} else if (bot->rcol < descol){
		bot->dir = EAST;
	} else{
		bot->dir = WEST;
	}

	// loop that does all the movement of the robot
	while (true){
		// when robot reaches destination break the loop
		if (bot->rrow==desrow && bot->rcol==descol){
			break;
		}

		switch(bot->dir){
			case NORTH:
					// means the robot's designated box is in the direction need to move
					if (bot->rrow-1==bot->brow && bot->rcol==bot->bcol){
						turnbot(bot,desrow,descol);
					}
					usleep(100000);
					// set position again
					bot->dir=NORTH;
					//move robot
					bot->rrow-=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();
					// check after moving the robot
					// does it need to change direction
					if (bot->rrow==desrow){
						if (bot->rcol < descol){
							bot->dir = EAST;
						} else{
							bot->dir =WEST;
						}
					}
					break;

			case SOUTH:
					if (bot->rrow+1==bot->brow && bot->rcol==bot->bcol){
						turnbot(bot,desrow,descol);
					}
					usleep(100000);
					bot->dir = SOUTH;
					bot->rrow+=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();
					if(bot->rrow==desrow){
						if (bot->rcol < descol){
							bot->dir = EAST;
						} else {
							bot->dir = WEST;
						}
					}
					break;
			case EAST:
					if (bot->rrow==bot->brow && bot->rcol==bot->bcol){
						turnbot(bot,desrow,descol);
					}
					usleep(100000);
					bot->dir = EAST;
					bot->rcol+=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();
					if(bot->rcol==descol){
						if (bot->rrow < desrow){
							bot->dir=SOUTH;
						} else{
							bot->dir=NORTH;
						}
					}
					break;
			default:
					if (bot->rrow==bot->brow && bot->rcol==bot->bcol){
						turnbot(bot,desrow,descol);
					}
					usleep(100000);
					bot->dir = WEST;
					bot->rcol-=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();
					if(bot->rcol==descol){
						if(bot->rrow < desrow){
							bot->dir=SOUTH;
						} else{
							bot->dir=NORTH;
						}
					}
		}
	}

}
void turnbot(roboxInfo* bot,int desrow, int descol){
	// check if moving horizontally or vertically
	// if vertical the turn needs to be horizontal
	// and vice versa
	if (bot->dir==NORTH || bot->dir==SOUTH){
		if(bot->rcol < descol){
			usleep(100000);
			bot->dir = EAST;
			bot->rcol+=1;
			writeRobotexe(bot);
			displayGridPane();
			displayStatePane();
		} else {
			usleep(100000);
			bot->dir = WEST;
			bot->rcol-=1;
			writeRobotexe(bot);
			displayGridPane();
			displayStatePane();
		}
	} else if (bot->dir==WEST || bot->dir==EAST){
		if(bot->rrow > desrow){
			usleep(100000);
			bot->dir = NORTH;
			bot->rrow-=1;
			writeRobotexe(bot);
			displayGridPane();
			displayStatePane();
		} else {
			usleep(100000);
			bot->dir=SOUTH;
			bot->rrow+=1;
			writeRobotexe(bot);
			displayGridPane();
			displayStatePane();
		}
	}
}
void pushcommand(roboxInfo* bot){

	//check the current direction box needs to push
	switch(bot->dir){
		case NORTH:
				// while loop that will push the box
				while(bot->rowmove != 0){
					usleep(100000);
					bot->rrow-=1;
					bot->brow-=1;
					bot->rowmove-=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();

				}
				break;
		case SOUTH:
				while(bot->rowmove != 0){
					usleep(100000);
					bot->rrow+=1;
					bot->brow+=1;
					bot->rowmove+=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();
				}
				break;
		case EAST:
				while(bot->colmove != 0){
					usleep(100000);
					bot->rcol+=1;
					bot->bcol+=1;
					bot->colmove+=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();
				}
				break;
		default:
				while(bot->colmove != 0){
					usleep(100000);
					bot->rcol-=1;
					bot->bcol-=1;
					bot->colmove-=1;
					writeRobotexe(bot);
					displayGridPane();
					displayStatePane();
				}
				break;
	}
}