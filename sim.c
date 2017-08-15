/******************************************************************************

                                                         Author: Jason Ma
                                                         Date:   Aug 14 2017
                                      mmSim

  File Name:       sim.c
  Description:     Simulation interface for mouse algorithms built on ncurses.
                   The simulation can be controlled through keyboard input,
                   featuring several layers of drawings. This way, any 16x16
                   array can be visualized along with wall information and 
                   mouse locations. These can also be done in different colors.
******************************************************************************/

#include <ncurses.h>
#include <stdio.h>
#include "sim.h"
#include "mouse.h"

int actualMaze[MAZE_WIDTH][MAZE_WIDTH] = {};

/******************************************************************************
  Routine Name: main
  File:         sim.c
  
  Description: Drives program, handles input, and delegates tasks based on
               pressed keys
******************************************************************************/
int main() {
  char key;
  int row, col;
  bool result;
  init(); 

  do{
    printMenu(CYAN);
    switch(key) {
      case 'l':                       //Load and draw maze
        initMaze(actualMaze, MAZE_FILE);
        for(row = 0; row < MAZE_WIDTH; row++)
          for(col = 0; col < MAZE_WIDTH; col++)
            drawTileWalls(col, row, actualMaze[row][col], BLUE);
        break;
      case 'p':                       //Display maze array 
        printMaze(actualMaze, CYAN);
        break;
      case 'd':                       //Draw empty maze
        drawMaze(EMPTY, YELLOW);
        break;
      case 'c':                       //Clear maze and maze array printouts
        drawMaze(FULL, CLEAR);
        printMaze(actualMaze, CLEAR);
        break;
      case 'i':
        initMouse(15, 0);
        break;
      case 'r':
        runMouse(MAPPING, YELLOW);
        break;
      case 'f':
        drawArray(ARRAY_FLOODMAZE, CYAN);
        break;
      case 'm':
        drawArray(ARRAY_MOUSEMAZE, CYAN);
        break;
      case 'a':
        drawArray(ARRAY_ACTUALMAZE, BLUE);
        break;
      case 't':
        runMouse(OPTIMIZING, YELLOW);
        break;
    }
    key = ' ';
    timeout(TIMEOUT);
  }while((key = getch()) != 'q' && key != 'Q');
  endwin();               //deallocate mem associated with ncurses
  return 0;
}

/******************************************************************************
  Routine Name: drawArray
  File:         sim.c
  
  Description: Draws specified maze using drawMode color pair onto the screen.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  arr                array containing maze
  drawMode           color pair to use when drawing maze
******************************************************************************/
void drawArray(int arr, int drawMode) {
  int row, col, value;
  
  attron(COLOR_PAIR(drawMode));

  for(row = 0; row < MAZE_WIDTH; row++)
    for(col = 0; col < MAZE_WIDTH; col++) {
      if(arr == ARRAY_MOUSEMAZE)
        value = getMouseMaze(row, col);
      else if(arr == ARRAY_FLOODMAZE)
        value = getFloodMaze(row, col);
      else if(arr == ARRAY_ACTUALMAZE)
        value = actualMaze[row][col];
      mvprintw(MAZE_START_ROW + row * 2 + 1, MAZE_START_COL + col * 3 + 1,
               "%2x", value);
    }

  attroff(COLOR_PAIR(drawMode));
}

/******************************************************************************
  Routine Name: runMouse
  File:         sim.c
  
  Description: Interface used to call mouse functions.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  mouseMode          mode mouse is in
  drawMode           color pair to color tile walls that were just updated
******************************************************************************/
void runMouse(int mouseMode, int drawMode) {
  int curRow, curCol;

  curRow = getMouseRow();
  curCol = getMouseCol();
  
  if(mouseMode == SPEED) {
    buildSpeedPath(8, 8);
    speedRun(8, 8);
    return;
  }
  else if(mouseMode == MAPPING)
    mapMaze(8, 8, STEP);
  else if(mouseMode == OPTIMIZING)
    mapMaze(15, 0, STEP);
  
  drawTileWalls(curCol, curRow, getMouseMaze(curRow, curCol), drawMode);

  drawMouse(curRow, curCol);
}

/******************************************************************************
  Routine Name: drawMouse
  File:         sim.c
  
  Description: Draws mouse marker on maze
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  mouseRow           row of mouse
  mouseCol           col of mouse
******************************************************************************/
void drawMouse(int mouseRow, int mouseCol) {
  int row, col;

  for(row = 0; row < MAZE_WIDTH; row++)
    for(col = 0; col < MAZE_WIDTH; col++)
      mvaddch(MAZE_START_ROW + row * 2 + 1, MAZE_START_COL + col * 3 + 1, ' ');
  
  mvaddch(MAZE_START_ROW + mouseRow * 2 + 1, MAZE_START_COL + mouseCol * 3 + 1,
          ACS_DIAMOND);
}

/******************************************************************************
  Routine Name: printMenu
  File:         sim.c
  
  Description: Displays status bar to side of maze
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  mode               color pair used to draw menu
******************************************************************************/
void printMenu(int mode) {
  int row, col, i;

  row = MAZE_START_ROW;
  col = MAZE_START_COL + MAZE_WIDTH * 3 + 3;
  attron(COLOR_PAIR(mode));
  
  //Print menu box
  mvaddch(row, col, ACS_ULCORNER);
  for(i = 0; i < MENU_WIDTH - 2; i++) {
    addch(ACS_HLINE);
  }
  addch(ACS_URCORNER);

  for(i = row + 1; i < MENU_HEIGHT; i++) {
    mvaddch(i, col, ACS_VLINE);
    mvaddch(i, col + MENU_WIDTH - 1, ACS_VLINE); 
  }

  mvaddch(row + MENU_HEIGHT - 1, col, ACS_LLCORNER);
  for(i = 0; i < MENU_WIDTH - 2; i++) {
    addch(ACS_HLINE);
  }
  addch(ACS_LRCORNER);

  //Print menu
  mvprintw(row + 1, col + 1, "l Load maze");
  mvprintw(row + 2, col + 1, "d Draw empty maze");
  mvprintw(row + 3, col + 1, "a Disp array");
  mvprintw(row + 4, col + 1, "i Init mouse");
  mvprintw(row + 5, col + 1, "r Run mouse step");
  mvprintw(row + 6, col + 1, "f Disp floodMaze");
  mvprintw(row + 7, col + 1, "m Disp mouseMaze");
  mvprintw(row + 8, col + 1, "c Clear maze");
  attroff(COLOR_PAIR(mode));
}

/******************************************************************************
  Routine Name: printMaze
  File:         sim.c
  
  Description: Prints maze to screen using color pair specified.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  maze               maze in array form to print
  mode               color pair to use
******************************************************************************/
void printMaze(int maze[MAZE_WIDTH][MAZE_WIDTH], int mode) {
  int row, col;
  
  attron(COLOR_PAIR(mode));

  for(row = 0; row < MAZE_WIDTH; row++) {
    move(row, 0);
    for(col = 0; col < MAZE_WIDTH; col++)
      printw("%x ", maze[row][col]);
  }

  attroff(COLOR_PAIR(mode));
}

/******************************************************************************
  Routine Name: drawTileWalls
  File:         sim.c
  
  Description: Draws walls of specified color for tile at given coordinates
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  xTile              x location of tile
  yTile              y location of tile
  walls              wall information
  mode               color pair to use
******************************************************************************/
void drawTileWalls(int xTile, int yTile, int walls, int mode) {
  int xDraw, yDraw;
  
  xDraw = MAZE_START_COL + xTile * 3 + 1;
  yDraw = MAZE_START_ROW + yTile * 2 + 1;
  
  attron(COLOR_PAIR(mode));
  
  if((walls & LEFT_WALL) == LEFT_WALL) {
    mvaddch(yDraw, xDraw - 1, ACS_VLINE);
  }
  if((walls & RIGHT_WALL) == RIGHT_WALL) {
    mvaddch(yDraw, xDraw + 2, ACS_VLINE);
  }
  if((walls & UPPER_WALL) == UPPER_WALL) {
    mvaddch(yDraw - 1, xDraw, ACS_HLINE);
    addch(ACS_HLINE);
  }
  if((walls & LOWER_WALL) == LOWER_WALL) {
    mvaddch(yDraw + 1, xDraw, ACS_HLINE);
    addch(ACS_HLINE);
  }
  
  attroff(COLOR_PAIR(mode));
}

/******************************************************************************
  Routine Name: drawMaze
  File:         sim.c
  
  Description: Draws either full or empty maze
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  type               full or empty
  mode               color pair to use
******************************************************************************/
void drawMaze(int type, int mode) {
  int row, col;
  
  attron(COLOR_PAIR(mode));
  if(type == FULL) {
    for(row = MAZE_START_ROW; row <= MAZE_START_ROW + MAZE_WIDTH * 2; row++) {
      //move cursor to beginning of row
      move(row, MAZE_START_COL);
      if(row % 2 == 0)
        for(col = 0; col < MAZE_WIDTH + 1; col++)
          mvaddch(row, MAZE_START_COL + col * 3, ACS_VLINE);
      else {
        addch(ACS_BULLET);
        for(col = 0; col < MAZE_WIDTH; col++) {
          addch(ACS_HLINE);
          addch(ACS_HLINE);
          addch(ACS_BULLET);
        }
      }
    }
  }
  else if(type == EMPTY) {
    row = MAZE_START_ROW;
    col = MAZE_START_COL;
    
    for(row = MAZE_START_ROW; row <= MAZE_START_ROW + MAZE_WIDTH * 2; row++) {
      if(row == MAZE_START_ROW || row == MAZE_START_ROW + MAZE_WIDTH * 2) {
        move(row, MAZE_START_COL);
        addch(ACS_BULLET);
        for(col = 0; col < MAZE_WIDTH; col++) {
          addch(ACS_HLINE);
          addch(ACS_HLINE);
          addch(ACS_BULLET);
        }
      }
      else if(row % 2 == 0) {
        mvaddch(row, MAZE_START_COL, ACS_VLINE);
        mvaddch(row, MAZE_START_COL + MAZE_WIDTH * 3, ACS_VLINE);
      }
      else {
        for(col = 0; col <= MAZE_WIDTH; col++) {
          mvaddch(row, MAZE_START_COL + col * 3, ACS_BULLET);
        }
      }
    }
  }

  attroff(COLOR_PAIR(mode));
}

/******************************************************************************
  Routine Name: initMaze
  File:         sim.c
  
  Description: Initializes maze with file containing wall information
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  maze               array to fill
  mazeStr            file containing wall info
******************************************************************************/
void initMaze(int maze[MAZE_WIDTH][MAZE_WIDTH], char * mazeStr) {
  FILE * mazeFile;
  int row, col, wall;
  mazeFile = fopen(mazeStr, "r+");

  for(row = 0; row < MAZE_WIDTH; row++) {
    for(col = 0; col < MAZE_WIDTH; col++) {
      fscanf(mazeFile, "%d", &wall);
      maze[row][col] = wall;
    }
    
  }

  fclose(mazeFile);
}

/******************************************************************************
  Routine Name: init
  File:         sim.c
  
  Description: Initializes color pairs and ncurses
******************************************************************************/
void init() {
  
  initscr();             //initializes ncurses
  raw();

  start_color();         //initializes colors
  init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CLEAR, COLOR_BLACK, COLOR_BLACK);
  init_pair(DEBUG, COLOR_BLACK, COLOR_YELLOW);
  curs_set(0);
  noecho();
}

/******************************************************************************
  Routine Name: getActualMaze
  File:         sim.c
  
  Description: Gets wall info of a tile from actual maze
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                row of tile
  col                col of tile
******************************************************************************/
int getActualMaze(int row, int col) {
  return actualMaze[row][col];
}
