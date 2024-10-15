#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants
FILE *file = NULL;
#define MAX_SQUARES 64
#define MAX_MOVES 100
#define MAX_FILE_NAME 100

// structs ---------------------->
typedef struct Square
{
    char piece;
    int value;
    struct Square *north;
    struct Square *south;
    struct Square *east;
    struct Square *west;
} Square;

// Stack to store moves
typedef struct Move
{
    /* data */
    char move[5];
    int value;
    Move *prev;
} Move;

Move move_stack[MAX_MOVES];
// <---------------------- structs

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

    char filename[MAX_FILE_NAME];

    // printf("Enter the name of the file: ");
    // scanf("%s", filename);

    // readBoardFromFile(board, filename);
    printBoard(board);

    // TODO: Implement stages 2 and 3

    // Clean up
    // TODO: Free the memory allocated for the board

    return 0;
}

// TODO: Implement functions here
Square *createBoard()
{
    // Allocate memory for the board
    Square *board = (Square *)malloc(sizeof(Square) * 64);
    // if memory allocation fails
    if (board == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Initialize the board with empty squares "."
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        board[i].piece = '.';
        board[i].north = (i >= 8) ? &board[i - 8] : NULL;
        board[i].south = (i < 56) ? &board[i + 8] : NULL;
        board[i].east = (i % 8 != 7) ? &board[i + 1] : NULL;
        board[i].west = (i % 8 != 0) ? &board[i - 1] : NULL;
    }

    return board;
}

// Print the board with connections
void printBoard(Square *board)
{
    printf("\nChess Board Configuration:\n\n");
    for (int row = 0; row < 8; row++)
    {
        // Print row number
        printf("%d | ", 8 - row);
        // Print pieces and horizontal connections
        for (int col = 0; col < 8; col++)
        {
            Square *current = &board[row * 8 + col];
            printf("%c", current->piece);
            if (col < 7)
            {
                printf("  ");
            }
        }
        printf("\n");
        // Print vertical connections
        if (row < 7)
        {
            printf("  ");
            printf("\n");
        }
    }
}
