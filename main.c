#include <SDL2/SDL.h>
#include <stdio.h>
#include "cgol.h"

void handleEvents(GameState *state);

void handleWindowEvent(GameState *state);

void handleMouseClick(GameState *state);

void handleMouseMove(GameState *state);

void handleKeypress(GameState *state, SDL_KeyCode key);

void getMouseCoordinates(int *x, int *y, int *cx, int *cy);

void drawLevel(GameState *state);

void updateField(GameState *state);

void switchBuffer(GameState *state);

int max(int a, int b);

static SDL_Event e;
static SDL_bool running = SDL_FALSE;

int main(int argc, char *args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize!\nError: %s\n", SDL_GetError());
        return 1;
    }

    int counteer = 0;
    int initialWidth = CELL_SIZE * FIELD_WIDTH + 1;
    int initialHeight = CELL_SIZE * FIELD_HEIGHT + 1;
    GameState *state = initGameState();
    state->window = SDL_CreateWindow("Conway's Game Of Life", SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, initialWidth, initialHeight,
                                     SDL_WINDOW_SHOWN);

    SDL_SetWindowMinimumSize(state->window, 10 * CELL_SIZE, 10 * CELL_SIZE);
    state->width = initialWidth;
    state->height = initialHeight;

    if (state->window == NULL) {
        printf("Window could not be created!\nError: %s\n", SDL_GetError());
        return 1;
    }

    state->renderer = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (state->renderer == NULL) {
        printf("Renderer could not be created!\nError: %s\n", SDL_GetError());
        return 1;
    }

    drawLevel(state);

    while (!state->shouldClose) {
        handleEvents(state);
        if (running) {
            if (counteer > STEP_COUNT) {
                updateField(state);
                drawLevel(state);
                counteer = 0;
            }
            counteer++;
        }
        SDL_RenderPresent(state->renderer);
        SDL_Delay(DELAY_MS);
    }

    SDL_DestroyWindow(state->window);
    SDL_DestroyRenderer(state->renderer);
    freeGameState(state);
    SDL_Quit();
    return 0;
}

void handleEvents(GameState *state) {
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            state->shouldClose = SDL_TRUE;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN && !running) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                state->lmbDown = SDL_TRUE;
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                state->rmbDown = SDL_TRUE;
            }
            handleMouseClick(state);
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                state->lmbDown = SDL_FALSE;
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                state->rmbDown = SDL_FALSE;
            }
        }
        if ((state->lmbDown || state->rmbDown) && !running && e.type == SDL_MOUSEMOTION) {
            handleMouseMove(state);
        }
        if (e.type == SDL_WINDOWEVENT) {
            handleWindowEvent(state);
        }
        if (e.type == SDL_KEYDOWN) {
            handleKeypress(state, e.key.keysym.sym);
        }
    }
}

void handleWindowEvent(GameState *state) {
    switch (e.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            state->width = e.window.data1;
            state->height = e.window.data2;
            drawLevel(state);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            drawLevel(state);
            break;
    }
}

void handleMouseClick(GameState *state) {
    int x, y, cx, cy, index;
    getMouseCoordinates(&x, &y, &cx, &cy);
    state->currentX = cx;
    state->currentY = cy;
    if (cx > FIELD_WIDTH || cy > FIELD_HEIGHT) { return; }
    index = cy * FIELD_WIDTH + cx;
    if (state->lmbDown) {
        *(state->field + index) = 1;
    } else if (state->rmbDown) {
        *(state->field + index) = 0;
    }
    if (*(state->field + index)) {
        SDL_SetRenderDrawColor(state->renderer, 0xff, 0xa8, 0x26, 0xFF);
        SDL_Rect fillRect = {x + 1, y + 1, CELL_SIZE - 1, CELL_SIZE - 1};
        SDL_RenderFillRect(state->renderer, &fillRect);
    } else {
        SDL_SetRenderDrawColor(state->renderer, 0x22, 0x22, 0x22, 0xFF);
        SDL_Rect fillRect = {x + 1, y + 1, CELL_SIZE - 1, CELL_SIZE - 1};
        SDL_RenderFillRect(state->renderer, &fillRect);
    }
}

void handleMouseMove(GameState *state) {
    int x, y, cx, cy;
    getMouseCoordinates(&x, &y, &cx, &cy);
    if (cx == state->currentX && cy == state->currentY) { return; }
    handleMouseClick(state);
}

void handleKeypress(GameState *state, SDL_KeyCode key) {
    switch (key) {
        case SDLK_SPACE:
            running = !running;
            break;
        case SDLK_q:
            state->shouldClose = SDL_TRUE;
            break;
        case SDLK_r:
            running = SDL_FALSE;
            resetGameState(state);
            drawLevel(state);
            break;
        default:
            break;
    }
}

void drawLevel(GameState *state) {
    SDL_SetRenderDrawColor(state->renderer, 0x22, 0x22, 0x22, 0xFF);
    SDL_RenderClear(state->renderer);
    SDL_SetRenderDrawColor(state->renderer, 0xff, 0xa8, 0x26, 0xFF);
//    for (int i = 0; i <= FIELD_WIDTH; i++) {
//        int x = i * CELL_SIZE;
//        SDL_RenderDrawLine(state->renderer, x, 0, x, state->height);
//    }
//    for (int i = 0; i <= FIELD_HEIGHT; i++) {
//        int y = i * CELL_SIZE;
//        SDL_RenderDrawLine(state->renderer, 0, y, state->width, y);
//    }
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        for (int j = 0; j < FIELD_WIDTH; j++) {
            if (*(state->field + (i * FIELD_WIDTH + j))) {
                SDL_SetRenderDrawColor(state->renderer, 0xff, 0xa8, 0x26, 0xFF);
                SDL_Rect fillRect = {j * CELL_SIZE + 1, i * CELL_SIZE + 1, CELL_SIZE - 1, CELL_SIZE - 1};
                SDL_RenderFillRect(state->renderer, &fillRect);
                SDL_SetRenderDrawColor(state->renderer, 0x47, 0x3a, 0x1e, 0xFF);
                for (int l = 0; l < 9; l++) {
                    if (!*(state->field + ((i - 1 + (l % 3)) * FIELD_WIDTH + (j - 1 + (max(l, 1) / 3))))) {
                        SDL_Rect fillAlphaRect = {(j - 1 + (max(l, 1) / 3)) * CELL_SIZE + 1,
                                                  (i - 1 + (l % 3)) * CELL_SIZE + 1, CELL_SIZE - 1, CELL_SIZE - 1};
                        SDL_RenderFillRect(state->renderer, &fillAlphaRect);
                    }
                }
            }
        }
    }
}

void updateField(GameState *state) {
    int index, count;
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        for (int j = 0; j < FIELD_WIDTH; j++) {
            count = 0;
            for (int x = -1; x < 2; x++) {
                if (i - 1 < 0 || j + x < 0 || j + x >= FIELD_WIDTH) { continue; }
                index = (i - 1) * FIELD_WIDTH + j + x;
                if (*(state->field + index)) { count++; }
            }
            for (int x = -1; x < 2; x++) {
                if (i + 1 >= FIELD_HEIGHT || j + x < 0 || j + x >= FIELD_WIDTH) { continue; }
                index = (i + 1) * FIELD_WIDTH + j + x;
                if (*(state->field + index)) { count++; }
            }
            if (j - 1 >= 0 && *(state->field + (i * FIELD_WIDTH + j - 1))) { count++; }
            if (j + 1 < FIELD_WIDTH && *(state->field + (i * FIELD_WIDTH + j + 1))) { count++; }
            if (count > 3 || count < 2) { *(state->buffer + (i * FIELD_WIDTH + j)) = 0; }
            else if (count == 3) { *(state->buffer + (i * FIELD_WIDTH + j)) = 1; }
            else { *(state->buffer + (i * FIELD_WIDTH + j)) = *(state->field + (i * FIELD_WIDTH + j)); }
        }
    }
    switchBuffer(state);
}

void switchBuffer(GameState *state) {
    uint8_t *tmp = state->field;
    state->field = state->buffer;
    state->buffer = tmp;
}

void getMouseCoordinates(int *x, int *y, int *cx, int *cy) {
    SDL_GetMouseState(x, y);
    *x -= *x % CELL_SIZE;
    *y -= *y % CELL_SIZE;
    *cx = *x / CELL_SIZE;
    *cy = *y / CELL_SIZE;
}

int max(int a, int b) {
    return a > b ? a : b;
}

/*
long seconds = time(NULL) + 1;
int fps = 0;
fps++;
if (seconds <= time(NULL)) {
    printf("FPS: %d\n", fps);
    fflush(stdout);
    fps = 0;
    seconds = time(NULL) + 1;
}*/

/*
for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
        printf("%d ", *(state->field + i * FIELD_WIDTH + j));
    }
    printf("\n");
}
rintf("\n");
*/