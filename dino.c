#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdlib.h>

#define DINO_FRAMES_NUM 2
#define INIT_DINO_X 30.0f
#define INIT_DINO_Y 500.0f
#define GROUND_VEL_X -5.0f
#define INIT_GROUND_Y 575.0f
#define GRAVITY 2500.0f

typedef enum {
    DINO_RUN1,
    DINO_RUN2,
    GROUND, 
    ASSET_COUNT
} AssetName;

typedef struct {
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
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
    Texture2D ground_frame;
    Rectangle ground_source_rect;
} Ground;


const char *asset_paths[ASSET_COUNT] = {
    "./assets/dino_run1.png",
    "./assets/dino_run2.png",
    "./assets/ground.png"
};


void move_ground(Ground *ground) {
    ground->pos = Vector2Add(ground->pos, ground->vel);
    
    if (ground->pos.x <= -ground->ground_frame.width) {
        ground->pos.x = 0;
    }
}

void dino_update(Dino *dino, float time) {
    Vector2 scaled_vel = Vector2Scale(dino->vel, time);
    dino->pos = Vector2Add(dino->pos, scaled_vel);
    dino->vel.y += GRAVITY * time;

    if (dino->pos.y >= 500) {
        dino->pos.y = 500;
        dino->vel.y = 0;
    }
}

void jump_key_press(Dino *dino) {
    if (dino->pos.y >= 500) dino->vel.y = -1000.0f;
}

void jump_key_release(Dino *dino) {
    if (dino->vel.y < -50.0f) dino->vel.y = -50.0f;
}

Ground* create_ground() {
    Ground *ground = malloc(sizeof(Ground));
    Texture2D ground_sprite = LoadTexture(asset_paths[GROUND]);
    Vector2 vel = { GROUND_VEL_X, 0.0f };
    Vector2 pos = { 0.0f, INIT_GROUND_Y };
    Rectangle ground_source_rect = {0, 0, ground_sprite.width, ground_sprite.height };
    
    ground->ground_frame = ground_sprite;
    ground->vel = vel;
    ground->pos = pos;
    ground->ground_source_rect = ground_source_rect;

    return ground;
}

Dino* create_dino() {
    Dino *dino = malloc(sizeof(Dino));
    Texture2D dino_run1 = LoadTexture(asset_paths[DINO_RUN1]);
    Texture2D dino_run2 = LoadTexture(asset_paths[DINO_RUN2]);
    Vector2 pos = { INIT_DINO_X, INIT_DINO_Y };
    Vector2 size = { dino_run1.width, dino_run1.height };
    Vector2 vel = { 0.0f, 0.0f };
    Rectangle dino_source_rect = {0, 0, dino_run1.width, dino_run1.height };

    dino->dino_source_rect = dino_source_rect;
    dino->dino_frame_loop[0] = dino_run1;
    dino->dino_frame_loop[1] = dino_run2;
    dino->pos = pos;
    dino->size = size;
    dino->vel = vel;

    return dino;
}

int main() {
    const int window_width = 800;
    const int window_height = 600;

    InitWindow(window_width, window_height, "Raylib Dino");
    InitAudioDevice();

    SetTargetFPS(120);

    int frames_counter = 0;
    int current_frame = 0;
    int current_line = 0;
    int key_pressed;

    Dino *dino = create_dino();
    Ground *ground = create_ground();

    while (!WindowShouldClose()) {
        move_ground(ground);

        if (IsKeyPressed(KEY_SPACE)) {
            jump_key_press(dino);
        }

        if (IsKeyReleased(KEY_SPACE)) {
            jump_key_release(dino);
        }

        dino_update(dino, GetFrameTime());

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

        dino->dino_source_rect.x = current_frame * dino->dino_frame_loop[current_frame].width;

        DrawTextureRec(dino->dino_frame_loop[current_frame], dino->dino_source_rect, dino->pos, WHITE);
        DrawTextureRec(ground->ground_frame, ground->ground_source_rect, ground->pos, WHITE);
        DrawTextureRec(ground->ground_frame, ground->ground_source_rect, (Vector2){ground->pos.x + ground->ground_frame.width, ground->pos.y}, WHITE);

        EndDrawing();
    }

    free(dino);
    free(ground);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}