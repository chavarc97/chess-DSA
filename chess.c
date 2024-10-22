#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants
FILE *file = NULL;
#define MAX_SQUARES 64
#define MAX_ROW_COL 8

// ANSI color codes
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"
// define piece values
#define PAWN 1
#define KNIGHT 3
#define BISHOP 3
#define TOWER 5
#define QUEEN 8
#define KING 10

// colors for the moves
typedef enum
{
    M_GREEN = 1,
    M_RED,
    M_BLUE,
    M_YELLOW
} MoveColor;


// Data structures
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
    int moveColor;
    struct Move *next;
} Move;

typedef struct Stack
{
    Move *top;
    struct Stack *prev;
} Stack;

// Function pointers for movement
typedef Square* (*MovementFunction)(Square*);

// Global variables
Move *head = NULL;

// function prototypes >>>>>>>>>>>>>>>>>>>>>>>>
// Board management
Square *createBoard();
void setPieceValue(Square *board);
Square *findSquare(Square *board);
// Board Navigation
Square* moveWest(Square *s) { return s->west; }
Square* moveEast(Square *s) { return s->east; }
Square* moveNorth(Square *s) { return s->north; }
Square* moveSouth(Square *s) { return s->south; }
// File Management
void readBoardFromFile(Square *board, const char *filename);
// Move Management
void findMoves(Square *target, Move *head);
Move *createMove(char *coord, int value, MoveColor moveColor);
void addMove(Move *h, Move *m);
void traverseAndAddMoves(Square *start, Move *head, Square* (*nextSquare)(Square *), int moveColor);
// Stack Management

// Display Functions
void printBoard(Square *board);
void printList(Move *head);
void printColorList(Move *move);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int main()
{
    Square *board = createBoard();
    // Example usage
    readBoardFromFile(board, "T3.txt");
    printBoard(board);
    Square *target = findSquare(board);
    printf("\ntarget address: %p\nCoordinates: %s\nValue: %d \n", target, target->coordinates, target->value);
    findMoves(target, head);
    printList(head);
    // Step 3 create a stack with the 5 top moves
    // Free memory
    free(board);
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
            /*
             * The logic behind the following lines is to set the coordinates of each square on the board.
             * The coordinates are set in the format of "A1", "A2", "A3", ..., "H8".
             * The ASCII value of 'A' is 65, and the ASCII value of '1' is 49.
             * So by implementing [row * 8 + col] we are able to set the coordinates of each square on the board.
             * For example, if row = 0 and col = 0, the coordinates of the square will be "A1" or if seen as a 1D array, it will be board[0].
             * Then by incrementing the row and col, we are able to set the coordinates of the rest of the squares on the board.
             */
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
    printf("   --------------------------------\n");
    // Print the board with connections
    for (int row = 0; row < 8; row++)
    {
        printf("%d | ", 1 + row); // Added row labels at left
        for (int col = 0; col < 8; col++)
        {
            Square *current = &board[row * 8 + col];
            // check if the square is a target
            if (current->isTarget)
                printf(GREEN "%c  " RESET, current->piece);
            else if (current->piece != '.')
                printf(RED "%c  " RESET, current->piece);
            else
                printf("%c  ", current->piece);
            if (col < 7)
                printf(" ");
        }
        printf("| %d\n", 1 + row); // Added row labels at right
        if (row < 7)
            printf("  \n");
    }
    printf("   ________________________________\n");
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

    /*
     * The following while loop is reading the board layout from the file.
     * The loop will continue until it reaches the end of the file or until it has read 8 rows.
     * The fgets function reads a line from the file and stores it in the variable line.
     * The loop then iterates through each character in the line and sets the piece value of the square on the board.
     * The piece value is set by calling the setPieceValue function.
     * The loop then increments the row variable to move to the next row.
     */
    int row = 0;
    char line[18];
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

        // Mark the target square
        if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8)
        {
            int targetIndex = targetRow * 8 + targetCol;
            board[targetIndex].isTarget = 1;
        }
    }

    // Close the file
    fclose(file);
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

Square *findSquare(Square *board)
{
    for (int i = 0; i < 64; i++)
    {
        if (board[i].isTarget)
        {
            return &board[i];
        }
    }
    return NULL;
}

void findMoves(Square *target, Move *head)
{
    // initialize 2 ptr to point to the target square and the linked list of moves
    Square *t_ptr = target;

    printf("Target square: %s\n", t_ptr->coordinates);
    // based on the target location traverse the board orthogonally (Tower movements)
    traverseAndAddMoves(target->west, head, moveWest, M_RED);
    traverseAndAddMoves(target->east, head, moveEast, M_BLUE);
    traverseAndAddMoves(target->north, head, moveNorth, M_GREEN);
    traverseAndAddMoves(target->south, head, moveSouth, M_YELLOW);
}

Move *createMove(char *coord, int value, MoveColor moveColor)
{
    Move *newMove = (Move *)malloc(sizeof(Move));
    if (newMove == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }
    strcpy(newMove->move, coord);
    newMove->value = value;
    newMove->next = NULL;
    newMove->moveColor = moveColor;
    return newMove;
}

void addMove(Move *h, Move *m)
{
    Move *tmp = head;
    if (!tmp)
        head = m;
    else
    {
        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = m;
    }
}

void printList(Move *head)
{
    Move *tmp = head;
    printf("List of moves: ");
    while (tmp != NULL)
    {
        printColorList(tmp);
        if (tmp->next == NULL)
            printf(RESET"\n");
        printf("%s", tmp->next!=NULL ? ", " : "\n");
        tmp = tmp->next;
    }
}

void printColorList(Move *move)
{
    char *color;
    switch (move->moveColor) {
        case M_GREEN:  color = GREEN;  break;
        case M_RED:    color = RED;    break;
        case M_BLUE:   color = BLUE;   break;
        case M_YELLOW: color = YELLOW; break;
        default:          color = RESET;
    }
    printf("%s%s-%d", color, move->move, move->value);
}

void traverseAndAddMoves(Square *start, Move *head, Square* (*nextSquare)(Square *), int moveColor) 
{
    Square *current = start;

    while (current != NULL) {
        Move *newMove = createMove(current->coordinates, current->value, moveColor);
        addMove(head, newMove);

        if (current->piece != '.') {
            break;
        }
        current = nextSquare(current);
    }
}