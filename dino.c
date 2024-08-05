#include "raylib/src/raylib.h"
#include "raylib/src/raymath.h"
#include <stdbool.h>
#include <stdio.h>
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
#define MAX_CACTI_NUM 10
#define INIT_CACTUS_X 900.0f

Texture2D* textures;
Sound* sounds;

typedef enum
{
    CACTUS1,
    CACTUS2,
    CACTUS3,
    DINO_RUN1,
    DINO_RUN2,
    GROUND,
    RETRY_BUTTON,
    RETRY_TEXT,
    DINO_DUCK1,
    DINO_DUCK2,
    ASSET_COUNT
} AssetName;

typedef enum
{
    DIE,
    JUMP,
    POINT,
    SOUND_COUNT
} SoundName;

typedef struct
{
    Texture2D button_asset;
    Texture2D text_asset;
    Vector2 button_pos;
    Vector2 text_pos;
} Retry;

typedef struct
{
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
    Texture2D current_frame;
    Rectangle dino_pos_rect;
} Dino;

typedef struct
{
    Vector2 size;
    Vector2 pos;
    Vector2 vel;
    Texture2D asset;
    Rectangle cactus_pos_rect;
} Cactus;

typedef struct
{
    Vector2 pos;
    Vector2 vel;
    Texture2D ground_frame;
} Ground;

void load_textures(const char* asset_paths[])
{
    textures = malloc(ASSET_COUNT * sizeof(Texture2D));
    for (int i = 0; i < ASSET_COUNT; i++)
    {
        textures[i] = LoadTexture(asset_paths[i]);
    }
}

void load_sounds(const char* sound_paths[])
{
    sounds = malloc(SOUND_COUNT * sizeof(Sound));
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        sounds[i] = LoadSound(sound_paths[i]);
    }
}

void render_cactus(Cactus* cactus)
{
    if (cactus != NULL)
    {
        DrawTexture(cactus->asset, cactus->pos.x, cactus->pos.y, WHITE);
    }
}

void render_ground(Ground* ground)
{
    DrawTexture(ground->ground_frame, ground->pos.x, ground->pos.y, WHITE);
    DrawTexture(ground->ground_frame, ground->pos.x + ground->ground_frame.width, ground->pos.y,
                WHITE);
}

void render_dino(Dino* dino) { DrawTexture(dino->current_frame, dino->pos.x, dino->pos.y, WHITE); }

void render_game_over(Retry* retry)
{
    DrawTexture(retry->button_asset, retry->button_pos.x, retry->button_pos.y, WHITE);
    DrawTexture(retry->text_asset, retry->text_pos.x, retry->text_pos.y, WHITE);
}

void render_game(Dino* dino, Ground* ground, Cactus* cacti[], int array_length)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    render_ground(ground);
    render_dino(dino);

    for (int i = 0; i < array_length; i++)
    {
        render_cactus(cacti[i]);
    }

    EndDrawing();
}

void update_ground(Ground* ground)
{
    ground->pos = Vector2Add(ground->pos, ground->vel);

    if (ground->pos.x <= -ground->ground_frame.width)
    {
        ground->pos.x = 0;
    }
}

void update_cactus(Cactus* cactus)
{
    cactus->pos = Vector2Add(cactus->pos, cactus->vel);

    cactus->cactus_pos_rect.x = cactus->pos.x;
    cactus->cactus_pos_rect.y = cactus->pos.y;
}

void update_dino(Dino* dino, float time, int frame_flag)
{
    Vector2 scaled_vel = Vector2Scale(dino->vel, time);
    dino->pos = Vector2Add(dino->pos, scaled_vel);
    dino->vel.y += GRAVITY * time;

    if (dino->pos.y >= 500)
    {
        dino->pos.y = 500;
        dino->vel.y = 0;
    }

    switch (frame_flag)
    {
    case 0:
        dino->current_frame = textures[DINO_RUN1];
        break;
    case 1:
        dino->current_frame = textures[DINO_RUN2];
        break;
    case 2:
        dino->current_frame = textures[DINO_DUCK1];
        dino->pos.y += textures[DINO_RUN1].height - textures[DINO_DUCK1].height;
        break;
    case 3:
        dino->current_frame = textures[DINO_DUCK2];
        dino->pos.y += textures[DINO_RUN2].height - textures[DINO_DUCK2].height;
        break;
    }

    dino->dino_pos_rect.x = dino->pos.x;
    dino->dino_pos_rect.y = dino->pos.y;
}

void jump_key_press(Dino* dino)
{
    PlaySound(sounds[JUMP]);
    if (dino->pos.y >= 500)
        dino->vel.y = -1000.0f;
}

void jump_key_release(Dino* dino)
{
    if (dino->vel.y < -50.0f)
        dino->vel.y = -50.0f;
}

Retry* create_retry_texture()
{
    Retry* retry = malloc(sizeof(Retry));
    Vector2 button_pos = {WINDOW_WIDTH / 2 - textures[RETRY_BUTTON].width / 2,
                          WINDOW_HEIGHT / 2 - textures[RETRY_BUTTON].height / 2};
    Vector2 text_pos = {WINDOW_WIDTH / 2 - textures[RETRY_TEXT].width / 2,
                        WINDOW_HEIGHT / 2 - textures[RETRY_TEXT].height / 2 - 70};

    retry->button_asset = textures[RETRY_BUTTON];
    retry->button_pos = button_pos;
    retry->text_asset = textures[RETRY_TEXT];
    retry->text_pos = text_pos;

    return retry;
}

Cactus* create_cactus(float* init_x)
{
    Cactus* cactus = malloc(sizeof(Cactus));
    int random = rand() % CACTUS_TYPES;
    Vector2 size = {textures[random].width, textures[random].height};
    Vector2 pos = {*init_x, INIT_GROUND_Y - textures[random].height + 20};
    Vector2 vel = {-10.0f, 0.0f};
    Rectangle cactus_pos_rect = {pos.x, pos.y, textures[random].width,
                                 textures[random].height + 20};

    cactus->asset = textures[random];
    cactus->size = size;
    cactus->pos = pos;
    cactus->vel = vel;
    cactus->cactus_pos_rect = cactus_pos_rect;

    init_x += rand() % 50;

    return cactus;
}

Ground* create_ground()
{
    Ground* ground = malloc(sizeof(Ground));
    Vector2 vel = {GROUND_VEL_X, 0.0f};
    Vector2 pos = {0.0f, INIT_GROUND_Y};

    ground->ground_frame = textures[GROUND];
    ground->vel = vel;
    ground->pos = pos;

    return ground;
}

Dino* create_dino()
{
    Dino* dino = malloc(sizeof(Dino));
    Vector2 size = {textures[DINO_RUN1].width, textures[DINO_RUN1].height};
    Vector2 vel = {0.0f, 0.0f};
    Vector2 pos = {INIT_DINO_X, INIT_DINO_Y};
    Rectangle dino_pos_rect = {INIT_DINO_X, INIT_DINO_Y, textures[DINO_RUN1].width - 5,
                               textures[DINO_RUN1].height - 5};
    Texture2D current_frame = textures[DINO_RUN1];

    dino->dino_pos_rect = dino_pos_rect;
    dino->current_frame = current_frame;
    dino->size = size;
    dino->pos = pos;
    dino->vel = vel;

    return dino;
}

void unload_and_free(Dino* dino, Cactus* cacti[], Retry* retry, Ground* ground, int array_length)
{
    for (int i = 0; i < ASSET_COUNT; i++)
    {
        UnloadTexture(textures[i]);
    }
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        UnloadSound(sounds[i]);
    }
    for (int i = 0; i < array_length; i++)
    {
        free(cacti[i]);
    }
    free(cacti);
    free(retry);
    free(dino);
    free(ground);
}

void update_game(Ground* ground, Cactus* cacti[], Dino* dino, float time, int frame_flag,
                 int array_length)
{
    update_dino(dino, time, frame_flag);

    for (int i = 0; i < array_length; i++)
    {
        if (cacti[i] != NULL)
        {
            update_cactus(cacti[i]);
        }
    }

    update_ground(ground);
}

void reset_game(Dino* dino, Ground* ground, Cactus** cactus, int* frames_counter,
                int* current_frame, int* cactus_spawn_counter, bool* game_over, int* frame_flag)
{
    dino->pos = (Vector2){INIT_DINO_X, INIT_DINO_Y};
    dino->vel = (Vector2){0.0f, 0.0f};
    dino->dino_pos_rect.x = INIT_DINO_X;
    dino->dino_pos_rect.y = INIT_DINO_Y;

    ground->pos = (Vector2){0.0f, INIT_GROUND_Y};
    ground->vel = (Vector2){GROUND_VEL_X, 0.0f};

    if (*cactus != NULL)
    {
        free(*cactus);
        *cactus = NULL;
    }

    *frames_counter = 0;
    *current_frame = 0;
    *cactus_spawn_counter = 0;
    *game_over = false;
    *frame_flag = 0;
}

void restart_game(Retry* retry, Dino* dino, Ground* ground, Cactus** cactus, int* frames_counter,
                  int* current_frame, int* cactus_spawn_counter, bool* game_over, int* frame_flag)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        if (mouse_x >= retry->button_pos.x &&
            mouse_x <= retry->button_pos.x + retry->button_asset.width &&
            mouse_y >= retry->button_pos.y &&
            mouse_y <= retry->button_pos.y + retry->button_asset.height)
        {
            reset_game(dino, ground, cactus, frames_counter, current_frame, cactus_spawn_counter,
                       game_over, frame_flag);
        }
    }
}

void handle_input(Dino* dino, int* frame_flag)
{
    if (IsKeyPressed(KEY_SPACE))
    {
        jump_key_press(dino);
    }

    if (IsKeyReleased(KEY_SPACE))
    {
        jump_key_release(dino);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        *frame_flag = 2;
    }

    if (IsKeyReleased(KEY_DOWN))
    {
        *frame_flag = 0;
    }
}

void remove_element(Cactus* cacti[], int* array_length)
{
    free(cacti[0]);
    for (int i = 1; i < *array_length; i++)
    {
        cacti[i - 1] = cacti[i];
    }

    (*array_length)--;
}

int main()
{
    const char* asset_paths[ASSET_COUNT] = {"./assets/cactus_1.png",     "./assets/cactus_2.png",
                                            "./assets/cactus_3.png",     "./assets/dino_run1.png",
                                            "./assets/dino_run2.png",    "./assets/ground.png",
                                            "./assets/retry_button.png", "./assets/retry_text.png",
                                            "./assets/dino_duck1.png",   "./assets/dino_duck2.png"};

    const char* sound_paths[SOUND_COUNT] = {"./sounds/die_sound.mp3", "./sounds/jump_sound.mp3",
                                            "./sounds/point_sound.mp3"};

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Dino");
    InitAudioDevice();
    srand(time(NULL));

    SetTargetFPS(60);

    int frames_counter = 0;
    int cactus_spawn_counter = 0;
    int current_frame = 0;
    bool game_over = false;
    int frame_flag = 0;
    int array_length = MAX_CACTI_NUM;
    float init_cactus_x = INIT_CACTUS_X;

    load_textures(asset_paths);
    load_sounds(sound_paths);

    Dino* dino = create_dino();
    Ground* ground = create_ground();
    Cactus* cacti[MAX_CACTI_NUM] = {0};
    Retry* retry = create_retry_texture();

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        DrawFPS(50, 50);
        render_game(dino, ground, cacti, array_length);

        if (!game_over)
        {
            handle_input(dino, &frame_flag);

            frames_counter++;
            if (frames_counter > 5)
            {
                frame_flag ^= 1;
                frames_counter = 0;
            }

            if (cactus_spawn_counter >= 300)
            {
                remove_element(cacti, &array_length);
                Cactus* cactus = create_cactus(&init_cactus_x);
                cacti[array_length - 1] = cactus;
                array_length++;
                cactus_spawn_counter = 0;
            }

            for (int i = 0; i < array_length; i++)
            {
                if (cacti[i] != NULL &&
                    CheckCollisionRecs(dino->dino_pos_rect, cacti[i]->cactus_pos_rect))
                {
                    PlaySound(sounds[DIE]);
                    game_over = true;
                }
            }

            update_game(ground, cacti, dino, dt, frame_flag, array_length);

            cactus_spawn_counter++;
        }
        else
        {
            render_game_over(retry);
            restart_game(retry, dino, ground, cacti, &frames_counter, &current_frame,
                         &cactus_spawn_counter, &game_over, &frame_flag);
        }
    }

    unload_and_free(dino, cacti, retry, ground, array_length);

    // TODO: Minimize dynamic memory allocation for web
    // TODO: Try to optimize game through js
    // TODO: Increase difficulty (speed) of the game
    // TODO: Refactor the cactus spawning
    // TODO: Change the dino pos rect height when crouching to avoid obstacles

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
