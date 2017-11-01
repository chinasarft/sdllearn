#include "canvas.h"
#include <string.h>

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
                canvas->drawRect.y = (canvas->windowHeight - newHeight) / 2;
            }
            else {
                newWidth = newHeight * (((float)canvas->canvasWidth) / canvas->canvasHeight);
                canvas->drawRect.x = (canvas->windowWidth - newWidth) / 2;
            }
        }
    }
    canvas->drawRect.w = newWidth;
    canvas->drawRect.h = newHeight;
}

void set_canvas_pad_color(Canvas * canvas, Uint8 r, Uint8 g, Uint8 b)
{
    canvas->padColor.r = r;
    canvas->padColor.g = g;
    canvas->padColor.b = b;
}

void init_canvas(Canvas * canvas, SDL_Renderer * renderer, SDL_Surface * surface,
    int canvasWidth, int canvasHeight,
    int windowWidth, int windowHeight)
{
    canvas->canvasWidth = canvasWidth;
    canvas->canvasHeight = canvasHeight;
    canvas->windowWidth = windowWidth;
    canvas->windowHeight = windowHeight;

    canvas->surface = surface;
    canvas->renderer = renderer;

    canvas->container.len = 0;
    canvas->container.cap = 20;
    canvas->container.selectedIdx = -1;
    canvas->container.sprites = (Sprite **)malloc(sizeof(Sprite*) * canvas->container.cap);

    set_draw_rect(canvas);
}

void present_canvas(Canvas * canvas)
{
    if(!is_canvas_same_ratio_as_window(canvas)){
        //pad color
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(canvas->renderer, &r, &g, &b, &a);

        SDL_SetRenderDrawColor(canvas->renderer, canvas->padColor.r, canvas->padColor.g, canvas->padColor.b, 255);

        SDL_Rect rects[2] = { 0 };
        if (canvas->drawRect.x == 0) {
            rects[0].w = canvas->drawRect.w;
            rects[0].h = canvas->drawRect.y;
            rects[1].y = canvas->drawRect.y + canvas->drawRect.h;
            rects[1].w = canvas->drawRect.w;
            rects[1].h = canvas->drawRect.y;
        }
        else {
            rects[0].w = canvas->drawRect.x;
            rects[0].h = canvas->drawRect.h;
            rects[1].x = canvas->drawRect.x + canvas->drawRect.w;
            rects[1].w = canvas->drawRect.x;
            rects[1].h = canvas->drawRect.h;
        }
        //draw rect
        SDL_RenderFillRects(canvas->renderer, &rects, 2);

        SDL_SetRenderDrawColor(canvas->renderer, r, g, b, a);
    }

    //Update the screen
    SDL_RenderPresent(canvas->renderer);
}

void clear_canvas(Canvas * canvas)
{
    SDL_RenderClear(canvas->renderer);
}

void destroy_canvas(Canvas * canvas)
{
    //TODO mainly destroy texture
}

void add_sprite_to_canvas(Canvas * canvas, Sprite * sprite)
{
    sprite_set_location_point(sprite, canvas->drawRect.x, canvas->drawRect.y);

    if (canvas->container.len == 0) {
        sprite->canvasIdx = 0;
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

void canvas_response_mouse_move(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent)
{
    int idx = canvas->container.selectedIdx;
    if (idx > -1) {
        sprite_response_mouse_move(canvas->container.sprites[idx], mouseEvent);
    }
    return;
}

void canvas_response_mouse_up(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent)
{
    int idx = canvas->container.selectedIdx;
    if (idx > -1) {
        sprite_response_mouse_up(canvas->container.sprites[idx], mouseEvent);
    }
    return;
}

void canvas_response_mouse_press(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent)
{
    Sprite ** sprites = canvas->container.sprites;

    int idx = canvas->container.selectedIdx;
    if (idx > -1)
        sprites[idx]->isSelected = 0;

    for (int i = 0; i < canvas->container.len; i++) {
        sprites[i]->isSelected = 0;
        sprite_response_mouse_press(sprites[i], mouseEvent);
        if (sprites[i]->isSelected) {
            canvas->container.selectedIdx = i;
            break;
        }
    }
}

void draw_canvas(Canvas * canvas)
{
    clear_canvas(canvas);

    Sprite ** sprites = canvas->container.sprites;
    Sprite * selectedSprite = NULL;
    for (int i = 0; i < canvas->container.len; i++) {
        Sprite * s = sprites[i];
        if (s->isSelected) {
            selectedSprite = s;
        }
        SDL_Rect dst = { s->leftTopX, s->leftTopY,  s->width, s->height };
        SDL_RenderCopyEx(canvas->renderer, s->texture, NULL, &dst, s->angle, 0, s->flip);
    }

    //TODO get render pixels here
    //SDL_GetWindowPixelFormat same as surface->format->format
    //SDL_RenderReadPixels(canvas->renderer, &canvas->drawRect,
    //    canvas->surface->format->format, NULL, canvas->surface->pitch);

    if (selectedSprite != NULL) {
        SDL_Rect rect = {selectedSprite->leftTopX, selectedSprite->leftTopY,
            selectedSprite->width, selectedSprite->height};
        draw_select_sprite(&rect, canvas->renderer, canvas->drawRect.x, canvas->drawRect.y);
    }

    present_canvas(canvas);
}

void canvas_move_selected_sprite_to_top(Canvas * canvas)
{
    int idx = canvas->container.selectedIdx;
    if (idx < 1) {
        return;
    }
    Sprite * tmp = canvas->container.sprites[0];

    canvas->container.sprites[idx]->canvasIdx = 0;
    canvas->container.sprites[0] = canvas->container.sprites[idx];

    tmp->canvasIdx = idx;
    canvas->container.sprites[idx] = tmp;
    return;
}