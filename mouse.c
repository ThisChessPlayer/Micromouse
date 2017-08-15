/******************************************************************************

                                                         Author: Jason Ma
                                                         Date:   Aug 14 2017
                                      mmSim

  File Name:       mouse.c
  Description:     Mouse implementation to keep track of mouse state and run
                   algorithms.
******************************************************************************/

#include "sim.h"
#include "mouse.h"

int mouseMaze[MAZE_WIDTH][MAZE_WIDTH];
int floodMaze[MAZE_WIDTH][MAZE_WIDTH];
int floodQueue[MAZE_WIDTH * MAZE_WIDTH][2];
int curRow;
int curCol;
int queueTop;
int destRow, destCol;

/******************************************************************************
  Routine Name: initMouse
  File:         mouse.c
  
  Description: Initializes mouse with the starting location
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  startRow           mouse start row
  startCol           mouse start column
******************************************************************************/
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

/******************************************************************************
  Routine Name: mapMaze
  File:         mouse.c
  
  Description: Traverse through maze until at target location.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  targetRow          target location row
  targetCol          target location column
  mode               if step, only takes one step
******************************************************************************/
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

/******************************************************************************
  Routine Name: atLoc
  File:         mouse.c
  
  Description: Returns 1 (true) if mouse is at location specified
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                target location row
  col                target location column
******************************************************************************/
int atLoc(int row, int col) {
  if(curRow == row && curCol == col)
    return 1;
  return 0;
}

/******************************************************************************
  Routine Name: scanWalls
  File:         mouse.c
  
  Description: References actual maze for wall information, and updates mouse's
               memory with appropriate walls.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                row of tile to update
  col                column of tile to update
******************************************************************************/
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

/******************************************************************************
  Routine Name: updateFlood
  File:         mouse.c
  
  Description: Updates floodfill maze starting from specified tile
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                row of tile to start from
  col                column of tile to start from
******************************************************************************/
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

/******************************************************************************
  Routine Name: floodFromQueue
  File:         mouse.c
  
  Description: Starts a floodfill update from each value in floodQueue
******************************************************************************/
void floodFromQueue() {
  //for each value in array, flood from location
  int i;

  for(i = 0; i < queueTop; i++) {
    floodFill(floodMaze[floodQueue[i][0]][floodQueue[i][1]], floodQueue[i][0], floodQueue[i][1]);
  }

  queueTop = 0;
}

/******************************************************************************
  Routine Name: validateFlood
  File:         mouse.c
  
  Description: Ensures that the flood at a particular tile is valid. If not, it
               validates the tile and validates its neighbors.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                row of tile to validate
  col                column of tile to validate
******************************************************************************/
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

/******************************************************************************
  Routine Name: floodFill
  File:         mouse.c
  
  Description: Full floodfill starting from specified location with flood num.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  num                num to start flood at
  row                row of tile to begin at
  col                column of tile to begin at
******************************************************************************/
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

//TODO maybe integrate this into main algorithm
/******************************************************************************
  Routine Name: closeBoxes
  File:         mouse.c
  
  Description: Boxes dead ends off to reduce computational intensity of
               flooding.
******************************************************************************/
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

/******************************************************************************
  Routine Name: mouseMove
  File:         mouse.c
  
  Description: Moves mouse based on specified mode. Can also force mouse to
               jump to specific location in maze.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  mode               if FLOOD, use floodfill maze to move in best direction.
                     if MANUAL, jump to location
  row                row of tile to jump to (MANUAL mode only)
  col                column of tile to jump to (MANUAL mode only)
******************************************************************************/
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

/******************************************************************************
  Routine Name: findLowerNeighbor
  File:         mouse.c
  
  Description: Find lowest neighbor's direction if one exists. Can return
               multiple neighbors' directions if they tie for lowest
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                row of tile to check neighbors of
  col                column of tile to check neighbors of
******************************************************************************/
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

/******************************************************************************
  Routine Name: buildSpeedPath
  File:         mouse.c
  
  Description: Precomputes path for speed run
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  targetRow          target tile row
  targetCol          target tile column
******************************************************************************/
void buildSpeedPath(int targetRow, int targetCol) {
  /* TODO needs vehicle */  
}

/******************************************************************************
  Routine Name: speedRun
  File:         mouse.c
  
  Description: Run through maze as fast as possible. Need vehicle to implement.
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  targetRow          target tile row
  targetCol          target tile column
******************************************************************************/
void speedRun(int targetRow, int targetCol) {
  /* TODO needs vehicle */
}

/******************************************************************************
  Routine Name: getMouseRow
  File:         mouse.c
  
  Description: Returns row mouse is at
******************************************************************************/
int getMouseRow() {
  return curRow;
}

/******************************************************************************
  Routine Name: getMouseCol
  File:         mouse.c
  
  Description: Returns column mouse is at
******************************************************************************/
int getMouseCol() {
  return curCol;
}

/******************************************************************************
  Routine Name: getMouseMaze
  File:         mouse.c
  
  Description: Returns mouse's memory of a specific tile of the maze
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                row of tile to return
  col                column of tile to return
******************************************************************************/
int getMouseMaze(int row, int col) {
  return mouseMaze[row][col];
}

/******************************************************************************
  Routine Name: getFloodMaze
  File:         mouse.c
  
  Description: Returns mouse's calculation of certain tile of floodfill
  
  Parameter Descriptions:
  name               description
  ------------------ -----------------------------------------------
  row                row of tile to return
  col                col of tile to return
******************************************************************************/
int getFloodMaze(int row, int col) {
  return floodMaze[row][col];
}
