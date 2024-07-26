#include <stdio.h>
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <stdbool.h>

#define GRAVITY 0.5f


typedef struct cube {
    int pos_x;
    int pos_y;
    float vel_x;
    float vel_y;
    int width;
    int height;
    bool jump;
} Cube;

// Works fine now
Cube* EMSCRIPTEN_KEEPALIVE create_cube() {
    Cube* cube = malloc(sizeof(Cube));
    cube->pos_x = 50;
    cube->pos_y = 400;
    cube->width = 100;
    cube->height = 100;
    cube->vel_x = 5;
    cube->vel_y = 5;
    bool jump = false;

    return cube;
}

// Giving the pointers to get the values out 
void EMSCRIPTEN_KEEPALIVE get_coords(Cube* cube, int* px, int* py) {
    *px = cube->pos_x;
    *py = cube->pos_y;
}

void EMSCRIPTEN_KEEPALIVE update_cube(Cube* cube, float time) {
    cube->vel_y += GRAVITY;
    cube->pos_y += cube->vel_y;
    cube->pos_x += cube->vel_x;

    if (cube->pos_y > 400) {
        cube->pos_y = 400;
        cube->vel_y = 0;
        cube->jump = false;
    }

    if (cube->pos_x < 5 || cube->pos_x > 900) cube->vel_x *= -1;
}

void EMSCRIPTEN_KEEPALIVE start_jump(Cube* cube) {
    if (!cube->jump) {
        cube->vel_y = -12.0f;
        cube->jump = true;
    }
}

void EMSCRIPTEN_KEEPALIVE end_jump(Cube* cube) {
    if (cube->vel_y < -6.0) cube->vel_y = -6.0;
}