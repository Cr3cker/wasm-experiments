#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define DINO_FRAMES_NUM 2
#define INIT_DINO_X 30.0f
#define INIT_DINO_Y 500.0f
#define GROUND_VEL_X -5.0f
#define INIT_GROUND_Y 575.0f
#define GRAVITY 2500.0f
#define CACTUS_TYPES 3
#define CACTUS_SPAWN_INTERVAL 300
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef enum {
    CACTUS1,
    CACTUS2,
    CACTUS3,
    DINO_RUN1,
    DINO_RUN2,
    GROUND,
    RETRY_BUTTON,
    RETRY_TEXT,
    ASSET_COUNT
} AssetName;

typedef struct {
    Texture2D button_asset;
    Texture2D text_asset;
    Vector2 button_pos;
    Vector2 text_pos;
} Retry;

typedef struct {
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
    Texture2D dino_frame_loop[DINO_FRAMES_NUM];
    Rectangle dino_source_rect;
    Rectangle dino_pos_rect;
} Dino;

typedef struct {
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
    Rectangle cactus_source_rect;
    Texture2D asset;
    Rectangle cactus_pos_rect;
} Cactus;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Texture2D ground_frame;
    Rectangle ground_source_rect;
} Ground;


const char *asset_paths[ASSET_COUNT] = {
    "./assets/cactus_1.png",
    "./assets/cactus_2.png",
    "./assets/cactus_3.png",
    "./assets/dino_run1.png",
    "./assets/dino_run2.png",
    "./assets/ground.png",
    "./assets/retry_button.png",
    "./assets/retry_text.png"
};


void move_ground(Ground *ground) {
    ground->pos = Vector2Add(ground->pos, ground->vel);
    
    if (ground->pos.x <= -ground->ground_frame.width) {
        ground->pos.x = 0;
    }
}

void move_cactus(Cactus *cactus) {
    cactus->pos = Vector2Add(cactus->pos, cactus->vel);

    cactus->cactus_pos_rect.x = cactus->pos.x;
    cactus->cactus_pos_rect.y = cactus->pos.y;
}

void dino_update(Dino *dino, float time) {
    Vector2 scaled_vel = Vector2Scale(dino->vel, time);
    dino->pos = Vector2Add(dino->pos, scaled_vel);
    dino->vel.y += GRAVITY * time;

    if (dino->pos.y >= 500) {
        dino->pos.y = 500;
        dino->vel.y = 0;
    }

    dino->dino_pos_rect.x = dino->pos.x;
    dino->dino_pos_rect.y = dino->pos.y;
}

void jump_key_press(Dino *dino) {
    if (dino->pos.y >= 500) dino->vel.y = -1000.0f;
}

void jump_key_release(Dino *dino) {
    if (dino->vel.y < -50.0f) dino->vel.y = -50.0f;
}

Retry* create_retry_texture() {
    Retry *retry = malloc(sizeof(Retry));
    Texture2D button_asset = LoadTexture(asset_paths[RETRY_BUTTON]);
    Vector2 button_pos = { WINDOW_WIDTH / 2 - button_asset.width / 2,  WINDOW_HEIGHT / 2 - button_asset.height / 2 };
    Texture2D text_asset = LoadTexture(asset_paths[RETRY_TEXT]);
    Vector2 text_pos = { WINDOW_WIDTH / 2 - text_asset.width / 2,  WINDOW_HEIGHT / 2 - text_asset.height / 2 - 70};

    retry->button_asset = button_asset;
    retry->button_pos = button_pos;
    retry->text_asset = text_asset;
    retry->text_pos = text_pos;

    return retry;
}

Cactus* create_cactus() {
    Cactus *cactus = malloc(sizeof(Cactus));
    int random = rand() % CACTUS_TYPES;
    Texture2D asset = LoadTexture(asset_paths[random]);
    Vector2 size = { asset.width, asset.height };
    Rectangle cactus_source_rect = { 0, 0, asset.width, asset.height };
    Vector2 pos = { 1000.0f, INIT_GROUND_Y - asset.height + 20};
    Vector2 vel = { -5.0f, 0.0f };
    Rectangle cactus_pos_rect = { pos.x, pos.y, asset.width, asset.height };

    cactus->asset = asset;
    cactus->size = size;
    cactus->cactus_source_rect = cactus_source_rect;
    cactus->pos = pos;
    cactus->vel = vel;
    cactus->cactus_pos_rect = cactus_pos_rect;

    return cactus;
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
    Vector2 size = { dino_run1.width, dino_run1.height };
    Vector2 vel = { 0.0f, 0.0f };
    Vector2 pos = { INIT_DINO_X, INIT_DINO_Y };
    Rectangle dino_source_rect = {0, 0, dino_run1.width, dino_run1.height };
    Rectangle dino_pos_rect = { INIT_DINO_X, INIT_DINO_Y, dino_run1.width, dino_run1.height };

    dino->dino_source_rect = dino_source_rect;
    dino->dino_pos_rect = dino_pos_rect;
    dino->dino_frame_loop[0] = dino_run1;
    dino->dino_frame_loop[1] = dino_run2;
    dino->size = size;
    dino->pos = pos;
    dino->vel = vel;

    return dino;
}

void unload_and_free(Dino *dino, Cactus *cactus, Retry *retry, Ground *ground) {
    for (int i = 0; i < DINO_FRAMES_NUM; i++) {
        UnloadTexture(dino->dino_frame_loop[i]);
    }
    UnloadTexture(ground->ground_frame);
    UnloadTexture(retry->button_asset);
    UnloadTexture(retry->text_asset);
    if (cactus != NULL) {
        UnloadTexture(cactus->asset);
    }

    free(cactus);
    free(retry);
    free(dino);
    free(ground);
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Dino");
    InitAudioDevice();
    srand(time(NULL));

    SetTargetFPS(120);

    int frames_counter = 0;
    int current_frame = 0;
    int cactus_spawn_counter = 0;
    bool game_over = false;

    Dino *dino = create_dino();
    Ground *ground = create_ground();
    Cactus *cactus = NULL;
    Retry *retry = create_retry_texture();

    while (!WindowShouldClose()) {
        if (!game_over) {
            move_ground(ground);

            if (cactus != NULL) {
                move_cactus(cactus);
            }

            if (IsKeyPressed(KEY_SPACE)) {
                jump_key_press(dino);
            }

            if (IsKeyReleased(KEY_SPACE)) {
                jump_key_release(dino);
            }

            if (cactus_spawn_counter >= CACTUS_SPAWN_INTERVAL) {
                    cactus = create_cactus();
                cactus_spawn_counter = 0;
            }

            if (cactus != NULL && CheckCollisionRecs(dino->dino_pos_rect, cactus->cactus_pos_rect)) {
                game_over = true;
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

            if (cactus != NULL) {
                DrawTextureRec(cactus->asset, cactus->cactus_source_rect, cactus->pos, WHITE);
            }

            cactus_spawn_counter++;
        } else {
            DrawTexture(retry->button_asset, retry->button_pos.x, retry->button_pos.y, WHITE);
            DrawTexture(retry->text_asset, retry->text_pos.x, retry->text_pos.y, WHITE);
        }

        EndDrawing();
    }

    unload_and_free(dino, cactus, retry, ground);

    // TODO: Optimize game by loading cactus assets only once
    // TODO: Handle retry button to start a new game (basically just switch game_over = false)
    
    CloseAudioDevice();
    CloseWindow();

    return 0;
}