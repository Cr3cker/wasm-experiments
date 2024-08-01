#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define DINO_FRAMES_NUM 2
#define INIT_DINO_X 30.0f
#define INIT_DINO_Y 500.0f
#define GROUND_VEL_X -10.0f
#define INIT_GROUND_Y 575.0f
#define GRAVITY 2500.0f
#define CACTUS_TYPES 3
#define CACTUS_SPAWN_INTERVAL 300
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

Texture2D *textures;
Sound *sounds;

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

typedef enum {
    DIE,
    JUMP,
    POINT,
    SOUND_COUNT
} SoundName;

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
    Rectangle dino_pos_rect;
} Dino;

typedef struct {
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
    Texture2D asset;
    Rectangle cactus_pos_rect;
} Cactus;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Texture2D ground_frame;
} Ground;

void load_textures(const char *asset_paths[]) {
    textures = malloc(ASSET_COUNT * sizeof(Texture2D));
    for (int i = 0; i < ASSET_COUNT; i++) {
        textures[i] = LoadTexture(asset_paths[i]);
    }
}

void load_sounds(const char *sound_paths[]) {
    sounds = malloc(SOUND_COUNT * sizeof(Sound));
    for (int i = 0; i < SOUND_COUNT; i++) {
        sounds[i] = LoadSound(sound_paths[i]);
    }
}

void render_common(Dino *dino, Ground *ground, Cactus *cactus, int current_frame) {
    DrawTexture(dino->dino_frame_loop[current_frame], dino->pos.x, dino->pos.y, WHITE);
    DrawTexture(ground->ground_frame, ground->pos.x, ground->pos.y, WHITE);
    DrawTexture(ground->ground_frame, ground->pos.x + ground->ground_frame.width, ground->pos.y, WHITE);

    if (cactus != NULL) {
        DrawTexture(cactus->asset, cactus->pos.x, cactus->pos.y, WHITE);
    }
}

void render_game(Dino *dino, Ground *ground, Cactus *cactus, int current_frame) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    render_common(dino, ground, cactus, current_frame);

    EndDrawing();
}

void render_game_over(Dino *dino, Ground *ground, Cactus *cactus, Retry *retry, int current_frame) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    render_common(dino, ground, cactus, current_frame);

    DrawTexture(retry->button_asset, retry->button_pos.x, retry->button_pos.y, WHITE);
    DrawTexture(retry->text_asset, retry->text_pos.x, retry->text_pos.y, WHITE);

    EndDrawing();
}

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
    PlaySound(sounds[JUMP]);
    if (dino->pos.y >= 500) dino->vel.y = -1000.0f;
}

void jump_key_release(Dino *dino) {
    if (dino->vel.y < -50.0f) dino->vel.y = -50.0f;
}

Retry* create_retry_texture() {
    Retry *retry = malloc(sizeof(Retry));
    Vector2 button_pos = {
        WINDOW_WIDTH / 2 - textures[RETRY_BUTTON].width / 2,
        WINDOW_HEIGHT / 2 - textures[RETRY_BUTTON].height / 2
    };
    Vector2 text_pos = {
        WINDOW_WIDTH / 2 - textures[RETRY_TEXT].width / 2,
        WINDOW_HEIGHT / 2 - textures[RETRY_TEXT].height / 2 - 70
    };

    retry->button_asset = textures[RETRY_BUTTON];
    retry->button_pos = button_pos;
    retry->text_asset = textures[RETRY_TEXT];
    retry->text_pos = text_pos;

    return retry;
}

Cactus* create_cactus() {
    Cactus *cactus = malloc(sizeof(Cactus));
    int random = rand() % CACTUS_TYPES;
    Vector2 size = { textures[random].width, textures[random].height };
    Vector2 pos = { 1000.0f, INIT_GROUND_Y - textures[random].height + 20};
    Vector2 vel = { -10.0f, 0.0f };
    Rectangle cactus_pos_rect = { pos.x, pos.y, textures[random].width, textures[random].height + 20 };

    cactus->asset = textures[random];
    cactus->size = size;
    cactus->pos = pos;
    cactus->vel = vel;
    cactus->cactus_pos_rect = cactus_pos_rect;

    return cactus;
}

Ground* create_ground() {
    Ground *ground = malloc(sizeof(Ground));
    Vector2 vel = { GROUND_VEL_X, 0.0f };
    Vector2 pos = { 0.0f, INIT_GROUND_Y };
    
    ground->ground_frame = textures[GROUND];
    ground->vel = vel;
    ground->pos = pos;

    return ground;
}

Dino* create_dino() {
    Dino *dino = malloc(sizeof(Dino));
    Vector2 size = { textures[DINO_RUN1].width, textures[DINO_RUN1].height };
    Vector2 vel = { 0.0f, 0.0f };
    Vector2 pos = { INIT_DINO_X, INIT_DINO_Y };
    Rectangle dino_pos_rect = { INIT_DINO_X, INIT_DINO_Y, textures[DINO_RUN1].width - 5, textures[DINO_RUN1].height - 5 };

    dino->dino_pos_rect = dino_pos_rect;
    dino->dino_frame_loop[0] = textures[DINO_RUN1];
    dino->dino_frame_loop[1] = textures[DINO_RUN2];
    dino->size = size;
    dino->pos = pos;
    dino->vel = vel;

    return dino;
}

void unload_and_free(Dino *dino, Cactus *cactus, Retry *retry, Ground *ground) {
    for (int i = 0; i < DINO_FRAMES_NUM; i++) {
        UnloadTexture(dino->dino_frame_loop[i]);
    }
    for (int i = 0; i < SOUND_COUNT; i++) {
        UnloadSound(sounds[i]);
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

void update(Ground* ground, Cactus *cactus, Dino *dino, float time) {
    dino_update(dino, time);

    if (cactus != NULL) {
        move_cactus(cactus);
    }

    move_ground(ground);
}

void reset_game(Dino *dino, Ground *ground, Cactus **cactus, int *frames_counter, int *current_frame, int *cactus_spawn_counter, bool *game_over) {
    dino->pos = (Vector2){INIT_DINO_X, INIT_DINO_Y};
    dino->vel = (Vector2){0.0f, 0.0f};
    dino->dino_pos_rect.x = INIT_DINO_X;
    dino->dino_pos_rect.y = INIT_DINO_Y;

    ground->pos = (Vector2){0.0f, INIT_GROUND_Y};
    ground->vel = (Vector2){GROUND_VEL_X, 0.0f};

    if (*cactus != NULL) {
        free(*cactus);
        *cactus = NULL;
    }

    *frames_counter = 0;
    *current_frame = 0;
    *cactus_spawn_counter = 0;
    *game_over = false;
}

void restart_game(Retry *retry, Dino *dino, Ground *ground, Cactus **cactus, int *frames_counter, int *current_frame, int *cactus_spawn_counter, bool *game_over) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        if (
            mouse_x >= retry->button_pos.x && 
            mouse_x <= retry->button_pos.x + retry->button_asset.width &&
            mouse_y >= retry->button_pos.y &&
            mouse_y <= retry->button_pos.y + retry->button_asset.height
        ) {
            reset_game(dino, ground, cactus, frames_counter, current_frame, cactus_spawn_counter, game_over);
        }
    }
}

void handle_input(Dino *dino) {
    if (IsKeyPressed(KEY_SPACE)) {
        jump_key_press(dino);
    }

    if (IsKeyReleased(KEY_SPACE)) {
        jump_key_release(dino);
    }
}

int main() {
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

    const char *sound_paths[SOUND_COUNT] = {
        "./sounds/die_sound.mp3",
        "./sounds/jump_sound.mp3",
        "./sounds/point_sound.mp3"
    };

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Dino");
    InitAudioDevice();
    srand(time(NULL));

    SetTargetFPS(60);

    int frames_counter = 0;
    int cactus_spawn_counter = 0;
    bool game_over = false;

    load_textures(asset_paths);
    load_sounds(sound_paths);

    Dino *dino = create_dino();
    Ground *ground = create_ground();
    Cactus *cactus = NULL;
    Retry *retry = create_retry_texture();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!game_over) {
            DrawFPS(50, 50);
            handle_input(dino);

            if (cactus_spawn_counter >= CACTUS_SPAWN_INTERVAL) {
                cactus = create_cactus();
                cactus_spawn_counter = 0;
            }

            if (cactus != NULL && CheckCollisionRecs(dino->dino_pos_rect, cactus->cactus_pos_rect)) {
                PlaySound(sounds[DIE]);
                game_over = true;
            }

            update(ground, cactus, dino, dt);

            frames_counter++;
            if (frames_counter > 5) {
                current_frame = (current_frame + 1) % 2;
                frames_counter = 0;
            }
            
            render_game(dino, ground, cactus, current_frame);
            cactus_spawn_counter++;
        } else {
            render_game_over(dino, ground, cactus, retry, current_frame);
            restart_game(retry, dino, ground, &cactus, &frames_counter, &current_frame, &cactus_spawn_counter, &game_over);
        }
    }

    unload_and_free(dino, cactus, retry, ground);

    // TODO: Minimize dynamic memory allocation for web
    // TODO: Try to optimize game through js
    // TODO: Add sounds to the game
        // load sounds
        // play sounds
        // unload sounds
    // TODO: Handle crouching (add crouching frame)
    // TODO: Increase difficulty (speed) of the game

    CloseAudioDevice();
    CloseWindow();

    return 0;
}