#pragma once
#include "sprite.h"
#include <string.h>

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
    } container;
    struct {
        Uint8 r;
        Uint8 g;
        Uint8 b;
    }padColor;
    SDL_Rect drawRect;
}Canvas;

static int is_canvas_same_ratio_as_window(Canvas * canvas)
{
    float diff = (canvas->canvasWidth / (float)canvas->windowWidth) -
        (canvas->canvasHeight / (float)canvas->windowHeight);

    if(diff < 0.00001){
        return 1;
    }
    return 0;
}

static void set_draw_rect(Canvas * canvas)
{
    int newWidth = 0, newHeight = 0;
    canvas->drawRect.x = 0;
    canvas->drawRect.y = 0;
    if (canvas->windowWidth > 0 && canvas->windowHeight > 0) {
        if (canvas->canvasWidth > 0 && canvas->canvasHeight > 0) {
            newWidth = canvas->windowWidth;
            newHeight = canvas->windowHeight;
            if (canvas->canvasWidth / (float)newWidth > canvas->canvasHeight / (float)newHeight) {
                newHeight = newWidth * (((float)canvas->canvasHeight) / canvas->canvasWidth);
                canvas->drawRect.x = (canvas->windowHeight - newHeight) / 2;
            }
            else {
                newWidth = newHeight * (((float)canvas->canvasWidth) / canvas->canvasHeight);
                canvas->drawRect.y = (canvas->windowWidth - newWidth) / 2;
            }
        }
    }
    canvas->drawRect.w = newWidth;
    canvas->drawRect.h = newHeight;
}

void init_canvas(Canvas * canvas, int canvasWidth, int canvasHeight,
    int windowWidth, int windowHeight)
{
    canvas->canvasWidth = canvasWidth;
    canvas->canvasHeight = canvasHeight;
    canvas->windowWidth = windowWidth;
    canvas->windowHeight = windowHeight;

    canvas->container.len = 0;
    canvas->container.cap = 20;
    canvas->container.sprites = (Sprite **)malloc(sizeof(Sprite*) * canvas->container.cap);

    set_draw_rect(canvas);
}

void clear_canvas(Canvas * canvas)
{
    if(is_canvas_same_ratio_as_window(canvas)){
        SDL_RenderClear(canvas->renderer);
        return;
    }

    //pad color
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(canvas->renderer, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(canvas->renderer, canvas->padColor.r, canvas->padColor.g, canvas->padColor.b, 255);
    SDL_RenderClear(canvas->renderer);

    SDL_SetRenderDrawColor(canvas->renderer, r, g, b, a);

    //draw rect
    SDL_RenderFillRect(canvas->renderer, &canvas->drawRect);
}

void destroy_canvas(Canvas * canvas)
{
    //TODO mainly destroy texture
}

void add_sprite_to_canvas(Canvas * canvas, Sprite * sprite)
{
    if (canvas->container.len == 0) {
        canvas->container.sprites[canvas->container.len++] = sprite;
        return;
    }
    if (canvas->container.len == canvas->container.cap) {
        sprite->canvasIdx = canvas->container.len;
        Sprite ** oldSprites = canvas->container.sprites;
        canvas->container.cap *= 2;
        canvas->container.sprites = (Sprite **)malloc(sizeof(Sprite*) * canvas->container.cap);
        memcpy(canvas->container.sprites + sizeof(Sprite*), oldSprites,
            canvas->container.len * sizeof(Sprite *));
        canvas->container.sprites[canvas->container.len++] = sprite;
        return;
    }
    sprite->canvasIdx = canvas->container.len;
    memmove(canvas->container.sprites + sizeof(Sprite*), canvas->container.sprites,
        canvas->container.len * sizeof(Sprite *));
    canvas->container.sprites[canvas->container.len++] = sprite;
    return;
}

void remove_sprite_from_canvas(Canvas * canvas, Sprite * sprite)
{
    if (sprite->canvasIdx < canvas->container.len) {
        Sprite ** begin;
        memmove(begin + sizeof(Sprite*) * sprite->canvasIdx,
            begin + sizeof(Sprite *) * (sprite->canvasIdx + 1),
            (canvas->container.len - sprite->canvasIdx - 1) * sizeof(Sprite*));
    }
    return;
}
