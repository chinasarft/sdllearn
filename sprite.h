#pragma once
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RADIUS 10
typedef struct{
    int width;
    int height;
}Canvas;
typedef struct{
    int width;
    int height;
    int leftTopX;
    int leftTopY;
    int previousX;
    int previousY;
    int isMousePressed;
    SDL_Texture * texture;
}Sprite;

int is_mouse_on_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
extern void __declspec(dllexport) sprite_response_mouse_move(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) sprite_response_mouse_up(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) sprite_response_mouse_press(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);

#ifdef __cplusplus
}
#endif
