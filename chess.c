#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants
FILE *file = NULL;
#define MAX_SQUARES 64
#define MAX_ROW_COL 8
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

// define piece values
#define PAWN 1
#define KNIGHT 3
#define BISHOP 3
#define TOWER 5
#define QUEEN 8
#define KING 10

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
void setPieceValue(Square *board);

// main function
int main()
{
    testBoard();
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

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            board[row * 8 + col].coordinates[0] = 'A' + col;
            board[row * 8 + col].coordinates[1] = '1' + row;
            board[row * 8 + col].coordinates[2] = '\0';
            board[row * 8 + col].piece = '.';
            board[row * 8 + col].value = 0;
            board[row * 8 + col].isTarget = 0;
            board[row * 8 + col].north = (row > 0) ? &board[(row - 1) * 8 + col] : NULL; // if row > 0, set north to the square above
            board[row * 8 + col].south = (row < 7) ? &board[(row + 1) * 8 + col] : NULL; // if row < 7, set south to the square below
            board[row * 8 + col].east = (col < 7) ? &board[row * 8 + col + 1] : NULL;    // if col < 7, set east to the square to the right
            board[row * 8 + col].west = (col > 0) ? &board[row * 8 + col - 1] : NULL;    // if col > 0, set west to the square to the left
        }
    }

    return board;
}

// Print the board with connections
void printBoard(Square *board)
{
    printf("\nChess Board Configuration:\n\n");
    printf("    A   B   C   D   E   F   G   H\n"); // Added column labels at top
    // Print the board with connections
    for (int row = 0; row < 8; row++)
    {
        printf("%d | ", 1 + row); // Added row labels at left
        for (int col = 0; col < 8; col++)
        {
            Square *current = &board[row * 8 + col];
            // check if the square is a target
            if (current->isTarget)
            {
                printf(GREEN "%c  " RESET, current->piece);
            }
            else if (current->isTarget == 0 && current->piece != '.')
            {
                printf(RED "%c  " RESET, current->piece);
            }
            else
            {
                printf("%c  ", current->piece);
            }
            if (col < 7)
            {
                printf(" ");
            }
        }
        printf("%d | ", 1 + row); // Added row labels at right
        printf("\n");
        if (row < 7)
        {
            printf("  ");
            printf("\n");
        }
    }
    printf("    A   B   C   D   E   F   G   H\n"); // Added column labels at bottom
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

    // Reset all target flags first
    for (int i = 0; i < 64; i++)
    {
        board[i].isTarget = 0;
    }

    // Read board configuration
    int row = 0;
    char line[18];

    // Read the board layout first (8 rows)
    while (row < 8 && fgets(line, 18, file) != NULL)
    {
        for (int col = 0; col < 8; col++)
        {
            int index = row * 8 + col;
            board[index].piece = line[col * 2];
            if (board[index].piece != '.')
            {
                setPieceValue(&board[index]);
            }
        }
        row++;
    }

    // Read the target coordinate (e.g., "B2")
    char targetCoord[4]; // Increased size to handle newline
    if (fgets(targetCoord, sizeof(targetCoord), file) != NULL)
    {
        // Remove newline if present
        targetCoord[strcspn(targetCoord, "\n")] = 0;

        // Convert the coordinate to row and column
        /* The lines `int targetRow = targetCoord[1] - '1';` and `int targetCol = targetCoord[0] -
        'A';` are converting the target coordinate from a character representation to a 0-based
        index for row and column respectively. */
        int targetRow = targetCoord[1] - '1';
        int targetCol = targetCoord[0] - 'A';

        // Debug print
        printf("Target coordinate: %s (row: %d, col: %d)\n",
               targetCoord, targetRow + 1, targetCol + 1);

        // Mark the target square
        if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8)
        {
            int targetIndex = targetRow * 8 + targetCol;
            board[targetIndex].isTarget = 1;
            printf("Marking target at index: %d\n", targetIndex);
        }
    }

    // Close the file
    fclose(file);
}

void testBoard()
{
    Square *board = createBoard();

    // Example usage
    readBoardFromFile(board, "T3.txt");
    printBoard(board);

    free(board);
}

void setPieceValue(Square *board)
{
    switch (board->piece)
    {
    case 'P':
        board->value = PAWN;
        break;
    case 'A':
        board->value = BISHOP;
        break;
    case 'C':
        board->value = KNIGHT;
        break;
    case 'T':
        board->value = TOWER;
        break;
    case 'Q':
        board->value = QUEEN;
        break;
    case 'K':
        board->value = KING;
        break;
    }
}