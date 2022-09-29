//
// Created by turok on 11/7/21.
//

#ifndef CGOL_CGOL_H
#define CGOL_CGOL_H

#include <SDL2/SDL.h>

#define CELL_SIZE 24
#define FIELD_WIDTH 40
#define FIELD_HEIGHT 32
#define DELAY_MS 30
#define STEP_COUNT 5

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_bool shouldClose;
    SDL_bool lmbDown;
    SDL_bool rmbDown;
    int currentX, currentY;
    int width, height;
    uint8_t *field;
    uint8_t *buffer;
} GameState;

GameState *initGameState() {
    GameState *state = malloc(sizeof(GameState));
    state->shouldClose = SDL_FALSE;
    state->window = NULL;
    state->renderer = NULL;
    state->field = malloc(FIELD_WIDTH * FIELD_HEIGHT * sizeof(uint8_t));
    state->buffer = malloc(FIELD_WIDTH * FIELD_HEIGHT * sizeof(uint8_t));
    for (int i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) { *(state->field + i) = 0; }
    for (int i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) { *(state->buffer + i) = 0; }
    return state;
}

void resetGameState(GameState *state) {
    for (int i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) { *(state->field + i) = 0; }
    for (int i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) { *(state->buffer + i) = 0; }
}

void freeGameState(GameState *state) {
    free(state->field);
    free(state);
}

#endif //CGOL_CGOL_H