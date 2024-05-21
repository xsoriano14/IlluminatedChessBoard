#include "board.h"
#include <stdio.h>
#include <stdlib.h>

void printArrayInRows(const int* array, int size, int elementsPerRow) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);

        // Check if it's time to start a new row
        if ((i + 1) % elementsPerRow == 0) {
            printf("\n");  // Move to the next line for a new row
        }
    }

    // Print a newline if the last row is incomplete
    if (size % elementsPerRow != 0) {
        printf("\n");
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

typedef enum {
    IDLE,
    PICKED_UP,
    PLACED_DOWN,
} STATE;

int main(){
    //STATE state = IDLE;
    //int* moveArray;
    //initChessboard();
    int* moveArr;
    initChessboard();
    //initChessboardForTesting();
    moveArr = getPossibleMoves(1,1);
    printArrayInRows(moveArr, 8*8, 8);
    
    free(moveArr);
    //displayBoard();
    //movePiece(1,1,2,2);
    //displayBoard();
}