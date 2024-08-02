#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define boolean int
#define false 0
#define true 1

int threadRow = 0;
int threadCol = 0;
boolean result = false;
pthread_mutex_t lockCol;
pthread_mutex_t lockRow;

typedef struct {
    char** puzzle;
    int size;
} Puzzle;

typedef struct {
    Puzzle* puzzle;
    char* word;
} findWordArgs;

boolean solve(Puzzle* puzzle, int row, int col, char* wordToFind, int letterNo);

int getRow(){
    pthread_mutex_lock(&lockRow);
    int ret = threadRow;
    threadRow++;
    pthread_mutex_unlock(&lockRow);

    return ret;
}

int getCol(){
    pthread_mutex_lock(&lockCol);
    int ret = threadCol;
    threadCol++;
    pthread_mutex_unlock(&lockCol);

    return ret;
}

int nextRow(int row, int direction, int size){
    // Directions are as follows
    /*
    * 7 0 1
    * 6 - 2
    * 5 4 3
    */
    if (direction == 0 || direction == 1 || direction == 7){
        return (row - 1 + size) % size;
    }
    else if (direction >= 3 && direction <= 5){
        return (row + 1) % size;
    }
    else {
        return row;
    }
}

int nextCol(int col, int direction, int size){
    if (direction >= 1 && direction <= 3){
        return (col + 1) % size;
    }
    else if (direction >= 5 && direction <= 7){
        return (col - 1 + size) % size;
    }
    else {
        return col;
    }
}

boolean valid(int row, int col, Puzzle* puzzle){
    boolean withinLimits = (row >= 0) && (col >= 0) && (row < puzzle->size) && (col < puzzle->size);
    boolean isLower = puzzle->puzzle[row][col] >= 'a' && puzzle->puzzle[row][col] <= 'z';

    if (withinLimits && isLower)
        return true;

    return false;
}

void* findWord(void* args){
    Puzzle* puzzle = ((findWordArgs*)args)->puzzle;
    char* word = ((findWordArgs*)args)->word;

    for (int i = getRow(); i < puzzle->size; i = getRow()){
        for (int j = getCol(); j < puzzle->size; j = getCol()){
            if (puzzle->puzzle[i][j] == word[0]){
                puzzle->puzzle[i][j] -= 32;

                if (solve(puzzle, i, j, word, 1)){
                    result = true;
                }
                else {
                    puzzle->puzzle[i][j] += 32;
                }
            }
        }
        if (getCol() >= puzzle->size){
            pthread_mutex_lock(&lockCol);
            threadCol = 0;
            pthread_mutex_unlock(&lockCol);
        }
    }

    return NULL;
}

boolean solve(Puzzle* puzzle, int row, int col, char* wordToFind, int letterNo){
    if (letterNo == strnlen(wordToFind, puzzle->size * puzzle->size)){
        return true;
    }

    for (int i = 0; i < 8; i++){
        int nRow = nextRow(row, i, puzzle->size);
        int nCol = nextCol(col, i, puzzle->size);

        if (valid(nRow, nCol, puzzle) && puzzle->puzzle[nRow][nCol] == wordToFind[letterNo]){
            puzzle->puzzle[nRow][nCol] -= 32;

            if (solve(puzzle, nRow, nCol, wordToFind, letterNo+1)){
                return true;
            }

            puzzle->puzzle[nRow][nCol] += 32;
        }
    }

    return false;
}

Puzzle* readPuzzle(char* file){
    FILE* readIn = fopen(file, "r");
    char tmp;
    int size = 0;

    fscanf(readIn, "%d", &size);
    fseek(readIn, 2, SEEK_CUR);

    char** pzl = calloc(sizeof(char*), size);

    for (int i = 0; i < size; i++){
        pzl[i] = calloc(sizeof(char), size);
    }

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            fread(&pzl[i][j], sizeof(char), 1, readIn);
        }
        fseek(readIn, 2, SEEK_CUR);
    }

    Puzzle* puzzle = calloc(sizeof(Puzzle), 1);
    puzzle->puzzle = pzl;
    puzzle->size = size;

    fclose(readIn);
    return puzzle;
}

void printPuzzle(Puzzle* puzzle){
    for (int i = 0; i < puzzle->size; i++){
        for (int j = 0; j < puzzle->size; j++){
            printf("%c", puzzle->puzzle[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv){
    Puzzle* pzl = readPuzzle(argv[1]);
    char* f = argv[2];
    int numThreads = atoi(argv[3]);

    findWordArgs* args = calloc(sizeof(findWordArgs), 1);
    args->puzzle = pzl;
    args->word = f;

    pthread_mutex_init(&lockRow, NULL);
    pthread_mutex_init(&lockCol, NULL);

    pthread_t threads[numThreads];
    for (int i = 0; i < numThreads; i++) {
        pthread_create(threads+i, NULL, findWord, args);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (result){
        printf("%s was found!\n", f);
        printPuzzle(pzl);
    }
    else {
        printf("%s was not found\n", f);
    }

    free(pzl);
    return 0;
}