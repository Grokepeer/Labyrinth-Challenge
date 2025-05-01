#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Node struct for the disjoint set
typedef struct node node;
struct node {
    int rank;   // If >0 gives the rank of the path, else it will be -1 which indicates a wall, ignore *parent if wall
    node *parent;
};

// Struct for generated matrix cells
typedef struct {
    int isWall;
} matrixCell;

typedef struct {
    int row;
    int col;
} vector;

typedef struct {
    vector headCoords;
    vector neckCoords;
} worm;

// Disjoint functions
node* findSet(node *node);
void uniteSets(node *x_node, node *y_node);

// Other functions
bool isConnected(node **matrix, int entryRow, int entryCol, int exitRow, int exitCol, int rows, int cols);
void labGenerator(matrixCell **matrix, unsigned int rows, unsigned int cols, unsigned int entryRow, unsigned int entryCol, unsigned int exitRow, unsigned int exitCol);

int main() {

    // Open file
    FILE *filePtr = fopen("labyrinth", "r");
    if (filePtr == NULL) {  // Check for file
        printf("No file");
        return 500; // Returns client error
    }

    // Extracts from file all formatted metadata
    unsigned int rows, cols, entryRow, entryCol, exitRow, exitCol;
    fscanf(filePtr, "%d %d\n%d %d\n%d %d", &rows, &cols, &entryRow, &entryCol, &exitRow, &exitCol);
    {   //Discards a single line
        char lineBuffer[cols + 1];
        fgets(lineBuffer, cols + 2, filePtr);
    }

    // Creates main matrix (pointer of pointers to integers)
    node** readMatrix = malloc(rows * sizeof(node *));

    // Scans file for matrix init
    for (int rowCounter = 0; rowCounter < rows; rowCounter++) {

        char lineBuffer[cols + 1];
        memset(lineBuffer, 0, (cols + 1) * sizeof(char));   // Resets all values to zero so that if the buffer doesn't refill all the spaces it won't pass past values forward
        fgets(lineBuffer, cols + 2, filePtr);    // Reads lines

        readMatrix[rowCounter] = malloc(cols * sizeof(node));  // Allocates rows

        bool endReached = false;    // Line termination flag

        for (int colCounter = 0; colCounter < cols; colCounter++) {
            readMatrix[rowCounter][colCounter].parent = &readMatrix[rowCounter][colCounter];    // Init parent pointer to itself

            if (lineBuffer[colCounter] == '\n') {   // Waits for EOL to set the flag
                endReached = true;
            }
            if (lineBuffer[colCounter] == 'x' && !endReached) {     // If x or line terminated
                readMatrix[rowCounter][colCounter].rank = -1;
            } else {
                readMatrix[rowCounter][colCounter].rank = 0;  // Fills matrix with zeros where not walls
            }

            printf("%d %d %d %d\n", rowCounter, colCounter, readMatrix[rowCounter][colCounter].rank, endReached);
        }
    }

    // Print Matrix of the read labyrinth
    printf("The readMatrix from file is:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", (readMatrix[i][j].rank == -1) ? 'x' : '.');
        }
        printf("\n");
    }

    // Checks if the read matrix is connected
    if (isConnected(readMatrix, entryRow, entryCol, exitRow, exitCol, rows, cols)) {
        printf("Connection check result:\n -> The labyrinth is indeed connected\n");
    } else {
        printf("Connection check result:\n -> The labyrinth is unfortunately NOT connected\n");
    }

    // Free memory
    for (int i = 0; i < rows; i++) {
        free(readMatrix[i]);
    }
    free(readMatrix);
    fclose(filePtr);

    // ----------------------------------------------------
    // Generator ------------------------------------------
    // ----------------------------------------------------

    // Creates and populates the new labyrinth
    matrixCell** generatedMatrix = malloc(sizeof(matrixCell *) * rows);
    labGenerator(generatedMatrix, rows, cols, entryRow, entryCol, exitRow, exitCol);

    // Print Matrix of the new labyrinth
    printf("\nThe generated labyrinth is:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", generatedMatrix[i][j].isWall ? 'x' : '.');
        }
        printf("\n");
    }

    // Free memory
    for (int i = 0; i < rows; i++) {
        free(generatedMatrix[i]);
    }
    free(generatedMatrix);

    return 0;
}

// Checks if a labyrinth is connected
bool isConnected(node **matrix, int entryRow, int entryCol, int exitRow, int exitCol, int rows, int cols) {
    // Check if the entry and exit points are within the bounds of the labyrinth
    if (entryRow < 1 || entryRow > rows || entryCol < 1 || entryCol > cols || exitRow < 1 || exitRow > rows || exitCol < 1 || exitCol > cols)
    {
        printf("Entry or exit point is out of bounds.\n");
        return 500; // Not connected, out of bounds
    }

    // Check if the entry and exit points are walls (1)
    if (matrix[entryRow - 1][entryCol - 1].rank == -1 || matrix[exitRow - 1][exitCol - 1].rank == -1)
    {
        printf("Entry or exit point is a wall.\n");
        return 500; // Not connected, not even possible to calculate path
    }

    // For each path in the matrix it unites sets with the adjacent ones
    for (int rowCounter = 0; rowCounter < rows; rowCounter++) {
        for (int colCounter = 0; colCounter < cols; colCounter++) {
            // Checks left
            if (colCounter > 0 && matrix[rowCounter][colCounter].rank != -1 && matrix[rowCounter][colCounter - 1].rank != -1) {
                uniteSets(&matrix[rowCounter][colCounter], &matrix[rowCounter][colCounter - 1]);
            }
            // Checks up
            if (rowCounter > 0 && matrix[rowCounter][colCounter].rank != -1 && matrix[rowCounter - 1][colCounter].rank != -1) {
                uniteSets(&matrix[rowCounter][colCounter], &matrix[rowCounter - 1][colCounter]);
            }
        }
    }

    // Checks that the roots of the entry and exit point are the same -> they are connected
    if (findSet(&matrix[entryRow - 1][entryCol - 1]) == findSet(&matrix[exitRow - 1][exitCol - 1])) {
        return true;
    } else {
        return false;
    }
}

// Generates a libyrinth given some params
void labGenerator(matrixCell** matrix, unsigned int rows, unsigned int cols, unsigned int entryRow, unsigned int entryCol, unsigned int exitRow, unsigned int exitCol) {
    srand(time(NULL));  // Init needed for rand() function

    // Init all matrix cells to walls
    for (int rowCounter = 0; rowCounter < rows; rowCounter++) {
        matrix[rowCounter] = malloc(sizeof(matrixCell) * cols); // Allocates space for rows

        for (int colCounter = 0; colCounter < cols; colCounter++) {
            matrix[rowCounter][colCounter].isWall = 1;
        }
    }

    // Allocates space for all the worms
    int wormPopulation = 2;
    worm **wormList = malloc(sizeof(worm *) * wormPopulation);

    // Init all worms to the entry point in the labyrinth
    for (int wormCounter = 0; wormCounter < wormPopulation; wormCounter++) {
        wormList[wormCounter] = (worm *)malloc(sizeof(worm));
        (wormList[wormCounter]->headCoords).row = entryRow - 1;
        (wormList[wormCounter]->headCoords).col = entryCol - 1;
        wormList[wormCounter]->neckCoords = wormList[wormCounter]->headCoords;
    }

    matrix[entryRow - 1][entryCol - 1].isWall = 0;  //Sets the starting point as empty

    // Until a worm has reached the exit point of the labyrinth all worms will be updated here
    int endReached = 0;
    while (!endReached) {
        // For each worm, update
        for (int wormCounter = 0; wormCounter < wormPopulation; wormCounter++) {
            worm* wormPtr = wormList[wormCounter];  // Saves the pointer to the current worm, for semplicity
            vector wormDirection = {wormPtr->headCoords.row - wormPtr->neckCoords.row, wormPtr->headCoords.col - wormPtr->neckCoords.col};  // Calculates the vector of the worm heading

            // If the worm stalls for at least one step, the heading will be a zero vector, it needs to receive a new random heading or it will remain stuck
            if (wormDirection.row == 0 && wormDirection.col == 0) {
                wormDirection.row = 1;
            }

            // Checks for potential diagonal vector results (the worm cannot move diagonally)
            if (wormDirection.row * wormDirection.col != 0) {
                wormDirection.row = 1; wormDirection.col = 0;
            }
            
            int newComputedRow = -1, newComputedCol = -1;   // Creates variables for new computed position of the head, init to impossible values

            int tmp = rand() % 20;  // Generates a random number between 0 and 19
            int switchCase;

            // This block is used to "allocate probability" for each movement direction or switchCase value, read below to know which direction each value corrisponds to
            if (tmp < 8) switchCase = 0;
            else if (tmp < 8) switchCase = 1;
            else if (tmp < 10) switchCase = 2;
            else if (tmp < 12) switchCase = 3;
            else switchCase = 4;

            switch (switchCase) {
                case 0: // Goes forwards
                    newComputedRow = wormPtr->headCoords.row + wormDirection.row;
                    newComputedCol = wormPtr->headCoords.col + wormDirection.col;
                    break;
                case 1: // Goes backwards
                    newComputedRow = wormPtr->headCoords.row + wormDirection.row * -1;
                    newComputedCol = wormPtr->headCoords.col + wormDirection.col * -1;
                    break;
                case 2: // Goes rightward
                    newComputedRow = wormPtr->headCoords.row + wormDirection.col;
                    newComputedCol = wormPtr->headCoords.col + wormDirection.row;
                    break;
                case 3: // Goes leftward
                    newComputedRow = wormPtr->headCoords.row + wormDirection.col * -1;
                    newComputedCol = wormPtr->headCoords.col + wormDirection.row * -1;
                    break;
                default:    // Stays put, doesn't move
                    break;
            }

            // Checks for plausability of the new computed position before moving
            if (newComputedCol >= 0 && newComputedCol < cols && newComputedRow >= 0 && newComputedRow < rows) {
                wormPtr->neckCoords = wormPtr->headCoords;    // Updates the neck position to the current head position
                wormPtr->headCoords.row = newComputedRow;
                wormPtr->headCoords.col = newComputedCol;

                matrix[wormPtr->headCoords.row][wormPtr->headCoords.col].isWall = 0;    // The new head position is liberated

                // Checks if the worm has reached the end of the labyrinth
                if (wormPtr->headCoords.row == exitRow - 1 && wormPtr->headCoords.col == exitCol - 1) {
                    endReached = 1;
                }
            }
        }
    }

    // Free memory
    for (int wormCounter = 0; wormCounter < wormPopulation; wormCounter++) {
        free(wormList[wormCounter]);
    }
    free(wormList);
}

// Given any node it finds its root node
node* findSet(node *nodePtr) {
    // Recursive search
    if (nodePtr != nodePtr->parent) {
        return findSet(nodePtr->parent);
    } else {
        return nodePtr;
    }

    // Non-recursive alternative for search algorithm in case useful
    // node *tmp1, *tmp2 = nodePtr;

    // // Loops until the parent is equal to the node itself (found the root node)
    // do {
    //     tmp1 = tmp2;
    //     tmp2 = tmp1->parent;
    // } while (tmp1 != tmp2);
    // return tmp1;
}

// Attaches two sets together
void uniteSets(node *x_node, node *y_node) {
    node *x_base = findSet(x_node), *y_base = findSet(y_node);  // Finds the node's roots

    // Checks which set is smaller and therefore has to be appended to the bigger one
    if (x_base->rank > y_base->rank) {
        y_base->parent = x_base;
    } else if (x_base->rank < y_base->rank) {
        x_base->parent = y_base;
    } else {
        y_base->parent = x_base;
        x_base->rank++;
    }
}