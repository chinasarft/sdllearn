#pragma once
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif


//   1      2      3
//  _________________
// 5|5     10     15|
//  |               |
//  |               |
// 6|6     12     18|
//  |               |
//  |               |
// 7|7     14     21|
//  
#define RADIUS(s)  (10/s->scaleRatio)
#define NONE_RECT 0
#define MIDDLE_RECT 12
#define LEFT_TOP_RECT 5
#define LEFT_BOTTOM_RECT 7
#define RIGHT_TOP_RECT 15
#define RIGHT_BOTTOM_RECT 21
#define TOP_CENTER_RECT 10
#define BOTTOM_CENTER_RECT 14
#define LEFT_CENTER_RECT 6
#define RIGHT_CENTER_RECT 18
#define MOVE_RECT 56

typedef struct{
    int width;
    int height;
    int leftTopX;
    int leftTopY;
    int previousX;
    int previousY;
    int isSelected;
    int selectedRect;
    int canvasIdx;
    SDL_Texture * texture;
    double angle;
    SDL_RendererFlip flip;
    float scaleRatio;
}Sprite;

void __declspec(dllexport) init_sprite(Sprite * sprite);
int is_mouse_on_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
extern void __declspec(dllexport) sprite_response_mouse_move(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) sprite_response_mouse_up(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) sprite_response_mouse_press(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) draw_select_sprite(SDL_Rect * rect, SDL_Renderer *renderer);
void sprite_set_scale_ratio(Sprite * sprite, float ratio);

#ifdef __cplusplus
}
#endif
