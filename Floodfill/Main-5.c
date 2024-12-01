// FloodFill.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "API.h"

#define MAX_X 16
#define MAX_Y 16

// Global variables for the current position of the robot.
int currentRow = 15;  // Starting at the bottom-left corner
int currentCol = 0;

// Array represents the flood fill algorithm
int floodFillPath[MAX_X][MAX_Y];
char wallInfo[MAX_X][MAX_Y];

// Check if there is a wall in a specific direction
int isWallLeft;
int isWallRight;
int isWallFront;

// Values to determine which way the robot is facing
// 0 - Up
// 1 - Right
// 2 - Down
// 3 - Left
int nextDirection = 0;
int currentDirection = 1;

// Print a message to the console
void logMessage(char* message) {
    fprintf(stderr, "%s\n", message);
    fflush(stderr);
}

// Initialize the flood fill path array values to -1
void initializeFloodFillPath() {
    for (int i = 0; i < MAX_X; i++) {
        for (int j = 0; j < MAX_Y; j++) {
            floodFillPath[i][j] = -1;
        }
    }
}

// Recursive function representing the flood fill algorithm
void floodFill(int row, int col, int distance) {
    if (row >= MAX_X || row < 0 || col >= MAX_Y || col < 0)
        return;
    if (floodFillPath[row][col] <= distance && floodFillPath[row][col] != -1)
        return;
    floodFillPath[row][col] = distance;

    if (col < MAX_Y - 1 && !(wallInfo[row][col] & 0b0100))
        floodFill(row, col + 1, distance + 1);
    if (col > 0 && !(wallInfo[row][col - 1] & 0b0100))
        floodFill(row, col - 1, distance + 1);
    if (row < MAX_X - 1 && !(wallInfo[row][col] & 0b0010))
        floodFill(row + 1, col, distance + 1);
    if (row > 0 && !(wallInfo[row - 1][col] & 0b0010))
        floodFill(row - 1, col, distance + 1);
}

// Function to get sensor data
void fetchSensorData() {
    isWallLeft = API_wallLeft();
    isWallFront = API_wallFront();
    isWallRight = API_wallRight();
}

// Function to update the wall information array
void updateWallInfo() {
    int maxIndex = MAX_X - 1;
    // If the robot is facing up
    if (currentDirection == 0) {
        if (isWallLeft && currentCol != 0) {
            wallInfo[currentRow][currentCol - 1] |= 0b0100;
            API_setWall(currentCol, maxIndex - currentRow, 'w');
        }
        if (isWallRight && currentCol != maxIndex) {
            wallInfo[currentRow][currentCol] |= 0b0100;
            API_setWall(currentCol, maxIndex - currentRow, 'e');
        }
        if (isWallFront && currentRow != 0) {
            wallInfo[currentRow - 1][currentCol] |= 0b0010;
            API_setWall(currentCol, maxIndex - currentRow, 'n');
        }
    }
    // If the robot is facing right
    else if (currentDirection == 1) {
        if (isWallLeft && currentRow != 0) {
            wallInfo[currentRow - 1][currentCol] |= 0b0010;
            API_setWall(currentCol, maxIndex - currentRow, 'n');
        }
        if (isWallRight && currentRow != maxIndex) {
            wallInfo[currentRow][currentCol] |= 0b0010;
            API_setWall(currentCol, maxIndex - currentRow, 's');
        }
        if (isWallFront && currentCol != maxIndex) {
            wallInfo[currentRow][currentCol] |= 0b0100;
            API_setWall(currentCol, maxIndex - currentRow, 'e');
        }
    }
    // If the robot is facing down
    else if (currentDirection == 2) {
        if (isWallLeft && currentCol != maxIndex) {
            wallInfo[currentRow][currentCol] |= 0b0100;
            API_setWall(currentCol, maxIndex - currentRow, 'e');
        }
        if (isWallRight && currentCol != 0) {
            wallInfo[currentRow][currentCol - 1] |= 0b0100;
            API_setWall(currentCol, maxIndex - currentRow, 'w');
        }
        if (isWallFront && currentRow != maxIndex) {
            wallInfo[currentRow][currentCol] |= 0b0010;
            API_setWall(currentCol, maxIndex - currentRow, 's');
        }
    }
    // If the robot is facing left
    else if (currentDirection == 3) {
        if (isWallLeft && currentRow != maxIndex) {
            wallInfo[currentRow][currentCol] |= 0b0010;
            API_setWall(currentCol, maxIndex - currentRow, 's');
        }
        if (isWallRight && currentRow != 0) {
            wallInfo[currentRow - 1][currentCol] |= 0b0010;
            API_setWall(currentCol, maxIndex - currentRow, 'n');
        }
        if (isWallFront && currentCol != 0) {
            wallInfo[currentRow][currentCol - 1] |= 0b0100;
            API_setWall(currentCol, maxIndex - currentRow, 'w');
        }
    }
}

// Function to determine the best move for the robot
void determineNextMove() {
    int left, right, down, up;
    int maxIndex = MAX_X - 1;

    // Check to see if the robot is on an edge row or column.
    if (currentCol != 0)
        left = floodFillPath[currentRow][currentCol - 1];
    else
        left = 99;

    if (currentCol != maxIndex)
        right = floodFillPath[currentRow][currentCol + 1];
    else
        right = 99;

    if (currentRow != 0)
        up = floodFillPath[currentRow - 1][currentCol];
    else
        up = 99;

    if (currentRow != maxIndex)
        down = floodFillPath[currentRow + 1][currentCol];
    else
        down = 99;

    // Check for walls
    if (currentRow > 0 && (wallInfo[currentRow - 1][currentCol] & 0b0010))
        up = 99;
    if (currentCol > 0 && (wallInfo[currentRow][currentCol - 1] & 0b0100))
        left = 99;
    if (currentCol < maxIndex && (wallInfo[currentRow][currentCol] & 0b0100))
        right = 99;
    if (currentRow < maxIndex && (wallInfo[currentRow][currentCol] & 0b0010))
        down = 99;

    // Determine the direction with the lowest path value
    if (up <= right && up <= down && up <= left) {
        nextDirection = 0;
        return;
    }
    if (left <= right && left <= down && left <= up) {
        nextDirection = 3;
        return;
    }
    if (right <= left && right <= down && right <= up) {
        nextDirection = 1;
        return;
    }
    if (down <= right && down <= left && down <= up) {
        nextDirection = 2;
        return;
    }
}

// Function to turn the robot to the correct direction
void rotateRobot() {
    // Robot is facing up
    if (currentDirection == 0) {
        if (nextDirection == 0)
            return; // Return if already facing the correct way
        if (nextDirection == 1) {
            API_turnRight();
            currentDirection = 1;
            return;
        }
        if (nextDirection == 2) {
            currentDirection = 2;
            API_turnRight();
            API_turnRight();
            return;
        }
        if (nextDirection == 3) {
            currentDirection = 3;
            API_turnLeft();
            return;
        }
    }
    // If the robot is facing right
    if (currentDirection == 1) {
        if (nextDirection == 0) {
            currentDirection = 0;
            API_turnLeft();
            return;
        }
        if (nextDirection == 1) {
            currentDirection = 1;
            return;
        }
        if (nextDirection == 2) {
            currentDirection = 2;
            API_turnRight();
            return;
        }
        if (nextDirection == 3) {
            currentDirection = 3;
            API_turnRight();
            API_turnRight();
            return;
        }
    }
    // If the robot is facing down
    if (currentDirection == 2) {
        if (nextDirection == 0) {
            currentDirection = 0;
            API_turnRight();
            API_turnRight();
            return;
        }
        if (nextDirection == 1) {
            currentDirection = 1;
            API_turnLeft();
            return;
        }
        if (nextDirection == 2) {
            currentDirection = 2;
            return;
        }
        if (nextDirection == 3) {
            currentDirection = 3;
            API_turnRight();
            return;
        }
    }
    // Robot is facing left
    if (currentDirection == 3) {
        if (nextDirection == 0) {
            currentDirection = 0;
            API_turnRight();
            return;
        }
        if (nextDirection == 1) {
            currentDirection = 1;
            API_turnRight();
            API_turnRight();
            return;
        }
        if (nextDirection == 2) {
            currentDirection = 2;
            API_turnLeft();
            return;
        }
        if (nextDirection == 3) {
            currentDirection = 3;
            return;
        }
    }
}

// Move the robot and update its position
void advanceRobot() {
    int maxIndex = MAX_X - 1;
    // If there is no wall in front
    if (isWallFront) {
        return;
    }
    // Update current row and column depending on the direction the mouse is facing
    if (currentDirection == 0 && currentRow > 0) {  // Add boundary check for currentRow
        currentRow = currentRow - 1;
        API_setColor(currentCol, maxIndex - currentRow, 'B');
        API_moveForward();
        return;
    }

    if (currentDirection == 1 && currentCol < maxIndex) { // Add boundary check for currentCol
        currentCol = currentCol + 1;
        API_setColor(currentCol, maxIndex - currentRow, 'B');
        API_moveForward();
        return;
    }

    if (currentDirection == 2 && currentRow < maxIndex) { // Add boundary check for currentRow
        currentRow = currentRow + 1;
        API_setColor(currentCol, maxIndex - currentRow, 'B');
        API_moveForward();
        return;
    }

    if (currentDirection == 3 && currentCol > 0) {  // Add boundary check for currentCol
        currentCol = currentCol - 1;
        API_setColor(currentCol, maxIndex - currentRow, 'B');
        API_moveForward();
        return;
    }
}

int main(int argc, char* argv[]) {
    logMessage("Running...");
    int goalX[2] = {7, 8}, goalY[2] = {7, 8};
    // Set colors for start and goal points
    API_setColor(0, 0, 'R');
    API_setText(0, 0, "Start");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            API_setColor(goalX[i], goalY[j], 'G');
            API_setText(goalX[i], goalY[j], "Goal");
        }
    }

    // Initialize maze as empty
    initializeFloodFillPath();
    // Orient the mouse to be facing right
    API_turnRight();
    // Initial flood fill
    floodFill(7, 7, 0); // Initialize the flood fill for the cell at (7, 7)
    floodFill(7, 8, 0); // Initialize the flood fill for the cell at (7, 8)
    floodFill(8, 7, 0); // Initialize the flood fill for the cell at (8, 7)
    floodFill(8, 8, 0); // Initialize the flood fill for the cell at (8, 8)

    // Run while the robot is not at the center
    while (currentRow != 7 || currentCol != 7) {
        initializeFloodFillPath(); // Clears the path array
        fetchSensorData(); // Checks for walls
        updateWallInfo(); // Updates walls array based on information

        floodFill(7, 7, 0); // Initialize the flood fill for the cell at (7, 7)
        floodFill(7, 8, 0); // Initialize the flood fill for the cell at (7, 8)
        floodFill(8, 7, 0); // Initialize the flood fill for the cell at (8, 7)
        floodFill(8, 8, 0); // Initialize the flood fill for the cell at (8, 8)

        determineNextMove();
        rotateRobot();
        advanceRobot();
    }
}
