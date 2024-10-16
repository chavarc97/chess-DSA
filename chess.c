#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants
FILE *file = NULL;
#define MAX_SQUARES 64

// structs
typedef struct Square
{
    // TODO: COORDIMATES OF THE SQUARE
    char piece;
    int value;
    struct Square *north;
    struct Square *south;
    struct Square *east;
    struct Square *west;
} Square;

typedef struct Move
{
    char move[3];
    int value;
    struct Move *next;
} Move;

typedef struct Stack 
{
    Move *top;
    Move *prev;   
} Stack;

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
        // pruaba commit sofia 
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

void readBoardFromFile(Square *board, const char *filename)
{
    // Open File
    char route[100] = "./data/";
    char fileRoute[100] = strcat(route, filename);
    file = fopen(fileRoute, "r");

    // Check if file exists
    if(file == NULL)
    {
        printf("File not found\n");
        return;
    }

    // Read the board configuration from the file

    // Modify the board configuration

    // Close the file
}
