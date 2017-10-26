#pragma once
#include <SDL.h>
typedef struct Canvas {
    int width;
    int height;
};
typedef struct Sprite {
    int width;
    int height;
    int leftTopX;
    int leftTopY;
    SDL_Texture * texture;
};

int is_mouse_in_sprite(Sprite * sprite, Canvas * canvas);
