#include "sprite.h"
int is_mouse_on_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX) && (mouseEvent->x < sprite->leftTopX + sprite->width)
        && (mouseEvent->y > sprite->leftTopY) && (mouseEvent->y < sprite->leftTopY + sprite->height);
}

static int is_mouse_on_left_top_rect(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX - RADIUS) && (mouseEvent->x < sprite->leftTopX + RADIUS)
        && (mouseEvent->y > sprite->leftTopY - RADIUS) && (mouseEvent->y < sprite->leftTopY + RADIUS);
}

static int is_mouse_on_right_top_rect(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX + sprite->width + - RADIUS) && (mouseEvent->x < sprite->leftTopX + sprite->width + RADIUS)
        && (mouseEvent->y > sprite->leftTopY - RADIUS) && (mouseEvent->y < sprite->leftTopY + RADIUS);
}

static int is_mouse_on_left_bottom_rect(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX - RADIUS) && (mouseEvent->x < sprite->leftTopX + RADIUS)
        && (mouseEvent->y > sprite->leftTopY + sprite->height - RADIUS) && (mouseEvent->y < sprite->leftTopY + sprite->height + RADIUS);
}

static int is_mouse_on_top_center(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX + sprite->width/2 - RADIUS) && (mouseEvent->x < sprite->leftTopX + sprite->width/2 + RADIUS)
        && (mouseEvent->y > sprite->leftTopY - RADIUS) && (mouseEvent->y < sprite->leftTopY + RADIUS);
}

static int is_mouse_on_bottom_center(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX + sprite->width / 2 - RADIUS) && (mouseEvent->x < sprite->leftTopX + sprite->width / 2 + RADIUS)
        && (mouseEvent->y > sprite->leftTopY + sprite->height - RADIUS) && (mouseEvent->y < sprite->leftTopY + sprite->height + RADIUS);
}

static int is_mouse_on_left_center(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX - RADIUS) && (mouseEvent->x < sprite->leftTopX + RADIUS)
        && (mouseEvent->y > sprite->leftTopY + sprite->height/2 - RADIUS) && (mouseEvent->y < sprite->leftTopY + sprite->height/2 + RADIUS);
}

static int is_mouse_on_right_center(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX + sprite->width - RADIUS) && (mouseEvent->x < sprite->leftTopX + sprite->width + RADIUS)
        && (mouseEvent->y > sprite->leftTopY + sprite->height/2 - RADIUS) && (mouseEvent->y < sprite->leftTopY + sprite->height/2 + RADIUS);
}


static int is_mouse_on_right_bottom_rect(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX + sprite->width + - RADIUS) && (mouseEvent->x < sprite->leftTopX + sprite->width + RADIUS)
        && (mouseEvent->y > sprite->leftTopY + sprite->height - RADIUS) && (mouseEvent->y < sprite->leftTopY + sprite->height + RADIUS);
}

int move_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    sprite->leftTopX = (mouseEvent->x - sprite->previousX + sprite->leftTopX);
    sprite->leftTopY = (mouseEvent->y - sprite->previousY + sprite->leftTopY);
    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;
}

int left_top_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    move_sprite(sprite, mouseEvent);

    sprite->width -= diffX;
    sprite->height -= diffY;
}

int left_bottom_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    sprite->leftTopX = sprite->leftTopX + diffX;

    sprite->width -= diffX;
    sprite->height += diffY;
}

int right_top_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    sprite->leftTopY = sprite->leftTopY + diffY;

    sprite->width += diffX;
    sprite->height -= diffY;
}

int right_bottom_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    sprite->width += diffX;
    sprite->height += diffY;
}

int top_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousY = mouseEvent->y;

    sprite->leftTopY = sprite->leftTopY + diffY;

    sprite->height -= diffY;
}

int bottom_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffY = mouseEvent->y - sprite->previousY;
    sprite->previousY = mouseEvent->y;

    sprite->height += diffY;
}

int left_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    sprite->leftTopX += diffX;
    sprite->width -= diffX;
}

int right_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    sprite->width += diffX;
}

void sprite_response_mouse_move(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    if (!sprite->isMousePressed)
        return;

    if (is_mouse_on_left_top_rect(sprite, mouseEvent)) {
        left_top_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_left_bottom_rect(sprite, mouseEvent)) {
        left_bottom_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_right_top_rect(sprite, mouseEvent)) {
        right_top_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_right_bottom_rect(sprite, mouseEvent)) {
        right_bottom_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_top_center(sprite, mouseEvent)) {
        top_center_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_bottom_center(sprite, mouseEvent)) {
        bottom_center_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_left_center(sprite, mouseEvent)) {
        left_center_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_right_center(sprite, mouseEvent)) {
        right_center_scale_sprite(sprite, mouseEvent);
    }
    else if (is_mouse_on_sprite(sprite, mouseEvent)) {
        move_sprite(sprite, mouseEvent);
    }
}

void sprite_response_mouse_press(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    sprite->isMousePressed = 1;
    if (is_mouse_on_left_top_rect(sprite, mouseEvent) ||
        is_mouse_on_left_bottom_rect(sprite, mouseEvent) ||
        is_mouse_on_right_top_rect(sprite, mouseEvent) ||
        is_mouse_on_right_bottom_rect(sprite, mouseEvent) ||
        is_mouse_on_sprite(sprite, mouseEvent) ||
        is_mouse_on_top_center(sprite, mouseEvent) ||
        is_mouse_on_bottom_center(sprite, mouseEvent) ||
        is_mouse_on_left_center(sprite, mouseEvent) ||
        is_mouse_on_right_center(sprite, mouseEvent)
        ){
        sprite->previousX = mouseEvent->x;
        sprite->previousY = mouseEvent->y;
    }
}

void sprite_response_mouse_up(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    sprite->isMousePressed = 0;
}