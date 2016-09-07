#ifndef SIM_H
#define SIM_H

/* how long to wait for keypress */
#define TIMEOUT 1000

/*  */
#define LEFT_CLOCK_BIT 0x4000

/* ncurses color pair */
#define CYAN   1
#define BLUE   2
#define YELLOW 3
#define CLEAR  4
#define DEBUG  5

/* maze drawing modes */
#define EMPTY 0
#define FULL  1

/* maze parameters */
#define MAZE_START_ROW 1
#define MAZE_START_COL 1
#define MAZE_WIDTH 16

/* maze drawing constants */
#define MENU_WIDTH  20
#define MENU_HEIGHT 33

/* maze array wall bits */
#define LEFT_WALL  1
#define UPPER_WALL 2
#define RIGHT_WALL 4
#define LOWER_WALL 8
#define WALLS      15

/* mazes to get from mouse */
#define ARRAY_MOUSEMAZE  0
#define ARRAY_FLOODMAZE  1
#define ARRAY_ACTUALMAZE 2

/* url for maze file */
#define MAZE_FILE "./maze"

/*
static int actualMaze[MAZE_WIDTH][MAZE_WIDTH] = {};
*/

/* function definitions */
void runMouse(int mouseMode, int drawMode);
void drawArray(int arr, int drawMode);
void drawMouse(int mouseRow, int mouseCol);

void printMenu(int mode);
void printMaze(int maze[MAZE_WIDTH][MAZE_WIDTH], int mode);
void drawMaze(int type, int mode);
void drawTileWalls(int yTile, int xTile, int walls, int mode);
void initMaze(int maze[MAZE_WIDTH][MAZE_WIDTH], char * mazeStr);
void init();

int getActualMaze(int row, int col);
#endif /* sim.h */
