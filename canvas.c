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
    canvas->scaleRatio = canvas->drawRect.w / (float)(canvas->canvasWidth);
}

static int get_pixel_format_pitch(Uint32 pixelFormat, int w)
{
    switch (pixelFormat) {
    case SDL_PIXELFORMAT_RGBA8888:
    case SDL_PIXELFORMAT_ARGB8888:
    case SDL_PIXELFORMAT_BGRA8888:
    case SDL_PIXELFORMAT_ABGR8888:
        return w*4;
    }
    return 0;
}

void set_canvas_pad_color(Canvas * canvas, Uint8 r, Uint8 g, Uint8 b)
{
    canvas->padColor.r = r;
    canvas->padColor.g = g;
    canvas->padColor.b = b;
}

int init_canvas(Canvas * canvas, SDL_Renderer * renderer,
    int canvasWidth, int canvasHeight,
    int windowWidth, int windowHeight)
{
    canvas->canvasWidth = canvasWidth;
    canvas->canvasHeight = canvasHeight;
    canvas->windowWidth = windowWidth;
    canvas->windowHeight = windowHeight;

    canvas->renderer = renderer;

    canvas->container.len = 0;
    canvas->container.cap = 20;
    canvas->container.selectedIdx = -1;
    canvas->container.sprites = (Sprite **)malloc(sizeof(Sprite*) * canvas->container.cap);

    set_draw_rect(canvas);

    canvas->targetTexture = SDL_CreateTexture(canvas->renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, canvas->canvasWidth, canvas->canvasHeight);
    if (canvas->targetTexture == NULL) {
        return -1;
    }
    canvas->pixels = (Uint8 *)malloc(canvas->canvasWidth * canvas->canvasHeight * 4);
    return 0;
}

void present_canvas(Canvas * canvas)
{
    //Update the screen
    SDL_RenderPresent(canvas->renderer);
}

// not use now
static void canvas_draw_padding(Canvas * canvas)
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
        SDL_RenderFillRects(canvas->renderer, rects, 2);

        SDL_SetRenderDrawColor(canvas->renderer, r, g, b, a);
    }

}

void clear_canvas(Canvas * canvas)
{
    SDL_RenderClear(canvas->renderer);
}

static void clear_canvas_with_pad_color(Canvas * canvas)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(canvas->renderer, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(canvas->renderer, canvas->padColor.r, canvas->padColor.g, canvas->padColor.b, 255);
    SDL_RenderClear(canvas->renderer);
    SDL_SetRenderDrawColor(canvas->renderer, r, g, b, a);
}

void destroy_canvas(Canvas * canvas)
{
    //TODO mainly destroy texture
    if (canvas->pixels != NULL) {
        free(canvas->pixels);
        canvas->pixels = NULL;
    }
    if (canvas->targetTexture != NULL) {
        SDL_DestroyTexture(canvas->targetTexture);
        canvas->targetTexture = NULL;
    }
}

static int canvas_realloc(Canvas *canvas)
{
    if (canvas->container.len == canvas->container.cap) {
        Sprite ** oldSprites = canvas->container.sprites;
        Sprite ** newSprites = (Sprite **)malloc(sizeof(Sprite*) * canvas->container.cap * 2);
        if (newSprites == NULL)
            return -1;
        canvas->container.cap *= 2;
        memcpy(newSprites, oldSprites, canvas->container.len * sizeof(Sprite *));
        canvas->container.sprites = newSprites;
        return 0 ;
    }
    return 1;
}

int canvas_insert_sprite(Canvas * canvas, Sprite * sprite, int insertIdx)
{
    int ret;

    if (insertIdx > canvas->container.len)
        return -1;
    
    if ((ret = canvas_realloc(canvas)) < 0)
        return ret;

    Sprite ** begin = canvas->container.sprites;
    if (canvas->container.len > 0) {
        for (int i = canvas->container.len - 1; i >= insertIdx; i--) {
            begin[i]->canvasIdx += 1;
            begin[i + 1] = begin[i];
        }
    }

    begin[insertIdx] = sprite;
    sprite->canvasIdx = insertIdx;
    canvas->container.len += 1;

    sprite_set_scale_ratio(sprite, canvas->scaleRatio);

    return 0;
}

int add_sprite_to_canvas(Canvas * canvas, Sprite * sprite)
{
    return canvas_insert_sprite(canvas, sprite, 0);
}

int remove_sprite_from_canvas(Canvas * canvas, Sprite * sprite)
{
    if (sprite->canvasIdx < canvas->container.len) {
        Sprite ** begin = canvas->container.sprites;
        if (sprite != begin[sprite->canvasIdx])
            return 2;

        for (int i = sprite->canvasIdx + 1; i < canvas->container.len; i++) {
            begin[i]->canvasIdx -= 1;
            begin[i - 1] = begin[i];
        }
        canvas->container.len -= 1;
        return 0;
    }
    return 1;
}

int canvas_remote_sprite_by_index(Canvas * canvas, int removeIdx)
{
    if (removeIdx < canvas->container.len) {
        return remove_sprite_from_canvas(canvas, canvas->container.sprites[removeIdx]);
    }
    return 1;
}

static void canvas_scale_mouse(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent)
{
    mouseEvent->x -= canvas->drawRect.x;
    mouseEvent->y -= canvas->drawRect.y;
    mouseEvent->x /= canvas->scaleRatio;
    mouseEvent->y /= canvas->scaleRatio;
}

void canvas_response_mouse_move(Canvas * canvas, SDL_MouseMotionEvent *mouseEvent)
{
    int idx = canvas->container.selectedIdx;
    if (idx > -1) {
        canvas_scale_mouse(canvas, mouseEvent);
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

    canvas_scale_mouse(canvas, mouseEvent);
    for (int i = 0; i < canvas->container.len; i++) {
        sprites[i]->isSelected = 0;
        sprite_response_mouse_press(sprites[i], mouseEvent);
        if (sprites[i]->isSelected) {
            canvas->container.selectedIdx = i;
            break;
        }
    }
}

#include <stdio.h>
FILE * file = NULL;
void write_texture(Uint8 *pixels, int w, int h, int pitch)
{
    if (pixels == NULL)
        return;
    if (file == NULL) {
        file = fopen("mt.rgba", "w+");
    }
    if (file != NULL) {
        int len = pitch * h;
        fwrite(pixels, len, 1, file);
        fclose(file);
        file = NULL;
    }
}

void draw_canvas(Canvas * canvas)
{
    Sprite ** sprites = canvas->container.sprites;
    Sprite * selectedSprite = NULL;

    SDL_SetRenderTarget(canvas->renderer, canvas->targetTexture);
    clear_canvas(canvas);

    for (int i = canvas->container.len - 1; i >= 0; i--) {
        Sprite * s = sprites[i];
        if (s->isSelected) {
            selectedSprite = s;
        }
        SDL_Rect dst = { s->leftTopX, s->leftTopY,  s->width, s->height };
        SDL_RenderCopyEx(canvas->renderer, s->texture, NULL, &dst, s->angle, 0, s->flip);
    }

    //start--------------------------------------
    //SDL_GetWindowPixelFormat same as surface->format->format
    //pitch value define by us not the render or render target
    if (canvas->catchCanvas) {
        int pitch = get_pixel_format_pitch(SDL_PIXELFORMAT_RGBA8888, canvas->canvasWidth);
        if (SDL_RenderReadPixels(canvas->renderer, NULL,
            SDL_PIXELFORMAT_RGBA8888, canvas->pixels, pitch) == 0) {
            write_texture(canvas->pixels, canvas->canvasWidth,
                canvas->canvasHeight, pitch);
        }
        else {
            char * tmp = SDL_GetError();
            int x = 0;
        }
        canvas->catchCanvas = 0;
    }
    //end--------------------------------------

    SDL_SetRenderTarget(canvas->renderer, NULL);
    clear_canvas_with_pad_color(canvas);

    SDL_RenderCopy(canvas->renderer, canvas->targetTexture, NULL, &canvas->drawRect);

    if (selectedSprite != NULL) {
        SDL_Rect rect = { selectedSprite->leftTopX * canvas->scaleRatio + canvas->drawRect.x,
            selectedSprite->leftTopY * canvas->scaleRatio + canvas->drawRect.y,
            selectedSprite->width * canvas->scaleRatio,
            selectedSprite->height * canvas->scaleRatio
        };
        draw_select_sprite(&rect, canvas->renderer);
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