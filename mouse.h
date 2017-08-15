/******************************************************************************

                                                         Author: Jason Ma
                                                         Date:   Aug 14 2017
                                      mmSim

  File Name:       mouse.h
  Description:     Mouse implementation to keep track of mouse state and run
                   algorithms.
******************************************************************************/

#ifndef MOUSE_H
#define MOUSE_H

/*
static int mouseMaze[MAZE_WIDTH][MAZE_WIDTH] = {};
static int floodMaze[MAZE_WIDTH][MAZE_WIDTH] = {255};

static int curRow;
static int curCol;
*/

/* mouse modes */
#define MAPPING    0
#define OPTIMIZING 1
#define SPEED      2

/* movement modes */
#define MANUAL 0
#define FLOOD  1

/* run mouse one cell at a time or run to completion */
#define AUTO 0
#define STEP 1

/* direction constants */
#define LEFT  1
#define UP    2
#define RIGHT 4
#define DOWN  8

void initMouse(int startRow, int startCol);

void mapMaze(int targetRow, int targetCol, int mode);

int atLoc(int row, int col);
void scanWalls(int row, int col);
void updateFlood(int row, int col);
void floodFromQueue();
void validateFlood(int row, int col);

void floodFill(int num, int row, int col);
void closeBoxes();
void mouseMove(int mode, int row, int col);
int findLowerNeighbor(int row, int col);

void buildSpeedPath(int targetRow, int targetCol);
void speedRun(int targetRow, int targetCol);

int getMouseRow();
int getMouseCol();
int getMouseMaze(int row, int col);
int getFloodMaze(int row, int col);
#endif /* MOUSE_H */
