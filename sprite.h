#pragma once
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif


#define RADIUS 10
#define NONE_RECT 0
#define LEFT_TOP_RECT 1
#define LEFT_BOTTOM_RECT 2
#define RIGHT_TOP_RECT 3
#define RIGHT_BOTTOM_RECT 4
#define TOP_CENTER_RECT 5
#define BOTTOM_CENTER_RECT 6
#define LEFT_CENTER_RECT 7
#define RIGHT_CENTER_RECT 8
#define MOVE_RECT 56
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
    int isSelected;
    int selectedRect;
    SDL_Texture * texture;
    double angle;
    SDL_RendererFlip flip;
}Sprite;

int is_mouse_on_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
extern void __declspec(dllexport) sprite_response_mouse_move(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) sprite_response_mouse_up(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) sprite_response_mouse_press(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) draw_select_sprite(Sprite * sprite, SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif
