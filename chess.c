#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants
FILE *file = NULL;

// structs ---------------------->
// ANSI escape codes for colors
#define RED "\033[1;31m"    // Bright red
#define RESET "\033[0m"     // Reset to default color
#define BOLD "\033[1m"      // Bold text
#define YELLOW "\033[1;33m" // Bright yellow (alternative option)
#define MAX_ROW_COL 8
typedef struct Square
{
    char coordinates[3];
    char piece;
    int value;
    int isTarget;        // Flag for target square
    struct Square *north;
    struct Square *south;
    struct Square *east;
    struct Square *west;
} Square;
// <---------------------- structs

// function prototypes
void printBoard(Square *board);
void readBoardFromFile(Square *board, const char *filename);
void verifyTarget(Square *board);
Square *createBoard();

// main function
int main()
{
    Square *board = createBoard();
    
    // Read the board file
    readBoardFromFile(board, "A1.txt");
    
    // Verify target was set correctly
    verifyTarget(board);
    
    // Print the board
    printBoard(board);
    
    free(board);
    return 0;
}

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

void printBoard(Square *board)
{
    printf("\nChess Board Configuration:\n\n");
    printf("    A  B  C  D  E  F  G  H\n");
    for (int row = 0; row < 8; row++)
    {
        printf("%d | ", 1 + row);
        for (int col = 0; col < 8; col++)
        {
            Square *current = &board[row * 8 + col];
            if (current->isTarget)
            {
                // Print the piece in red with bold
                printf(BOLD RED "%c" RESET, current->piece);
            }
            else
            {
                printf("%c", current->piece);
            }
            if (col < 7)
            {
                printf("  ");
            }
        }
        printf(" | %d", 1 + row);
        printf("\n");
        if (row < 7)
        {
            printf("  ");
            printf("\n");
        }
    }
    printf("    A  B  C  D  E  F  G  H\n");
}

void readBoardFromFile(Square *board, const char *filename)
{
    char route[100] = "./data/";
    char fileRoute[100];
    strcpy(fileRoute, route);
    strcat(fileRoute, filename);
    file = fopen(fileRoute, "r");

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
        }
        row++;
    }

    // Read the target coordinate (e.g., "B2")
    char targetCoord[4];  // Increased size to handle newline
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
               targetCoord, targetRow +1, targetCol +1);

        // Mark the target square
        if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8)
        {
            int targetIndex = targetRow * 8 + targetCol;
            board[targetIndex].isTarget = 1;
            printf("Marking target at index: %d\n", targetIndex);
        }
    }

    fclose(file);
}

// Test function to verify target marking
void verifyTarget(Square *board)
{
    printf("\nVerifying target squares:\n");
    for (int i = 0; i < 64; i++)
    {
        if (board[i].isTarget)
        {
            printf("Found target at index %d with piece '%c'\n", i, board[i].piece);
        }
    }
}
