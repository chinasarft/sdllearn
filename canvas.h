#pragma once
#include "sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    int canvasWidth;
    int canvasHeight;
    int windowWidth;
    int windowHeight;
    SDL_Renderer *renderer;
    struct {
        Sprite ** sprites;
        int len;
        int cap;
        int selectedIdx;
    } container;
    struct {
        Uint8 r;
        Uint8 g;
        Uint8 b;
    }padColor;
    SDL_Rect drawRect; //real draw area. w h is real w h
    float scaleRatio; // drawRect.w / canvasWidth
    SDL_Surface * surface;
    SDL_Texture * canvasTexture;
}Canvas;

// canvasWidth: not real width. like 16
// canvasHeight: not real height like 9
void __declspec(dllexport) init_canvas(Canvas * canvas, SDL_Renderer * renderer, SDL_Surface * surface,
                                        int canvasWidth, int canvasHeight,
                                        int windowWidth, int windowHeight);
void __declspec(dllexport) clear_canvas(Canvas * canvas);
void __declspec(dllexport) destroy_canvas(Canvas * canvas);

int add_sprite_to_canvas(Canvas * canvas, Sprite * sprite);

int remove_sprite_from_canvas(Canvas * canvas, Sprite * sprite);

void set_canvas_pad_color(Canvas * canvas, Uint8 r, Uint8 g, Uint8 b);
int canvas_insert_sprite(Canvas * canvas, Sprite * sprite, int insertIdx);
int canvas_remote_sprite_by_index(Canvas * canvas, int removeIdx);

void __declspec(dllexport) canvas_response_mouse_move(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) canvas_response_mouse_up(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent);
void __declspec(dllexport) canvas_response_mouse_press(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent);

void draw_canvas(Canvas * canvas);
void canvas_move_selected_sprite_to_top(Canvas * canvas);

#ifdef __cplusplus
}
#endif