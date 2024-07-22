#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>


typedef struct _dinosaur {
    int pos_x;
    int pos_y;
    float velocity;
} dinosaur;

void EMSCRIPTEN_KEEPALIVE struct_create(dinosaur **ppStruct) {
    dinosaur *pStruct = malloc(sizeof(dinosaur));
    
    pStruct->pos_x = 35;
    pStruct->pos_y = 28;

    *ppStruct = pStruct;
}

void EMSCRIPTEN_KEEPALIVE struct_destroy(dinosaur* myStruct) {
    free(myStruct);
}

void EMSCRIPTEN_KEEPALIVE struct_print(dinosaur* myStruct) {
    printf("%d, %d\n", myStruct->pos_x, myStruct->pos_y);
}

void EMSCRIPTEN_KEEPALIVE struct_query(dinosaur* myStruct, int *px, int *py) {
    *px = myStruct->pos_x;
    *py = myStruct->pos_y;
}

void EMSCRIPTEN_KEEPALIVE struct_set(dinosaur* myStruct, int x, int y) {
    myStruct->pos_x = x;
    myStruct->pos_y = y;
}

int main() {
    printf("Hello, World!");
    return 0;
}

