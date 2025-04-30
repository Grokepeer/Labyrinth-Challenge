#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Node struct for the disjoint forest
typedef struct {
    int rank;
    struct node *parent;
} node;

typedef struct {
    int isWall;
} matrixCell;

typedef struct {
    int rowHead;
    int colHead;
    int rowNeck;
    int colNeck;
} worm;

typedef struct {
    int row;
    int col;
} vector;

// Disjoint functions
node* makeSet();
node* findSet(node *node);
node* uniteSets(node *x_node, node *y_node);

// Other functions
// bool isConnected(int entryRow, int entryCol, int exitRow, int exitCol, int** labyrinth, int rows, int cols);
void printMatrix(int **matrix, int rows, int cols);
int** initMatrix(int rows, int cols);
void labGenerator(matrixCell **matrix, unsigned int rows, unsigned int cols, unsigned int entryRow, unsigned int entryCol, unsigned int exitRow, unsigned int exitCol);
void updateWormNeck(worm *worm);

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

        //printf("%s", lineBuffer);

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
    printf("hello");
    printMatrix(readMatrix, rows, cols);
    printf("second hello");

    // ----------------------------------------------------

    // Free memory
    for (int i = 0; i < rows; i++) {
        free(readMatrix[i]);
    }
    free(readMatrix);
    fclose(filePtr);

    // Generator ------------------------------------------

    matrixCell** generatedMatrix = malloc(sizeof(matrixCell *) * rows);

    srand(time(NULL));

    printf("hello");

    labGenerator(generatedMatrix, rows, cols, entryRow, entryCol, exitRow, exitCol);

    printMatrix(generatedMatrix, rows, cols);

    return 0;

    // struct Node *ptr = make_set();

    // int rows = 20, cols = 20;
    // bool **matrix = initMatrix(rows, cols);

    // printMatrix(matrix, rows, cols);

    // // Free allocated memory
    // for (int i = 0; i < rows; i++) {
    //     free(matrix[i]);
    // }
    // free(matrix);

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
    
    for (int rowCounter = 0; rowCounter < rows; rowCounter++) {
        matrix[rowCounter] = malloc(sizeof(matrixCell) * cols);

        for (int colCounter = 0; colCounter < cols; colCounter++) {
            printf("test0");
            matrix[rowCounter][colCounter].isWall = 1;
        }
    }

    int endReached = 0;
    int wormPopulation = 1;

    worm **wormList = malloc(sizeof(worm *) * wormPopulation);

    for (int wormCounter = 0; wormCounter < wormPopulation; wormCounter++) {
        wormList[wormCounter] = (worm *)malloc(sizeof(worm));
        wormList[wormCounter]->rowHead = entryRow;
        wormList[wormCounter]->colHead = entryCol;
        wormList[wormCounter]->rowNeck = entryRow - 1;
        wormList[wormCounter]->colNeck = entryCol - 1;
    }

    printf("test1");

    while (!endReached) {
        printf("test2");
        for (int wormCounter = 0; wormCounter < wormPopulation; wormCounter++) {
            vector goAhead = {wormList[wormCounter]->rowHead - wormList[wormCounter]->rowNeck, wormList[wormCounter]->colHead - wormList[wormCounter]->colNeck};

            switch (rand() % 5) {
                case 0: // Goes forwards
                    updateWormNeck(wormList[wormCounter]);
                    wormList[wormCounter]->rowHead += goAhead.row;
                    wormList[wormCounter]->colHead += goAhead.col;
                    break;
                case 1: // Goes backwards
                    updateWormNeck(wormList[wormCounter]);
                    wormList[wormCounter]->rowHead += goAhead.row * -1;
                    wormList[wormCounter]->colHead += goAhead.col * -1;
                    break;
                case 2: // Goes rightward
                    updateWormNeck(wormList[wormCounter]);
                    wormList[wormCounter]->rowHead += goAhead.col;
                    wormList[wormCounter]->colHead += goAhead.row;
                    break;
                case 3: // Goes leftward
                    updateWormNeck(wormList[wormCounter]);
                    wormList[wormCounter]->rowHead += goAhead.col * -1;
                    wormList[wormCounter]->colHead += goAhead.row * -1;
                    break;
                default:
                    break;
            }

            matrix[wormList[wormCounter]->rowHead][wormList[wormCounter]->colHead].isWall = 0;

            if (wormList[wormCounter]->rowHead == exitRow && wormList[wormCounter]->colHead == exitCol) {
                endReached = 1;
            }
        }
    }


}

void updateWormNeck(worm *worm) {
    worm->rowNeck = worm->rowHead;
    worm->colNeck = worm->colHead;
}

// Prints out to screen the given matrix
void printMatrix(int **matrix, int rows, int cols) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", matrix[i][j] ? 'x' : ' ');
        }
        printf("\n");
    }
}

int** initMatrix(int rows, int cols) {
    int **matrix = malloc(rows * sizeof(int *));

    for (int i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 1;
        }
    }

    return matrix;
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