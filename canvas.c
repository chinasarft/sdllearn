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

void init_canvas(Canvas * canvas, int canvasWidth, int canvasHeight,
    int windowWidth, int windowHeight)
{
    canvas->canvasWidth = canvasWidth;
    canvas->canvasHeight = canvasHeight;
    canvas->windowWidth = windowWidth;
    canvas->windowHeight = windowHeight;

    canvas->renderer = NULL;

    canvas->container.len = 0;
    canvas->container.cap = 20;
    canvas->container.selectedIdx = -1;
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
    // because a is 0 is fully transparency
    SDL_SetRenderDrawColor(canvas->renderer, r, g, b, 255);

    //draw rect
    SDL_RenderFillRect(canvas->renderer, &canvas->drawRect);
    SDL_SetRenderDrawColor(canvas->renderer, r, g, b, a);
}

void destroy_canvas(Canvas * canvas)
{
    //TODO mainly destroy texture
}

void add_sprite_to_canvas(Canvas * canvas, Sprite * sprite)
{
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

static int render_texture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, 
    const double angle, const SDL_Point *center, SDL_RendererFlip flip) {
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    return SDL_RenderCopyEx(ren, tex, NULL, &dst, angle, 0, flip);
}

void draw_canvas(Canvas * canvas)
{
    clear_canvas(canvas);

    int deltaX = canvas->drawRect.x;
    int deltaY = canvas->drawRect.y;

    Sprite ** sprites = canvas->container.sprites;
    for (int i = 0; i < canvas->container.len; i++) {
        Sprite * s = sprites[i];
        if (s->isSelected) {
            draw_select_sprite(s, canvas->renderer, canvas->drawRect.x, canvas->drawRect.y);
        }
        render_texture(s->texture, canvas->renderer, s->leftTopX + deltaX, 
            s->leftTopY + deltaY, s->width, s->height,
            s->angle, 0, s->flip);
    }

    //Update the screen
    SDL_RenderPresent(canvas->renderer);
}
