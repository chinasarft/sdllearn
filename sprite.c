#include "sprite.h"
int is_mouse_on_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    return  (mouseEvent->x > sprite->leftTopX) && (mouseEvent->x < sprite->leftTopX + sprite->width)
        && (mouseEvent->y > sprite->leftTopY) && (mouseEvent->y < sprite->leftTopY + sprite->height);
}

static inline void set_flip_angle(Sprite * sprite, SDL_RendererFlip flip)
{
    if (sprite->angle > 179.9) {
        if (flip == SDL_FLIP_HORIZONTAL) {
            sprite->angle = 0;
            sprite->flip = SDL_FLIP_VERTICAL;
        }
        else if (flip == SDL_FLIP_VERTICAL) {
            sprite->angle = 0;
            sprite->flip = SDL_FLIP_HORIZONTAL;
        }
        else {
            sprite->flip = SDL_FLIP_NONE;
        }
    }
    else {
        if (sprite->flip == SDL_FLIP_NONE)
            sprite->flip = flip;
        else if (sprite->flip != flip) {
            sprite->flip = SDL_FLIP_NONE;
            sprite->angle = 180;
        }
        else {
            sprite->flip = SDL_FLIP_NONE;
        }
    }
}

static inline void set_angle_flip(Sprite * sprite, double angle)
{
    sprite->angle -= angle;
    if (sprite->flip != SDL_FLIP_NONE) {
        if (sprite->angle > 179.9) {
            if (sprite->flip == SDL_FLIP_HORIZONTAL) {
                sprite->flip = SDL_FLIP_VERTICAL;
            }
            else if (sprite->flip == SDL_FLIP_VERTICAL) {
                sprite->flip = SDL_FLIP_HORIZONTAL;
            }
        }
    }
}

static int is_mouse_on_left_rect_x(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    if ((mouseEvent->x > sprite->leftTopX - RADIUS(sprite)) && (mouseEvent->x < sprite->leftTopX + RADIUS(sprite)))
        return 1;
    return 0;
}

static int is_mouse_on_middle_rect_x(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    if ((mouseEvent->x > sprite->leftTopX + sprite->width / 2 - RADIUS(sprite)) &&
        (mouseEvent->x < sprite->leftTopX + sprite->width / 2 + RADIUS(sprite)))
        return 2;
    return 0;
}

static int is_mouse_on_right_rect_x(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    if ((mouseEvent->x > sprite->leftTopX + sprite->width - RADIUS(sprite)) &&
        (mouseEvent->x < sprite->leftTopX + sprite->width + RADIUS(sprite)))
        return 3;
    return 0;
}

static int is_mouse_on_rect_x(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int x;
    if (x = is_mouse_on_left_rect_x(sprite, mouseEvent))
        return x;
    else if ((x = is_mouse_on_middle_rect_x(sprite, mouseEvent)))
        return x;
    else if ((x = is_mouse_on_right_rect_x(sprite, mouseEvent)))
        return x;
    return 0;
}

static int is_mouse_on_top_rect_y(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{

    if ((mouseEvent->y > sprite->leftTopY - RADIUS(sprite)) && (mouseEvent->y < sprite->leftTopY + RADIUS(sprite)))
        return 5;
    return 0;
}

static int is_mouse_on_middle_rect_y(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{

    if ((mouseEvent->y > sprite->leftTopY + sprite->height / 2 - RADIUS(sprite)) &&
        (mouseEvent->y < sprite->leftTopY + sprite->height / 2 + RADIUS(sprite)))
        return 6;
    return 0;
}

static int is_mouse_on_bottom_rect_y(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    if ((mouseEvent->y > sprite->leftTopY + sprite->height - RADIUS(sprite)) &&
        (mouseEvent->y < sprite->leftTopY + sprite->height + RADIUS(sprite)))
        return 7;
    return 0;
}

static int is_mouse_on_rect_y(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int y;
    if (y = is_mouse_on_top_rect_y(sprite, mouseEvent))
        return y;
    else if ((y = is_mouse_on_middle_rect_y(sprite, mouseEvent)))
        return y;
    else if ((y = is_mouse_on_bottom_rect_y(sprite, mouseEvent)))
        return y;
    return 0;
}

static int locate_scale_rect(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int x = is_mouse_on_rect_x(sprite, mouseEvent);
    if (x == 0)
        return 0;
    int y = is_mouse_on_rect_y(sprite, mouseEvent);
    return x * y;
}

static int top_center_to_bottom_flip(Sprite * sprite, int diffY)
{
        set_flip_angle(sprite, SDL_FLIP_VERTICAL);
        sprite->leftTopY= sprite->leftTopY + sprite->height;
        sprite->height = diffY - sprite->height;
        sprite->selectedRect = BOTTOM_CENTER_RECT;
}

static void bottom_center_to_top_flip(Sprite * sprite, int diffY)
{
        set_flip_angle(sprite, SDL_FLIP_VERTICAL);
        sprite->height = -diffY - sprite->height;
        sprite->leftTopY -= sprite->height;
        sprite->selectedRect = TOP_CENTER_RECT;
}

static void left_center_to_right_flip(Sprite * sprite, int diffX)
{
        set_flip_angle(sprite, SDL_FLIP_HORIZONTAL);
        sprite->leftTopX += sprite->width;
        sprite->width = diffX - sprite->width;
        sprite->selectedRect = RIGHT_CENTER_RECT;
}

static void right_center_to_left_flip(Sprite * sprite, int diffX)
{
        set_flip_angle(sprite, SDL_FLIP_HORIZONTAL);
        sprite->width = -diffX - sprite->width;
        sprite->leftTopX -= sprite->width;
        sprite->selectedRect = LEFT_CENTER_RECT;
}

void init_sprite(Sprite * sprite)
{
    memset(sprite, 0, sizeof(Sprite));
    sprite->canvasIdx = -1;
    sprite->scaleRatio = 1.0;
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

    int diffWidth = sprite->width - diffX;
    int diffHeight = sprite->height - diffY;

    if (diffWidth > 0 && diffHeight > 0) {
        move_sprite(sprite, mouseEvent);
        sprite->width -= diffX;
        sprite->height -= diffY;
        return 0;
    }

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    if (diffWidth < 0 && diffHeight < 0) {
        set_angle_flip(sprite, 180);

        sprite->leftTopX = sprite->leftTopX + sprite->width;
        sprite->leftTopY = sprite->leftTopY + sprite->height;

        sprite->width = diffX - sprite->width;
        sprite->height = diffY - sprite->height;
        sprite->selectedRect = RIGHT_BOTTOM_RECT;
    }
    else if (diffWidth < 0){
        left_center_to_right_flip(sprite, diffX);
        sprite->leftTopY += diffY;
        sprite->height -= diffY;
        sprite->selectedRect = RIGHT_TOP_RECT;
    }
    else if (diffHeight < 0) {
        top_center_to_bottom_flip(sprite, diffY);
        sprite->leftTopX += diffX;
        sprite->width -= diffX;
        sprite->selectedRect = LEFT_BOTTOM_RECT;
    }
}

int left_bottom_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    int diffWidth = sprite->width - diffX;
    int diffHeight = sprite->height + diffY;

    if (diffWidth < 0 && diffHeight < 0) {
        set_angle_flip(sprite, 180);

        sprite->leftTopX += sprite->width;
        sprite->width = diffX - sprite->width;

        sprite->height = -diffY - sprite->height;
        sprite->leftTopY += sprite->height;

        sprite->selectedRect = RIGHT_TOP_RECT;
    }
    else if (diffWidth < 0){
        right_center_to_left_flip(sprite, diffX);
        sprite->height += diffY;
        sprite->selectedRect = RIGHT_BOTTOM_RECT;
    }
    else if (diffHeight < 0) {
        bottom_center_to_top_flip(sprite, diffY);
        sprite->leftTopX += diffX;
        sprite->width -= diffX;
        sprite->selectedRect = LEFT_TOP_RECT;
    }
    else {
        sprite->leftTopX = sprite->leftTopX + diffX;
        sprite->width -= diffX;
        sprite->height += diffY;
    }
}

int right_top_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    int diffWidth = sprite->width + diffX;
    int diffHeight = sprite->height - diffY;

    if (diffWidth < 0 && diffHeight < 0) {
        set_angle_flip(sprite, 180);

        sprite->width = -diffX - sprite->width;
        sprite->leftTopX -= sprite->width;

        sprite->leftTopY += sprite->height;
        sprite->height = diffY - sprite->height;

        sprite->selectedRect = LEFT_BOTTOM_RECT;
    }
    else if (diffWidth < 0) {
        left_center_to_right_flip(sprite, diffX);
        sprite->height -= diffY;
        sprite->selectedRect = LEFT_TOP_RECT;
    }
    else if (diffHeight < 0) {
        top_center_to_bottom_flip(sprite, diffY);
        sprite->width += diffX;
        sprite->selectedRect = RIGHT_BOTTOM_RECT;
    }
    else {
        sprite->leftTopY = sprite->leftTopY + diffY;
        sprite->width += diffX;
        sprite->height -= diffY;
    }
}

int right_bottom_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    if ((sprite->width + diffX < 0) && (sprite->height + diffY) < 0) {
        set_angle_flip(sprite, 180);

        sprite->width = -diffX - sprite->width;
        sprite->height = -diffY - sprite->height;

        sprite->leftTopX += sprite->width;
        sprite->leftTopY += sprite->height;

        sprite->selectedRect = LEFT_TOP_RECT;
    }
    else if (sprite->width + diffX < 0){
        right_center_to_left_flip(sprite, diffX);
        sprite->height += diffY;
        sprite->selectedRect = LEFT_BOTTOM_RECT;
    }
    else if (sprite->height + diffY < 0) {
        bottom_center_to_top_flip(sprite, diffY);
        sprite->width += diffX;
        sprite->selectedRect = RIGHT_TOP_RECT;
    }
    else {
        sprite->width += diffX;
        sprite->height += diffY;
    }
}

int top_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousY = mouseEvent->y;

    // vertical flip
    if( (sprite->height - diffY) < 0){
        top_center_to_bottom_flip(sprite, diffY);
    }
    else {
        sprite->leftTopY = sprite->leftTopY + diffY;
        sprite->height -= diffY;
    }
}

int bottom_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffY = mouseEvent->y - sprite->previousY;

    // vertical flip
    if ((sprite->height + diffY) < 0) {
        bottom_center_to_top_flip(sprite, diffY);
    }
    else {
        sprite->previousY = mouseEvent->y;
        sprite->height += diffY;
    }
}

int left_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    //horizon flip
    if ((sprite->width - diffX) < 0) {
        left_center_to_right_flip(sprite, diffX);
    }
    else {
        sprite->leftTopX += diffX;
        sprite->width -= diffX;
    }
}

int right_center_scale_sprite(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    int diffX = mouseEvent->x - sprite->previousX;
    int diffY = mouseEvent->y - sprite->previousY;

    sprite->previousX = mouseEvent->x;
    sprite->previousY = mouseEvent->y;

    //horizon flip
    if ((sprite->width + diffX) < 0) {
        right_center_to_left_flip(sprite, diffX);
    }
    else {
        sprite->width += diffX;
    }
}

void sprite_response_mouse_move(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    if (!sprite->isSelected)
        return;

    switch (sprite->selectedRect) {
    case LEFT_TOP_RECT:
        left_top_scale_sprite(sprite, mouseEvent);
        break;
    case LEFT_BOTTOM_RECT:
        left_bottom_scale_sprite(sprite, mouseEvent);
        break;
    case RIGHT_TOP_RECT:
        right_top_scale_sprite(sprite, mouseEvent);
        break;
    case RIGHT_BOTTOM_RECT:
        right_bottom_scale_sprite(sprite, mouseEvent);
        break;
    case TOP_CENTER_RECT:
        top_center_scale_sprite(sprite, mouseEvent);
        break;
    case BOTTOM_CENTER_RECT:
        bottom_center_scale_sprite(sprite, mouseEvent);
        break;
    case LEFT_CENTER_RECT:
        left_center_scale_sprite(sprite, mouseEvent);
        break;
    case RIGHT_CENTER_RECT:
        right_center_scale_sprite(sprite, mouseEvent);
        break;
    case MOVE_RECT:
        move_sprite(sprite, mouseEvent);
        break;
    }

}

void sprite_response_mouse_press(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    sprite->selectedRect = locate_scale_rect(sprite, mouseEvent);
    if(sprite->selectedRect == MIDDLE_RECT)
        sprite->selectedRect = MOVE_RECT;
    else if(sprite->selectedRect == NONE_RECT){
        if (is_mouse_on_sprite(sprite, mouseEvent)) {
            sprite->selectedRect = MOVE_RECT;
        }
    }

    if(sprite->selectedRect > NONE_RECT){
        sprite->previousX = mouseEvent->x;
        sprite->previousY = mouseEvent->y;
        sprite->isSelected = 1;
    }
}

void sprite_response_mouse_up(Sprite * sprite, SDL_MouseMotionEvent *mouseEvent)
{
    sprite->selectedRect = NONE_RECT;
}

void draw_select_sprite(SDL_Rect * rect, SDL_Renderer *renderer)
{
//    SDL_Rect rect = {sprite->leftTopX, sprite->leftTopY, sprite->width, sprite->height};
    int bc = 20;
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_Rect rect0 = { rect->x - 1, rect->y - 1, rect->w + 2, rect->h + 2 };
    SDL_RenderDrawRect(renderer, &rect0);

    //lefttop
    SDL_Rect rect1 = { rect->x - bc/2, rect->y - bc/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect1);
    //righttop
    SDL_Rect rect2 = { rect->x + rect->w - bc/2, rect->y - bc/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect2);

    //leftbottom
    SDL_Rect rect3 = { rect->x  - bc/2, rect->y + rect->h - bc/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect3);
    //rightbottom
    SDL_Rect rect4 = { rect->x + rect->w - bc/2, rect->y + rect->h - bc/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect4);

    //leftcenter
    SDL_Rect rect5 = { rect->x - bc/2, rect->y - bc/2 + rect->h/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect5);
    //rightcenter
    SDL_Rect rect6 = { rect->x + rect->w - bc/2, rect->y - bc/2 + rect->h/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect6);

    //topcenter
    SDL_Rect rect7 = { rect->x + rect->w/2  - bc/2, rect->y - bc/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect7);
    //bottomcenter
    SDL_Rect rect8 = { rect->x + rect->w/2 - bc/2, rect->y + rect->h - bc/2, bc, bc};
    SDL_RenderDrawRect(renderer, &rect8);


    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void sprite_set_scale_ratio(Sprite * sprite, float ratio)
{
    sprite->scaleRatio = ratio;
}
