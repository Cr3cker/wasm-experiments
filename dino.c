#include <stdio.h>
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>

#define DINO_FRAMES_NUM 2
#define INIT_DINO_X 40.0f
#define INIT_DINO_Y 40.0f

typedef enum {
    DINO_RUN1,
    DINO_RUN2,
    GROUND, 
    ASSET_COUNT
} AssetName;

typedef struct {
    Vector2 size;
    Vector2 pos;
    Texture2D dino_frame_loop[DINO_FRAMES_NUM];
    Rectangle dino_source_rect;
} Dino;

typedef struct {
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
} Cactus;

typedef struct {
    Vector2 pos;
    Vector2 vel;
} Ground;


const char *asset_paths[ASSET_COUNT] = {
    "./assets/dino_run1.png",
    "./assets/dino_run2.png",
    "./assets/ground.png"
};

Dino* create_dino() {
    Dino *dino = malloc(sizeof(Dino));
    Texture2D dino_run1 = LoadTexture(asset_paths[DINO_RUN1]);
    Texture2D dino_run2 = LoadTexture(asset_paths[DINO_RUN2]);
    Vector2 pos = { INIT_DINO_X, INIT_DINO_Y };
    Vector2 size = { dino_run1.width, dino_run1.height };
    Rectangle dino_source_rect = {0, 0, dino_run1.width, dino_run1.height };
    dino->dino_frame_loop[0] = dino_run1;
    dino->dino_frame_loop[1] = dino_run1;
    dino->pos = pos;
    dino->size = size;

    return dino;
}

int main() {
    const int window_width = 800;
    const int window_height = 600;

    InitWindow(window_width, window_height, "Raylib Dino");
    InitAudioDevice();

    SetTargetFPS(120);

    // Texture2D dino_run1 = LoadTexture("./assets/dino_run1.png");
    // Texture2D dino_run2 = LoadTexture("./assets/dino_run2.png");
    Texture2D ground = LoadTexture("./assets/ground.png");

    // Rectangle dino_source = { 0, 0, dino_run1.width, dino_run1.height };

    int frames_counter = 0;
    int current_frame = 0;
    int current_line = 0;

    // Texture2D frame_loop[2];
    // frame_loop[0] = dino_run1;
    // frame_loop[1] = dino_run2;

    // Vector2 dino_position = { 30.0f, 30.0f };

    Dino *dino = create_dino();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        frames_counter++;

        if (frames_counter > 10) {
            current_frame++;

            if (current_frame >= 2) {
                current_frame = 0;
            }

            frames_counter = 0;
        }

        dino_source.x = frame_loop[current_frame].width;

        DrawTextureRec(frame_loop[current_frame], dino_source, dino_position, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}