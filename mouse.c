#include "sim.h"
#include "mouse.h"

int mouseMaze[MAZE_WIDTH][MAZE_WIDTH];
int floodMaze[MAZE_WIDTH][MAZE_WIDTH];
int floodQueue[MAZE_WIDTH * MAZE_WIDTH][2];
int curRow;
int curCol;
int queueTop;
int destRow, destCol;

void initMouse(int startRow, int startCol) {
  int row, col;

  curRow = startRow;
  curCol = startCol;

  for(row = 0; row < MAZE_WIDTH; row++)
    for(col = 0; col < MAZE_WIDTH; col++) {
      mouseMaze[row][col] = 0;
      floodMaze[row][col] = MAZE_WIDTH * MAZE_WIDTH - 1;
    }

  queueTop = 0;
}

void mapMaze(int targetRow, int targetCol, int mode) {
  int curLoc;
  //read wall info
  //floodfill
  //move

  int atDest;

  destRow = targetRow;
  destCol = targetCol;
  floodFill(0, targetRow, targetCol);

  do{
    curLoc = mouseMaze[curRow][curCol];
    scanWalls(curRow, curCol);
    //if(curLoc != mouseMaze[curRow][curCol]) {
      //updateFlood(curRow, curCol);
      //floodFromQueue();
      validateFlood(curRow, curCol);
    //}
    mouseMove(FLOOD, 0, 0);

    //closeBoxes();

    if(mode == STEP)
      return;

    atDest = atLoc(targetRow, targetCol);
  }while(atDest == 0);
}

int atLoc(int row, int col) {
  if(curRow == row && curCol == col)
    return 1;
  return 0;
}

void scanWalls(int row, int col) {
  int actualWall;
  
  actualWall = getActualMaze(row, col);
  if(actualWall & LEFT_WALL && (mouseMaze[row][col] & LEFT_WALL) == 0) {
    mouseMaze[row][col] |= LEFT_WALL;

    if(col > 0)
      mouseMaze[row][col - 1] |= RIGHT_WALL;
  }
  if(actualWall & RIGHT_WALL && (mouseMaze[row][col] & RIGHT_WALL) == 0) {
    mouseMaze[row][col] |= RIGHT_WALL;

    if(col < MAZE_WIDTH - 1)
      mouseMaze[row][col + 1] |= LEFT_WALL;
  }

  if(actualWall & UPPER_WALL && (mouseMaze[row][col] & UPPER_WALL) == 0) {
    mouseMaze[row][col] |= UPPER_WALL;

    if(row > 0)
      mouseMaze[row - 1][col] |= LOWER_WALL;
  }

  if(actualWall & LOWER_WALL && (mouseMaze[row][col] & LOWER_WALL) == 0) {
    mouseMaze[row][col] |= LOWER_WALL;

    if(row < MAZE_WIDTH - 1)
      mouseMaze[row + 1][col] |= UPPER_WALL;
  }
}

void updateFlood(int row, int col) {
  int lower, higher, nonNeighbor;

  //search for lower neighbors that this tile depends on
  lower = findLowerNeighbor(row, col);

  //found at least 1 lower neighbor
  if(lower != 0) {
    //add self to flood queue
    floodQueue[queueTop][0] = row;
    floodQueue[queueTop][1] = col;
    queueTop++;
  }
  else {
    
    //find all higher flood value neighbors
    higher = lower ^ (mouseMaze[row][col] & WALLS);

    //reset current tile flood
    floodMaze[row][col] = MAZE_WIDTH * MAZE_WIDTH - 1;

    //for each higher flood neighbor, recursively update flooding
    
    if((higher & LEFT) != LEFT)
      updateFlood(row, col - 1);
    if((higher & UP) != UP)
      updateFlood(row - 1, col);
    if((higher & RIGHT) != RIGHT)
      updateFlood(row, col + 1);
    if((higher & DOWN) != DOWN)
      updateFlood(row + 1, col);
    
    /*
    if((lower & LEFT) != LEFT)
      updateFlood(row, col - 1);
    if((lower & UP) != UP)
      updateFlood(row - 1, col);
    if((lower & RIGHT) != RIGHT)
      updateFlood(row, col + 1);
    if((lower & DOWN) != DOWN)
      updateFlood(row + 1, col);
    */
    /*
    nonNeighbor = (lower | higher) ^ WALLS;
    
    if(nonNeighbor != 0) {
      
    }
    */
  }
  
  //update non-neighbor tiles that depended on this tile
  /*
  if() {
  if(floodMaze[row - 1][col] > floodMaze[row][col])
    updateFlood(row - 1, col);
  if(floodMaze[row + 1][col] > floodMaze[row][col])
    updateFlood(row + 1, col);
  if(floodMaze[row][col - 1] > floodMaze[row][col])
    updateFlood(row, col - 1);
  if(floodMaze[row][col + 1] > floodMaze[row][col])
    updateFlood(row, col + 1);
  }
  */
}

void floodFromQueue() {
  //for each value in array, flood from location
  int i;

  for(i = 0; i < queueTop; i++) {
    floodFill(floodMaze[floodQueue[i][0]][floodQueue[i][1]], floodQueue[i][0], floodQueue[i][1]);
  }

  queueTop = 0;
}

void validateFlood(int row, int col) {
  int lower, lowest;


  if(row == destRow && col == destCol)
    return;

  //find all neighbors with lower values
  lower = findLowerNeighbor(row, col);
  /*
  if((lower & UP) == UP && floodMaze[row - 1][col] == num - 1)
    return;
  if((lower & DOWN) == DOWN && floodMaze[row + 1][col] == num - 1)
    return;
  if((lower & LEFT) == LEFT && floodMaze[row][col - 1] == num - 1)
    return;
  if((lower & RIGHT) == RIGHT && floodMaze[row][col + 1] == num - 1)
    return;
  */

  //determine lowest flood value of neighbors
  lowest = 255;

  if(row > 0 && (mouseMaze[row][col] & UPPER_WALL) != UPPER_WALL)
    lowest = floodMaze[row - 1][col];
  if(row < MAZE_WIDTH - 1 && (mouseMaze[row][col] & LOWER_WALL) != LOWER_WALL && floodMaze[row + 1][col] < lowest)
    lowest = floodMaze[row + 1][col];
  if(col > 0 && (mouseMaze[row][col] & LEFT_WALL) != LEFT_WALL && floodMaze[row][col - 1] < lowest)
    lowest = floodMaze[row][col - 1];
  if(col < MAZE_WIDTH - 1 && (mouseMaze[row][col] & RIGHT_WALL) != RIGHT_WALL && floodMaze[row][col + 1] < lowest)
    lowest = floodMaze[row][col + 1];
  
  //if value is valid, do nothing
  if(floodMaze[row][col] == lowest + 1)
    return;
 
  //otherwise, make value valid
  floodMaze[row][col] = lowest + 1;
  
  if(row > 0)
    validateFlood(row - 1, col);
  if(row < MAZE_WIDTH - 1)
    validateFlood(row + 1, col);
  if(col > 0)
    validateFlood(row, col - 1);
  if(col < MAZE_WIDTH - 1)
    validateFlood(row, col + 1);

  /*
  if(row > 0 && (mouseMaze[row][col] & UPPER_WALL) != UPPER_WALL)
    validateFlood(row - 1, col);
  if(row < MAZE_WIDTH - 1 && (mouseMaze[row][col] & LOWER_WALL) != LOWER_WALL)
    validateFlood(row + 1, col);
  if(col > 0 && (mouseMaze[row][col] & LEFT_WALL) != LEFT_WALL)
    validateFlood(row, col - 1);
  if(col < MAZE_WIDTH - 1 && (mouseMaze[row][col] & RIGHT_WALL) != RIGHT_WALL)
    validateFlood(row, col + 1);
  */
}

void floodFill(int num, int row, int col) {
  floodMaze[row][col] = num;

  if(row > 0 && floodMaze[row - 1][col] > num && (mouseMaze[row][col] & UPPER_WALL) == 0)
    floodFill(num + 1, row - 1, col);
  if(row < MAZE_WIDTH - 1 && floodMaze[row + 1][col] > num && (mouseMaze[row][col] & LOWER_WALL) == 0)
    floodFill(num + 1, row + 1, col);

  if(col > 0 && floodMaze[row][col - 1] > num && (mouseMaze[row][col] & LEFT_WALL) == 0)
    floodFill(num + 1, row, col - 1);
  if(col < MAZE_WIDTH - 1 && floodMaze[row][col + 1] > num && (mouseMaze[row][col] & RIGHT_WALL) == 0)
    floodFill(num + 1, row, col + 1);
}

void closeBoxes() {
  int row, col;

  for(row = 0; row < MAZE_WIDTH; row++)
    for(col = 0; col < MAZE_WIDTH; col++)
      //check if 1 bit is set to 0
      if(((mouseMaze[row][col] ^ WALLS) & ((mouseMaze[row][col] ^ WALLS) - 1)) == 0)
        if((row == curRow && col == curCol) == 0) {
          switch(mouseMaze[row][col] ^ WALLS) {
            case LEFT_WALL:
              mouseMaze[row][col] |= LEFT_WALL;
              mouseMaze[row][col - 1] |= RIGHT_WALL;
              break;
            case UPPER_WALL:
              mouseMaze[row][col] |= UPPER_WALL;
              mouseMaze[row - 1][col] |= LOWER_WALL;
              break;
            case RIGHT_WALL:
              mouseMaze[row][col] |= RIGHT_WALL;
              mouseMaze[row][col + 1] |= LEFT_WALL;
              break;
            case LOWER_WALL:
              mouseMaze[row][col] |= LOWER_WALL;
              mouseMaze[row + 1][col] |= UPPER_WALL;
              break;
          }
          //mouseMaze[row][col] = WALLS;
          drawTileWalls(col, row, mouseMaze[row][col], YELLOW);
        }
}

void mouseMove(int mode, int row, int col) {
  int lowest;
  int dir;
  if(mode == FLOOD) {
    dir = findLowerNeighbor(curRow, curCol);

    if((dir & LEFT) == LEFT)
      curCol--;
    else if((dir & UP) == UP)
      curRow--;
    else if((dir & RIGHT) == RIGHT)
      curCol++;
    else if((dir & DOWN) == DOWN)
      curRow++;

    mvprintw(30, 50, "%x", dir);
  }
  else if(mode == MANUAL) {
    curRow = row;
    curCol = col;
  }
}

int findLowerNeighbor(int row, int col) {
  
  /* 
   * lowest - smallest found flood
   * result - lowest directions
   */
  int lowest, result;
  
  result = 0;
  //lowest = MAZE_WIDTH * MAZE_WIDTH - 1;
  lowest = floodMaze[row][col];

  /* 1. Check if adjacent tile is neighbor (accessible by mouse)
   * 2. If neighboring, see if flood value is lower or equal to current lowest
   * 3. Update lowest and lowest tile bits accordingly
   */

  //Check upper neighbor
  if((mouseMaze[row][col] & UPPER_WALL) != UPPER_WALL) {
    if(floodMaze[row - 1][col] < lowest) {
      lowest = floodMaze[row - 1][col];
      result = UP;
    }
    else if(floodMaze[row - 1][col] == lowest)
      result |= UP;
  }

  //Check lower neighbor
  if((mouseMaze[row][col] & LOWER_WALL) != LOWER_WALL) {
    if(floodMaze[row + 1][col] < lowest) {
      lowest = floodMaze[row + 1][col];
      result = DOWN;
    }
    else if(floodMaze[row + 1][col] == lowest)
      result |= DOWN;
  }
  
  //Check left neighbor
  if((mouseMaze[row][col] & LEFT_WALL) != LEFT_WALL) {
    if(floodMaze[row][col - 1] < lowest) {
      lowest = floodMaze[row][col - 1];
      result = LEFT;
    }
    else if(floodMaze[row][col - 1] == lowest)
      result |= LEFT;
  }
  
  //Check right neighbor
  if((mouseMaze[row][col] & RIGHT_WALL) != RIGHT_WALL) {
    if(floodMaze[row][col + 1] < lowest) {
      lowest = floodMaze[row][col + 1];
      result = RIGHT;
    }
    else if(floodMaze[row][col + 1] == lowest)
      result |= RIGHT;
  }

  return result;
}

void buildSpeedPath(int targetRow, int targetCol) {
  /* TODO needs vehicle */  
}

void speedRun(int targetRow, int targetCol) {
  /* TODO needs vehicle */
}

int getMouseRow() {
  return curRow;
}

int getMouseCol() {
  return curCol;
}

int getMouseMaze(int row, int col) {
  return mouseMaze[row][col];
}

int getFloodMaze(int row, int col) {
  return floodMaze[row][col];
}
