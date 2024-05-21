#include <stdbool.h>

#ifndef BOARD_H
#define BOARD_H

#define BOARD_SIZE 3

typedef struct{
    int value;
    char* filePath;
}reedSwitch;

typedef struct{
    char piece; // ' '
    reedSwitch rs; // 1
}TILE;

void initChessboard();

void initChessboardForTesting();

//Returns an array of length 64, 0 = piece cannot move there, 1 = piece CAN move there.
//MUST call free on the returned array
int* getPossibleMoves(int x, int y);

void displayBoard();

int readReedSwitch(reedSwitch rs);

bool isPickedUp(TILE tile);

bool isPlaced(TILE tile);

int* convertToLEDarray(int* array);

int arrayToBinaryInteger(int* array);

void movePiece(int x, int y, int x2, int y2);

TILE getTile(int x, int y);






#endif