#include <ncurses.h>
#include <stdio.h>
#include "sim.h"
#include "mouse.h"

int actualMaze[MAZE_WIDTH][MAZE_WIDTH] = {};

int main() {
  char key;
  int row, col;
  
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
    }
    key = ' ';
    timeout(TIMEOUT);
  }while((key = getch()) != 'q' && key != 'Q' && key != EOF);
  
  endwin();               //deallocate mem associated with ncurses
  return 0;
}

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

void drawMouse(int mouseRow, int mouseCol) {
  int row, col;

  for(row = 0; row < MAZE_WIDTH; row++)
    for(col = 0; col < MAZE_WIDTH; col++)
      mvaddch(MAZE_START_ROW + row * 2 + 1, MAZE_START_COL + col * 3 + 1, ' ');
  
  mvaddch(MAZE_START_ROW + mouseRow * 2 + 1, MAZE_START_COL + mouseCol * 3 + 1,
          ACS_DIAMOND);
}

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

int getActualMaze(int row, int col) {
  return actualMaze[row][col];
}
