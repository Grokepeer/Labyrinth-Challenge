#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Node struct for the disjoint forest
typedef struct node node;
struct node {
    int rank;
    node *parent;
};

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

void updateWormNeck(worm *worm) {
    worm->neckCoords = worm->headCoords;
}

// Disjoint functions
node* makeSet();
node* findSet(node *node);
node* uniteSets(node *x_node, node *y_node);

// Other functions
// bool isConnected(int entryRow, int entryCol, int exitRow, int exitCol, int** labyrinth, int rows, int cols);
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

    // Creates main matrix (pointer of pointers to integers)
    int** readMatrix = malloc(rows * sizeof(int *));

    // Scans file for matrix init
    for (int rowCounter = 0; rowCounter < rows; rowCounter++) {

        char lineBuffer[cols + 1];
        fgets(lineBuffer, cols + 2, filePtr);    // Reads lines

        readMatrix[rowCounter] = malloc(cols * sizeof(int));  // Allocates rows

        bool endReached = false;    // Line termination flag

        for (int colCounter = 0; colCounter < cols; colCounter++) {
            if (lineBuffer[colCounter] == 'x' && !endReached) {     // If x or line terminated
                readMatrix[rowCounter][colCounter] = 1;
            } else {
                readMatrix[rowCounter][colCounter] = 0;  // Fills matrix with zeros where not walls
            }
            if (lineBuffer[colCounter] == '\n') {   // Waits for EOL to set the flag
                endReached = true;
            }
        }
    }

    // Print Matrix
    // printMatrix(readMatrix, rows, cols);

    // ----------------------------------------------------

    // Free memory
    for (int i = 0; i < rows; i++) {
        free(readMatrix[i]);
    }
    free(readMatrix);
    fclose(filePtr);

    // ----------------------------------------------------
    // Generator ------------------------------------------
    // ----------------------------------------------------

    matrixCell** generatedMatrix = malloc(sizeof(matrixCell *) * rows);

    labGenerator(generatedMatrix, rows, cols, entryRow, entryCol, exitRow, exitCol);

    // Print Matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", generatedMatrix[i][j].isWall ? 'x' : ' ');
        }
        printf("\n");
    }

    // ----------------------------------------------------

    // Free memory
    for (int i = 0; i < rows; i++) {
        free(generatedMatrix[i]);
    }
    free(generatedMatrix);

    return 0;
}

// bool isConnected(int entryRow, int entryCol, int exitRow, int exitCol, int** labyrinth, int rows, int cols)
// {
//     // Check if the entry and exit points are within the bounds of the labyrinth
//     if (entryRow < 1 || entryRow > rows || entryCol < 1 || entryCol > cols || exitRow < 1 || exitRow > rows || exitCol < 1 || exitCol > cols)
//     {
//         printf("Entry or exit point is out of bounds.\n");
//         return 500; // Not connected, out of bounds
//     }

//     // Check if the entry and exit points are walls (1)
//     if (labyrinth[entryRow - 1][entryCol - 1] == 1 || labyrinth[exitRow - 1][exitCol - 1] == 1)
//     {
//         printf("Entry or exit point is a wall.\n");
//         return 500; // Not connected, not even possible to calculate path
//     }

//     //create a disjoint set to keep track of connected components
//     disjoint_set_forest *ds = create_disjoint_set_forest(sizeof(int), node_int_cmp);
//     if (ds == NULL)
//     {
//         printf("Failed to create disjoint set forest\n");
//         return -1; // Failed to create disjoint set
//     }

//     for(int i = 0; i < rows; i++)
//     {
//         for(int j = 0; j < cols; j++)
//         {
//             // If the cell is not a wall (1), create a set for it
//             if(labyrinth[i][j] == 0)
//             {
//                 //printf("Creating set for cell (%d, %d)\n", i, j); //TODO: remove
//                 int *data = malloc(sizeof(int));
//                 *data = i * row + j; // Linearize the 2D coordinates and save in data
//                 int a = make_set(ds, data);
//                 //printf("Result of make_set: %d\n", a); //TODO: remove
//                 if(i != 0) //Upper check
//                 {
//                     //printf("Upper check\n"); //TODO: remove
//                     if(labyrinth[i-1][j] == 0)
//                     {
//                         disjoint_set_node *s1 = find(ds, data);
//                         int* data1 = malloc(sizeof(int));
//                         *data1 = (i-1) * row + j; // Linearize the 2D coordinates and save in data
//                         disjoint_set_node *s2 = find(ds, data1);
//                         set_union(ds, s1, s2);
//                     }
//                 }
//                 if(j != 0) //Left check
//                 {
//                     //printf("Left check\n"); //TODO: remove
//                     if(labyrinth[i][j-1] == 0)
//                     {
//                         disjoint_set_node *s1 = find(ds, data);
//                         int* data1 = malloc(sizeof(int));
//                         *data1 = i * row + (j-1); // Linearize the 2D coordinates and save in data
//                         disjoint_set_node *s2 = find(ds, data1);
//                         set_union(ds, s1, s2);
//                     }
//                 }
//             }
//         }
//     }
//     // Check if the entry and exit points belong to the same set
//     int *entry_data = malloc(sizeof(int));
//     *entry_data = (entryRow - 1) * row + (entryCol -1); // Linearize the 2D coordinates and save in data
//     disjoint_set_node *entry_set = find(ds, entry_data);
//     int *exit_data = malloc(sizeof(int));
//     *exit_data = (exitRow - 1) * row + (exitCol - 1); // Linearize the 2D coordinates and save in data
//     disjoint_set_node *exit_set = find(ds, exit_data);
//     int connected = (entry_set == exit_set) ? 1 : 0; // Check if they are in the same set using a tertiary operator
//     //printf("Root of entry set: %d\n", *(int *)entry_set->data); //TODO: remove
//     //printf("Root of exit set: %d\n", *(int *)exit_set->data); //TODO: remove
//     delete_disjoin_set_forest(ds); // Free the disjoint set forest
//     free(entry_data); // Free the allocated memory for entry_data
//     free(exit_data); // Free the allocated memory for exit_data
//     return connected;
// }

void labGenerator(matrixCell** matrix, unsigned int rows, unsigned int cols, unsigned int entryRow, unsigned int entryCol, unsigned int exitRow, unsigned int exitCol) {
    srand(time(NULL));

    for (int rowCounter = 0; rowCounter < rows; rowCounter++) {
        matrix[rowCounter] = malloc(sizeof(matrixCell) * cols);

        for (int colCounter = 0; colCounter < cols; colCounter++) {
            matrix[rowCounter][colCounter].isWall = 1;
        }
    }

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
        // printf("test2");
        for (int wormCounter = 0; wormCounter < wormPopulation; wormCounter++) {
            worm* wormPtr = wormList[wormCounter];
            vector wormDirection = {wormPtr->headCoords.row - wormPtr->neckCoords.row, wormPtr->headCoords.col - wormPtr->neckCoords.col};

            if (wormDirection.row == 0 && wormDirection.col == 0) {
                wormDirection.row = 1;
            }

            printf("%d %d\n", wormDirection.row, wormDirection.col);
            if (wormDirection.row * wormDirection.col != 0) {
                wormDirection.row = 1; wormDirection.col = 0;
            }
            
            int newComputedRow = -1, newComputedCol = -1;

            int tmp = rand() % 20;
            int switchCase;

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
                default:
                    break;
            }

            if (newComputedCol >= 0 && newComputedCol < cols && newComputedRow >= 0 && newComputedRow < rows) {
                updateWormNeck(wormPtr);
                wormPtr->headCoords.row = newComputedRow;
                wormPtr->headCoords.col = newComputedCol;
                // printf("%d %d\n", wormPtr->headCoords.row, wormPtr->headCoords.col);
                // printf("%d %d - %d %d\n", wormDirection.row, wormDirection.col, wormPtr->headCoords.row, wormPtr->headCoords.col);

                matrix[wormPtr->headCoords.row][wormPtr->headCoords.col].isWall = 0;

                if (wormPtr->headCoords.row == exitRow - 1 && wormPtr->headCoords.col == exitCol - 1) {
                    endReached = 1;
                }
            }
        }
    }

    for (int wormCounter = 0; wormCounter < wormPopulation; wormCounter++) {
        free(wormList[wormCounter]);
    }
    free(wormList);
}

// Creates a set with one node (root node)
node* makeSet() {
    node *new_set = malloc(sizeof(node));

    new_set->rank = 0;  // Base rank is 1
    new_set->parent = new_set;  // The parent node of the root node is itself

    return new_set;
}

// Given any node it finds its root node
node* findSet(node *nodePtr) {
    node *tmp1, *tmp2 = nodePtr;

    // Loops until the parent is equal to the node itself (found the root node)
    do {
        tmp1 = tmp2;
        tmp2 = tmp1->parent;
    } while (tmp1 != tmp2);

    return tmp1;
}

// Attaches two sets together
node* uniteSets(node *x_node, node *y_node) {
    node *x_base = findSet(x_node), *y_base = findSet(y_node);
    node *new_base, *old_base;

    // Checks which set is smaller and therefore has to be appended to the bigger one
    if (x_base->rank > y_base->rank) {
        new_base = x_base;
        old_base = y_base;
    } else {
        new_base = y_base;
        old_base = x_base;
    }

    old_base->parent = new_base;    // Attaches the smaller base to the bigger one
    
    if (old_base->rank == new_base->rank) { // If the tree grew in rank, it will update it
        new_base->rank += 1;
    }

    return new_base;
}