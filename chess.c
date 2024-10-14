#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants

// structs
typedef struct Square
{
    char piece;
    int value;
    struct Square *north;
    struct Square *south;
    struct Square *east;
    struct Square *west;
} Square;

// function prototypes
Square *createBoard();
void readBoardFromFile(Square *board, const char *filename);
void printBoard(Square *board);
void findMoves(Square *board, int row, int col);
void pushMove(char *move, int value);
void printStack();

// main function
int main()
{
    Square *board = createBoard();

    char filename[100];

    printf("Enter the name of the file: ");
    scanf("%s", filename);

    readBoardFromFile(board, filename);
    printBoard(board);

    // TODO: Implement stages 2 and 3

    // Clean up
    // TODO: Free the memory allocated for the board

    return 0;
}

// TODO: Implement functions here
