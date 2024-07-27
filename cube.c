#include <stdio.h>
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <stdbool.h>

#define GRAVITY 0.5f
#define CUBE_WIDTH 100
#define CUBE_HEIGHT 100
#define CUBE_INIT_X 50
#define CUBE_INIT_Y 400
#define CUBE_VEL_X 3.0f
#define CUBE_VEL_Y 5.0f
#define GND 450
#define VELOCITY_BOOST -15.0f
#define VELOCITY_LIMIT -6.0f

typedef struct cube {
    int pos_x;
    int pos_y;
    float vel_x;
    float vel_y;
    int width;
    int height;
    bool jump;
} Cube;

typedef struct obstacle {
    int width;
    int height;
    int pos_x;
    int pos_y;
    float vel_x;
} Obstacle;

Cube* EMSCRIPTEN_KEEPALIVE create_cube() {
    Cube* cube = malloc(sizeof(Cube));
    cube->pos_x = CUBE_INIT_X;
    cube->pos_y = CUBE_INIT_Y;
    cube->width = CUBE_WIDTH;
    cube->height = CUBE_HEIGHT;
    cube->vel_x = CUBE_VEL_X;
    cube->vel_y = CUBE_VEL_Y;
    cube->jump = false;

    return cube;
}

Obstacle* EMSCRIPTEN_KEEPALIVE generate_obstacle() {
    Obstacle* obs = malloc(sizeof(Obstacle));
    obs->pos_x = 1100;
    obs->pos_y = 411;
    obs->width = 50;
    obs->height = 150;
    obs->vel_x = -3.0f;

    return obs;
}

void EMSCRIPTEN_KEEPALIVE get_coords_cube(Cube* cube, int* px, int* py) {
    *px = cube->pos_x;
    *py = cube->pos_y;
}

void EMSCRIPTEN_KEEPALIVE get_coords_obs(Obstacle* obs, int* ox, int* oy) {
    *ox = obs->pos_x;
    *oy = obs->pos_y;
}

void EMSCRIPTEN_KEEPALIVE update_frame(Cube* cube, Obstacle* obs, float time) {
    cube->vel_y += GRAVITY;
    cube->pos_y += cube->vel_y;
    obs->pos_x += obs->vel_x;

    if (cube->pos_y > GND) {
        cube->pos_y = GND;
        cube->vel_y = 0;
        cube->jump = false;
    }
}

void EMSCRIPTEN_KEEPALIVE start_jump(Cube* cube) {
    if (!cube->jump) {
        cube->vel_y = VELOCITY_BOOST;
        cube->jump = true;
    }
}

void EMSCRIPTEN_KEEPALIVE end_jump(Cube* cube) {
    if (cube->vel_y < VELOCITY_LIMIT) cube->vel_y = VELOCITY_LIMIT;
}