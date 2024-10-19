#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants
FILE *file = NULL;
#define MAX_SQUARES 64
#define MAX_ROW_COL 8
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

// structs
typedef struct Square
{
    char coordinates[3]; // A1, A2, A3, ..., H8
    char piece;
    int value;
    int isTarget; // 1 if the square is a target, 0 otherwise
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
void testBoard();

// main function
int main()
{
    testBoard();
    // Square *board = createBoard();

    // char filename[100];

    // printf("Enter the name of the file: ");
    // scanf("%s", filename);

    // printBoard(board);
    // readBoardFromFile(board, filename);
    // printBoard(board);

    // TODO: Implement stages 2 and 3

    // Clean up
    // TODO: Free the memory allocated for the board
    // free(board);
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
    // Initialize the board with empty squares and coordinates
    for (int row = 0; row < MAX_ROW_COL; row++)
    {
        for (int col = 0; col < MAX_ROW_COL; col++)
        {
            int index = row * 8 + col;
            // Set coordinates (e.g., "A1", "B2", etc.)
            board[index].coordinates[0] = 'A' + col; // File (A-H)
            board[index].coordinates[1] = '1' + row; // Rank (1-8)
            board[index].coordinates[2] = '\0';      // Null terminator

            // Set piece and connections
            board[index].piece = '.';
            board[index].isTarget = 0;
            board[index].north = (row > 0) ? &board[(row - 1) * 8 + col] : NULL;
            board[index].south = (row < 7) ? &board[(row + 1) * 8 + col] : NULL;
            board[index].east = (col < 7) ? &board[row * 8 + (col + 1)] : NULL;
            board[index].west = (col > 0) ? &board[row * 8 + (col - 1)] : NULL;
        }
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
        printf("%d | ", 1 + row);
        // Print pieces and horizontal connections
        for (int col = 0; col < 8; col++)
        {
            Square *current = &board[row * 8 + col];
            if (current->isTarget)
            {
                printf(GREEN "%c" RESET, current->piece);  // Print in green if target
            } else {
                printf("%c", current->piece);
            }
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
    printf("    A  B  C  D  E  F  G  H\n"); // Added column labels at bottom
}

void readBoardFromFile(Square *board, const char *filename)
{
    // Open File
    char route[100] = "./data/";
    char fileRoute[100];
    strcpy(fileRoute, route);    // copy of route in fileRoute
    strcat(fileRoute, filename); // concatenate (./data/filename.txt)
    file = fopen(fileRoute, "r");

    // Check if file exists
    if (file == NULL)
    {
        printf("File not found\n");
        return;
    }
    int i = 0; // position
    int j = 0; // row
    char line[18];
    // Read the board configuration from the file  and modify the board configuration

    while (fgets(line, 18, file) != NULL && i < MAX_SQUARES)
    {                // Fgets read the lines < 18 chars
        int col = 0; // Column
        while (col < 8)
        {
            board[i].piece = line[col * 2]; // skip the spaces with *2
            col++;
            i++;
        }
        j++;
    }

    // Close the file
    fclose(file);
}

void testBoard()
{
    Square *board = createBoard();
    
    // Example usage
    readBoardFromFile(board, "A1.txt");
    printBoard(board);
    
    free(board);
}