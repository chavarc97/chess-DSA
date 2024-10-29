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
typedef enum
{
    PAWN = 1,
    KNIGHT = 3,
    BISHOP = 3,
    TOWER = 5,
    QUEEN = 8,
    KING = 10
} PieceValue;

// Colors for the moves
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

    // Add diagonal pointers
    struct Square *northEast;
    struct Square *northWest;
    struct Square *southEast;
    struct Square *southWest;
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
typedef Square *(*MovementFunction)(Square *);

// Global variables
Move *head = NULL;

// FUNCTION PROTOTYPES >>>>>>>>>>>>>>>>>>>>>>>>
// Board management
Square *createBoard();
void setPieceValue(Square *board);
Square *findSquare(Square *board);
int isBoardValid(Square *board);

// Board Navigation
Square *moveWest(Square *s) { return s->west; }
Square *moveEast(Square *s) { return s->east; }
Square *moveNorth(Square *s) { return s->north; }
Square *moveSouth(Square *s) { return s->south; }
Square *moveNorthEast(Square *s) { return s->northEast; }
Square *moveNorthWest(Square *s) { return s->northWest; }
Square *moveSouthEast(Square *s) { return s->southEast; }
Square *moveSouthWest(Square *s) { return s->southWest; }

// File Management
void readBoardFromFile(Square *board, const char *filename);

// Move Management
void findMoves(Square *target, Move *head);
Move *createMove(char *coord, int value, MoveColor moveColor);
void addMove(Move *h, Move *m);
void traverseAndAddMoves(Square *start, Move *head, Square *(*nextSquare)(Square *), int moveColor);
void freeMoves(Move *head);

// Stack Management
void findTopMoves(Move *head, Stack **topMoves, Stack **auxStack);
void push(Stack **s, Move *m);
Move *pop(Stack **s);
Stack *initStack();
void printStack(Stack *s);
void freeStack(Stack *s);

// Display Functions
void printBoard(Square *board);
void printList(Move *head);
void printColorList(Move *move, int flag);

void freeBoard(Square **board);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int main()
{
    Square *board = createBoard();

    // Example usage
    readBoardFromFile(board, "A1.txt");
    printBoard(board);
    Square *target = findSquare(board);
    printf("\ntarget address: %p\nCoordinates: %s\nValue: %d \n", target, target->coordinates, target->value);
    findMoves(target, head);
    printList(head);

    // Step 3 create a stack with the 5 top moves
    // char tm[10] = {"topMoves"};

    Stack *topMoves = initStack();
    Stack *auxStack = initStack();
    findTopMoves(head, &topMoves, &auxStack);
    printStack(topMoves);

    // Free memory
    freeMoves(head);
    freeStack(topMoves);
    freeStack(auxStack);
    freeBoard(&board);
    return 0;
}

// Fuctions
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
            int index = row * 8 + col;
            board[index].coordinates[0] = 'A' + col;
            board[index].coordinates[1] = '8' - row;
            board[index].coordinates[2] = '\0';
            board[index].piece = '.';
            board[index].value = 0;
            board[index].isTarget = 0;
            board[index].north = (row > 0) ? &board[(row - 1) * 8 + col] : NULL; // if row > 0, set north to the square above
            board[index].south = (row < 7) ? &board[(row + 1) * 8 + col] : NULL; // if row < 7, set south to the square below
            board[index].east = (col < 7) ? &board[row * 8 + col + 1] : NULL;    // if col < 7, set east to the square to the right
            board[index].west = (col > 0) ? &board[row * 8 + col - 1] : NULL;    // if col > 0, set west to the square to the left
            // Add diagonal pointers
            // Diagonal connections
            board[index].northEast = (row > 0 && col < 7) ? &board[(row - 1) * 8 + col + 1] : NULL;
            board[index].northWest = (row > 0 && col > 0) ? &board[(row - 1) * 8 + col - 1] : NULL;
            board[index].southEast = (row < 7 && col < 7) ? &board[(row + 1) * 8 + col + 1] : NULL;
            board[index].southWest = (row < 7 && col > 0) ? &board[(row + 1) * 8 + col - 1] : NULL;
        }
    }
    // Return the board
    return board;
}

int isBoardValid(Square *board)
{
    return board != NULL;
}
// Print the board with connections
void printBoard(Square *board)
{
    if (!isBoardValid(board))
    {
        printf("Error: Cannot print board - board has been freed or is invalid\n");
        return;
    }
    printf("\nChess Board Configuration:\n\n");
    printf("    A   B   C   D   E   F   G   H\n"); // Added column labels at top
    printf("   --------------------------------\n");
    // Print the board with connections
    for (int row = 0; row < 8; row++)
    {
        printf("%d | ", 8 - row); // Added row labels at left
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
        printf("| %d\n", 8 - row); // Added row labels at right
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
    strcpy(fileRoute, route);     // copy of route in fileRoute
    strcat(fileRoute, filename);  // concatenate (./data/filename.txt)
    file = fopen(fileRoute, "r"); // Open file to read it

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

// This fuction set the value of each peace depending on the letters.
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

// Fuction to find the target square.
Square *findSquare(Square *board)
{
    for (int i = 0; i < 64; i++)
    {
        if (board[i].isTarget)
        {
            return &board[i];
        }
    }
    // At the end of the for return Null if theres no target.
    return NULL;
}

void findMoves(Square *target, Move *head)
{
    // Initialize 2 ptr to point to the target square and the linked list of moves
    Square *t_ptr = target;

    printf("Target square: %s\n", t_ptr->coordinates);
    // Based on the target location traverse the board orthogonally (Tower movements)
    if (t_ptr->piece == 'T')
    {
        traverseAndAddMoves(target->west, head, moveWest, M_RED);
        traverseAndAddMoves(target->east, head, moveEast, M_BLUE);
        traverseAndAddMoves(target->north, head, moveNorth, M_GREEN);
        traverseAndAddMoves(target->south, head, moveSouth, M_YELLOW);
    }
    // Based on the target location traverse the board diagonally (Bishop movements (A))
    if (t_ptr->piece == 'A')
    {
        traverseAndAddMoves(target->northEast, head, moveNorthEast, M_GREEN);
        traverseAndAddMoves(target->northWest, head, moveNorthWest, M_RED);
        traverseAndAddMoves(target->southEast, head, moveSouthEast, M_BLUE);
        traverseAndAddMoves(target->southWest, head, moveSouthWest, M_YELLOW);
    }
}

// Fuction that creates and return a new node "Move".
Move *createMove(char *coord, int value, MoveColor moveColor)
{ // Allocate memory for the move
    Move *newMove = (Move *)malloc(sizeof(Move));
    // if memory allocation fails
    if (newMove == NULL)
    {
        printf("Memory allocation failed\n");
        free(newMove);
        exit(1);
    }
    // Copy coordinates in the new node move
    strcpy(newMove->move, coord);
    // Set value
    newMove->value = value;
    // Next null so is not connected to the list yet.
    newMove->next = NULL;
    // Set move color
    newMove->moveColor = moveColor;
    return newMove;
}

// Fuction to add a move
void addMove(Move *h, Move *m)
{ // temporary is equal to head.
    Move *tmp = head;
    // If head is empty, the new node is the first on the list.
    if (!tmp)
        head = m;
    else
    { // Find the last node in the list.
        while (tmp->next != NULL)
            tmp = tmp->next;
        // Assign the new node to the next node of head.
        tmp->next = m;
    }
}

void printList(Move *head)
{
    // Assign tmp to head
    Move *tmp = head;
    // If head doesnt exist is empty
    if (tmp == NULL)
    {
        printf("List: empty\n");
        return;
    }
    printf("List of moves: ");
    // Until tmp isnt empty or without a node.
    while (tmp != NULL)
    {
        // print node with color
        printColorList(tmp, 0);
        // If the node is the last one.
        if (tmp->next == NULL)
            printf(RESET "\n");
        printf("%s", tmp->next != NULL ? ", " : "\n"); // print a coma if its not the last one.
        tmp = tmp->next;                               // Go to the next node to continue the cicle.
    }
}

void printColorList(Move *move, int moveOrStack)
{
    // Format
    int flag = moveOrStack;
    char *color;
    // Color assignment
    switch (move->moveColor)
    {
    case M_GREEN:
        color = GREEN;
        break;
    case M_RED:
        color = RED;
        break;
    case M_BLUE:
        color = BLUE;
        break;
    case M_YELLOW:
        color = YELLOW;
        break;
    default:
        color = RESET;
    }

    if (flag == 0)
        printf("%s%s-%d", color, move->move, move->value);
    else
        // Prints with line break
        printf("%s%s-%d\n", color, move->move, move->value);
}

// Fuction to add traverse moves.
void traverseAndAddMoves(Square *start, Move *head, Square *(*nextSquare)(Square *), int moveColor)
{ // Initialize in the first square.
    Square *current = start;

    // Until current is different to null
    while (current != NULL)
    {
        // Create a new move based in coordinates, value and color.
        Move *newMove = createMove(current->coordinates, current->value, moveColor);
        // Adding the move.
        addMove(head, newMove);
        // If the pice if different from . , then end the cicle.
        if (current->piece != '.')
        {
            break;
        }
        // Go to the next square.
        current = nextSquare(current);
    }
}

// Initialization of stack
Stack *initStack()
{
    // Allocate memory for the stack
    Stack *s = (Stack *)malloc(sizeof(Stack));
    // if memory allocation fails
    if (!s)
    {
        printf("\nMemory allocation failed\n");
        return NULL;
    }
    // Initialize top and prev as nulls.
    s->top = NULL;
    s->prev = NULL;
    return s;
}

void findTopMoves(Move *head, Stack **topMoves, Stack **auxStack)
{
    // from the list of moves, find the top 5 moves
    // iterate through the list of moves ignoring moves with value 0
    // when a move with value > 0 is found push it to the stack
    // if you found a move smaller than the top of the stack, pop the top from the stack and push it into the aux stack
    // push the new move into the stack and then push the top move from the aux stack back into the stack

    // Top stack aux---------------------------

    // Pointer to the head of the Stack
    Move *m_ptr = head;

    // Until it isnt null
    while (m_ptr != NULL)
    {
        // If the value is greater than cero.
        if (m_ptr->value > 0)
        { // before (while) -- after (if): avoid repeating verifications
            // If its empty it push the first move
            if ((*topMoves)->top == NULL)
            {
                push(topMoves, m_ptr);
            }
            else
            {
                // Until the pointer is less than the move in the stack.
                while ((*topMoves)->top != NULL && m_ptr->value < (*topMoves)->top->value)
                {
                    // Pop the moves at the principal stack and push them into the auxiliar stack.
                    Move *tmp = pop(topMoves);
                    push(auxStack, tmp);
                }
                // Pushthe move into the principal stack
                push(topMoves, m_ptr);
                while ((*auxStack)->top != NULL)
                { // Instead of (*auxStack) != NULL, use this so they cant have underflow.

                    // Now pop from the aux and push into the principal stack until aux is empty.
                    Move *tmp = pop(auxStack);
                    push(topMoves, tmp);
                }
            }
        }
        // Go to the next move
        m_ptr = m_ptr->next;
    }
}

void push(Stack **s, Move *m)
{
    // Allocate memory for the new node of stack
    Stack *newTop = (Stack *)malloc(sizeof(Stack));
    // if memory allocation fails
    if (!newTop)
    {
        printf("\nMemory allocation failed\n");
        return;
    }
    // Assign the movement to the top node
    newTop->top = m;
    // The prev node of the new node point to the stack.
    newTop->prev = *s;
    // Update the stack so that the new node is the new top.
    *s = newTop;
}

Move *pop(Stack **s)
{
    // If the stack is empty
    if ((*s)->top == NULL)
    {
        printf("\n Stack Underflow\n");
        printf("%p", s);
        return 0;
    }
    // Point to the node thats going to be erased
    Stack *temp = *s;
    // Make that the node point to the prev node.
    *s = (*s)->prev;
    // Save the movement at the top to return it.
    Move *val = temp->top;
    // Free memory
    free(temp);
    // Return erased move.
    return val;
}

void printStack(Stack *s)
{
    // If the stack is empty
    if (s == NULL)
    {
        printf("Stack is empty: no moves to display :(\n");
        return;
    }
    else if (s->top == NULL) // If the top is empty
    {
        printf("TOP MOVES:\nStack is empty: no moves to display :(\n");
        return;
    }
    {
        printf("\n-------------------------------\n");
        printf("TOP MOVES: \n");
        // check how many moves are in the stack
        int m_count = 0;
        Stack *tmp = s;
        while (tmp != NULL && tmp->top != NULL)
        {
            m_count++;
            tmp = tmp->prev; // Move to prev node.
        }
        // Step through the stack and show each move
        while (s != NULL && s->top != NULL)
        {
            printf("%d: ", m_count);   // Print number of moves
            printColorList(s->top, 1); // Print it with color.
            s = s->prev;               // Move to prev node
        }
    }
}

void freeMoves(Move *head)
{
    // Free memory of all the nodes in the list.
    Move *current = head;
    while (current != NULL)
    {
        Move *temp = current;
        current = current->next; // Go to next node
        free(temp);              // Free node
    }
}

void freeStack(Stack *s)
{
    // Free memory of all the nodes at the stack
    while (s != NULL)
    {
        Stack *temp = s;
        s = s->prev; // Go to prev
        free(temp);  // Free actual node.
    }
}

void freeBoard(Square **board)
{
    if (board == NULL || *board == NULL)
    {
        return;
    }

    // Free the board
    free(*board);
    // Set the pointer to NULL to prevent use after free
    *board = NULL;
}
