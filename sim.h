#ifndef SIM_H
#define SIM_H

#define TIMEOUT 1000

#define LEFT_CLOCK_BIT 0x4000

#define DRAW  1
#define DARK  2
#define CLEAR 3
#define DEBUG 4

#define EMPTY 0
#define FULL  1

#define MAZE_START_ROW 1
#define MAZE_START_COL 1
#define MAZE_WIDTH 16

#define MENU_WIDTH  20
#define MENU_HEIGHT 33

#define LEFT_WALL  1
#define UPPER_WALL 2
#define RIGHT_WALL 4
#define LOWER_WALL 8

#define MAZE_FILE "./maze"

/* function definitions */
void printMenu(int mode);
void printMaze(int maze[MAZE_WIDTH][MAZE_WIDTH], int mode);
void drawMaze(int type, int mode);
void drawTileWalls(int yTile, int xTile, int walls, int mode);
void initMaze(int maze[MAZE_WIDTH][MAZE_WIDTH], char * mazeStr);
void init();

#endif /* sim.h */
