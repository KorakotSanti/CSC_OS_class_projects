//
//  main.cpp
//  GL threads
//
//  Created by Jean-Yves Hervé on 2017-04-24, revised 2019-11-19
//

 /*-------------------------------------------------------------------------+
 |	A graphic front end for a grid+state simulation.						|
 |																			|
 |	This application simply creates a glut window with a pane to display	|
 |	a colored grid and the other to display some state information.			|
 |	Sets up callback functions to handle menu, mouse and keyboard events.	|
 |	Normally, you shouldn't have to touch anything in this code, unless		|
 |	you want to change some of the things displayed, add menus, etc.		|
 |	Only mess with this after everything else works and making a backup		|
 |	copy of your project.  OpenGL & glut are tricky and it's really easy	|
 |	to break everything with a single line of code.							|
 |																			|
 |	Current GUI:															|
 |		- 'ESC' --> exit the application									|
 |		- 'r' --> add red ink												|
 |		- 'g' --> add green ink												|
 |		- 'b' --> add blue ink												|
 +-------------------------------------------------------------------------*/
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
//
#include "gl_frontEnd.h"

//==================================================================================
//	Function prototypes
//==================================================================================
void displayGridPane(void);
void displayStatePane(void);
void initializeApplication(void);

/**
 Function that runs the traveler threads
*/
void* TravelerFunc(void*);

/**
 Function to place the thread in its initial position
 @param colorTrail the assigned color of the thrad
 @param row the initial positon row
 @param col th initial position col
 @param colorch the array which holds the information on color channel
*/
void placeColorOnGrid(TravelerType colorTrail,unsigned int row, unsigned int col);

using namespace std;

/*! enum for the types of locks */
typedef enum LockType {
						REDLOCK = 0, /*!< lock for red ink*/
						GREENLOCK,   /*!< lock for green ink*/
						BLUELOCK,    /*!< lock for blue ink*/
						DRAWLOCK,    /*!< lock for drawing the grid*/
						STATELOCK,   /*!< lock for drawing the state ink*/
						//
						NUM_LOCKS
} LockType;
//==================================================================================
//	Application-level global variables
//==================================================================================

//	Don't touch
extern int	GRID_PANE, STATE_PANE;
extern int	gMainWindow, gSubwindow[2];

//	The state grid and its dimensions
int** grid;
const int NUM_ROWS = 20, NUM_COLS = 20;

//	the number of live threads (that haven't terminated yet)
int MAX_NUM_TRAVELER_THREADS = 9;
int numLiveThreads = 0;

//	the ink levels
int MAX_LEVEL = 50;
int MAX_ADD_INK = 10;
int redLevel = 20, greenLevel = 10, blueLevel = 40;

//	ink producer sleep time (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int producerSleepTime = 100000;

//	Enable this declaration if you want to do the traveler information
//	maintaining extra credit section
//TravelerInfo *travelList;

// an array of mutex locks
pthread_mutex_t myLock[NUM_LOCKS];



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

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render the grid.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	pthread_mutex_lock(&myLock[DRAWLOCK]);
	drawGrid(grid, NUM_ROWS, NUM_COLS);
	pthread_mutex_unlock(&myLock[DRAWLOCK]);
	
	//	Use this drawing call instead if you do the extra credits for
	//	maintaining traveler information
	// drawGridAndTravelers(grid, NUM_ROWS, NUM_COLS, travelList);
	
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

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	pthread_mutex_lock(&myLock[STATELOCK]);
	drawState(numLiveThreads, redLevel, greenLevel, blueLevel);
	pthread_mutex_unlock(&myLock[STATELOCK]);
	
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

//------------------------------------------------------------------------
//	These are the functions that would be called by a traveler thread in
//	order to acquire red/green/blue ink to trace its trail.
//	You *must* synchronized access to the ink levels
//------------------------------------------------------------------------
//

/**
 Function to get red ink from the red ink tank
 @param theRed number to take away from the ink tank
*/
int acquireRedInk(int theRed)
{
	int ok = 0;
	// added mutex lock and unlock
	pthread_mutex_lock(&myLock[REDLOCK]);
	if (redLevel >= theRed)
	{
		redLevel -= theRed;
		ok = 1;
	}
	pthread_mutex_unlock(&myLock[REDLOCK]);
	return ok;
}

/**
 Function to get green ink form the ink tank
 @param theGreen number to take away from the ink tank
*/
int acquireGreenInk(int theGreen)
{
	int ok = 0;
	// added mutex lock and unlock
	pthread_mutex_lock(&myLock[GREENLOCK]);
	if (greenLevel >= theGreen)
	{
		greenLevel -= theGreen;
		ok = 1;
	}
	pthread_mutex_unlock(&myLock[GREENLOCK]);
	return ok;
}

/**
 Function to get blue ink from the ink tank
 @param theBlue number to take away from the ink tank
*/
int acquireBlueInk(int theBlue)
{
	int ok = 0;
	//added mutex lock and unlock
	pthread_mutex_lock(&myLock[BLUELOCK]);
	if (blueLevel >= theBlue)
	{
		blueLevel -= theBlue;
		ok = 1;
	}
	pthread_mutex_unlock(&myLock[BLUELOCK]);
	return ok;
}

//------------------------------------------------------------------------
//	These are the functions that would be called by a producer thread in
//	order to refill the red/green/blue ink tanks.
//	You *must* synchronized access to the ink levels
//------------------------------------------------------------------------
//

/**
 Function to refill the red ink
 @param theRed the number to fill the red ink tank with
*/
int refillRedInk(int theRed)
{
	int ok = 0;
	pthread_mutex_lock(&myLock[REDLOCK]);
	if (redLevel + theRed <= MAX_LEVEL)
	{
		redLevel += theRed;
		ok = 1;
	}
	pthread_mutex_unlock(&myLock[REDLOCK]);
	return ok;
}

/**
 Function to refill the green ink
 @param theRed the number to fill the green ink tank with
*/
int refillGreenInk(int theGreen)
{
	int ok = 0;
	pthread_mutex_lock(&myLock[GREENLOCK]);
	if (greenLevel + theGreen <= MAX_LEVEL)
	{
		greenLevel += theGreen;
		ok = 1;
	}
	pthread_mutex_unlock(&myLock[GREENLOCK]);
	return ok;
}

/**
 Function to refill the blue ink
 @param theRed the number to fill the blue ink tank with
*/
int refillBlueInk(int theBlue)
{
	int ok = 0;
	pthread_mutex_lock(&myLock[BLUELOCK]);
	if (blueLevel + theBlue <= MAX_LEVEL)
	{
		blueLevel += theBlue;
		ok = 1;
	}
	pthread_mutex_unlock(&myLock[BLUELOCK]);
	return ok;
}

//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupProducers(void)
{
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * producerSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		producerSleepTime = newSleepTime;
	}
}

void slowdownProducers(void)
{
	//	increase sleep time by 20%
	producerSleepTime = (12 * producerSleepTime) / 10;
}

//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function
//------------------------------------------------------------------------
int main(int argc, char** argv)
{
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane);
	
	//	Now we can do application-level
	initializeApplication();
	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	
	//	Free allocated resource before leaving (not absolutely needed, but
	//	just nicer.  Also, if you crash there, you know something is wrong
	//	in your code.
	for (int i=0; i< NUM_ROWS; i++)
		free(grid[i]);
	free(grid);
	//
//	free(travelList);
	
	//	This will never be executed (the exit point will be in one of the
	//	call back functions).
	return 0;
}


//==================================================================================
//
//	This is a part that you have to edit and add to.
//
//==================================================================================


void initializeApplication(void)
{
	//	Allocate the grid
	grid = (int**) malloc(NUM_ROWS * sizeof(int*));
	for (int i=0; i<NUM_ROWS; i++)
		grid[i] = (int*) malloc(NUM_COLS * sizeof(int));
	
	//---------------------------------------------------------------
	//	All the code below to be replaced/removed
	//	I initialize the grid's pixels to have something to look at
	//---------------------------------------------------------------
	//	Yes, I am using the C random generator after ranting in class that the C random
	//	generator was junk.  Here I am not using it to produce "serious" data (as in a
	//	simulation), only some color, in meant-to-be-thrown-away code
	
	for (int i=0; i<NUM_ROWS; i++)
	{
		for (int j=0; j<NUM_COLS; j++)
		{
			
			grid[i][j] = 0xFF000000;
			
		}
	}

	//create the threads to run traveler threads
	pthread_t travelthrs[MAX_NUM_TRAVELER_THREADS];
	numLiveThreads=MAX_NUM_TRAVELER_THREADS;

	// initialize all the mutex locks
	for (int i=0;i<NUM_LOCKS;i++){
		pthread_mutex_init(&myLock[i],NULL);
	}

	// create all the threads and check for error
	for (int i=0;i<MAX_NUM_TRAVELER_THREADS;i++){
		int err = pthread_create(&travelthrs[i],NULL,TravelerFunc,NULL);
		if(err != 0){
			cout<<"Could not create Thread "<<endl;
			exit(1);
		}
	}

	//	Enable this code if you want to do the traveler information
	//	maintaining extra credit section
	// travelList = (TravelerInfo*) malloc(MAX_NUM_TRAVELER_THREADS * sizeof(TravelerInfo));
	// for (int k=0; k< MAX_NUM_TRAVELER_THREADS; k++)
	// {
	// 	travelList[k].type = (TravelerType)(rand() % NUM_TRAV_TYPES);
	// 	travelList[k].row = rand() % NUM_ROWS;
	// 	travelList[k].col = rand() % NUM_COLS;
	// 	travelList[k].dir = (TravelDirection)(rand() % NUM_TRAVEL_DIRECTIONS);
	// 	travelList[k].isLive = 1;
	// }
}

void* TravelerFunc(void*){

	unsigned int row = rand() % NUM_ROWS; // get the starting row
	unsigned int col = rand() % NUM_COLS; // get the starting col
	TravelerType colorTrail = (TravelerType) (rand() % NUM_TRAV_TYPES); // get a random color
	TravelDirection dir = (TravelDirection) (rand() % NUM_TRAVEL_DIRECTIONS); // get a random direction
	int randomdir[] = {1,3}; // array to hold the int value which represents the new displacement


	// call this function to place the initial position of thread
	placeColorOnGrid(colorTrail,row,col);

	// displacement 
	while (true){
		// first checks if the current position of the thread is in the corner if it is break out of the loop
		if((row==NUM_ROWS-1 && col==NUM_COLS-1) || (row==NUM_ROWS-1 && col==0) || (row==0 && col==NUM_COLS-1) || (row==0 && col==0)){
			break;
		}
		// check for the direction it is going to go and move it 
		switch (dir) {
			case NORTH:
				if(row+1==NUM_ROWS){
					row--;
				}else{
					row++;
				}
				break;
			case WEST:
				if(col==0){
					col++;
				}
				else{
					col--;
				}
				break;
			case SOUTH:
				if(row==0){
					row++;
				}else{
					row--;
				}
				break;
			default:
				if(col+1==NUM_COLS){
					col--;
				}else{
					col++;
				}
				break;
				
		}
		// place the color on the grid of desire position
		placeColorOnGrid(colorTrail,row,col);
		// get a random displacement that is going to left or right
		dir = (TravelDirection)((dir+randomdir[rand()%2]) % NUM_TRAVEL_DIRECTIONS);
	}
	// the end of loop indicates the end of thread
	numLiveThreads--;
	return NULL;
}

void placeColorOnGrid(TravelerType colorTrail,unsigned int row,unsigned int col){
	// loop control variable
	bool looper=true;

	// will continuously loop
	while(looper){
		switch(colorTrail){
			case RED_TRAV:
				// check for ink first before placing the color down on grid
				if (acquireRedInk(10)){
					// place the color in the grid
					int pixel = grid[row][col];
					grid[row][col] = pixel | 0xFF0000FF; 
					looper=false;
				}
				break;
			case GREEN_TRAV:
				if (acquireGreenInk(10)){
					// place color in the grid
					int pixel = grid[row][col];
					grid[row][col] = pixel | 0xFF00FF00;
					looper=false;
				}
				break;
			default:
				if (acquireBlueInk(10)){
					// place color in the grid
					int pixel = grid[row][col];
					grid[row][col] = pixel | 0xFFFF0000;
					looper=false;
				}
				break;
		}
		usleep(100);
	}
}

