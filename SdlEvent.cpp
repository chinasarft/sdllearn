#include "SdlEvent.h"

SdlEvent::SdlEvent(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    isSDLInitOk = false;
    btnCnt = 0;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        isSDLInitOk = true;
        qDebug() << "SDL_Init Error: " << SDL_GetError();
    }
    memset(&background, 0, sizeof(background));
    memset(&image, 0, sizeof(image));
    //background.angle = 180;

    connect(&thread, SIGNAL(resultReady()), this, SLOT(slot_render()));
    thread.start();
}

SdlEvent::~SdlEvent()
{
    if (thread.isRunning()) {
        thread.SetExit();
        thread.wait(); //must wait
    }
    if (isSDLInitOk) {
        isSDLInitOk = false;
        SDL_Quit();
    }
    //Destroy the various items
    cleanup(background.texture, image.texture);
    if (window) {
        cleanup(window);
    }
}

void SdlEvent::closeEvent(QCloseEvent *event)
{
    qDebug() << "close............";
}

void SdlEvent::logSDLError(std::string str)
{
    qDebug() << str.c_str();
}

void SdlEvent::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, 
    const double angle, const SDL_Point *center, SDL_RendererFlip flip) {
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    int ret = 0;
    ret = SDL_RenderCopyEx(ren, tex, NULL, &dst, angle, 0, flip);
    if (ret != 0) {
        qDebug() << "SDL_RenderCopy Error: " << SDL_GetError();
    }
}

void SdlEvent::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    renderTexture(tex, ren, x, y, w, h,0, 0, SDL_FLIP_NONE);
}

SDL_Texture* SdlEvent::loadTexture(const std::string &file, SDL_Renderer *ren)
{
    SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
    if (texture == nullptr) {
        logSDLError("LoadTexture");
    }
    return texture;
}
void SdlEvent::cleanup(SDL_Window * win)
{
    if (win != nullptr) {
        SDL_DestroyWindow(win);
    }
}

void SdlEvent::cleanup(SDL_Renderer * render)
{
    if(render != nullptr)
        SDL_DestroyRenderer(render);
}

void SdlEvent::cleanup(SDL_Texture* tex1, SDL_Texture * tex2)
{
    if(tex1 != nullptr)
        SDL_DestroyTexture(tex1);
    if(tex2 != nullptr)
        SDL_DestroyTexture(tex2);
}

void SdlEvent::on_pushButton_clicked()
{
    //Setup our window and renderer
    if (window == nullptr) {
        window = SDL_CreateWindowFrom((HWND)ui.label->winId());
        if (window == nullptr) {
            logSDLError("CreateWindow");
            return;
        }
    }

    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) {
            logSDLError("CreateRenderer");
            return;
        }
    }

    //The textures we'll be using
    const std::string resPath = "C:\\d\\render\\sdl_learn\\sdl_project\\SdlEvent\\";
    background.texture = loadTexture(resPath + "background.png", renderer);
    image.texture = loadTexture(resPath + "image.png", renderer);
    //Make sure they both loaded ok
    if (background.texture == nullptr || image.texture == nullptr) {
        cleanup(background.texture, image.texture);
        return ;
    }

    SDL_QueryTexture(background.texture, NULL, NULL, &background.width, &background.height);
    background.leftTopX = 0;
    background.leftTopY = 0;
    return ;
}

void SdlEvent::enableBlend()
{
    if(renderer != nullptr)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (background.texture != nullptr) {
        SDL_SetTextureBlendMode(background.texture, SDL_BLENDMODE_BLEND);
    }
    if (image.texture != nullptr) {
        SDL_SetTextureBlendMode(image.texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(image.texture, transparentValue);
    }
}
void SdlEvent::on_spinBox_valueChanged(int arg1)
{
    qDebug() << "new value" << arg1;
    transparentValue = arg1;
}

void SdlEvent::slot_render()
{
    if (renderer == nullptr) {
        qDebug() << "render................";
        return;
    }
    
    SDL_Event e;
    //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            qDebug() << "event quit...........";
            break;
        case SDL_KEYDOWN:
            qDebug() << "event keydown...........";
            break;
        case SDL_MOUSEBUTTONDOWN:
            sprite_response_mouse_press(&background, &e.motion);
            break;
        case SDL_MOUSEBUTTONUP:
            sprite_response_mouse_up(&background, &e.motion);
            
            prex = 0;
            prey = 0;
            mouseDown = false;
            qDebug() << "event mouseup===============";
            
            break;
        case SDL_MOUSEMOTION:
            sprite_response_mouse_move(&background, &e.motion);

            break;
        default:
            qDebug() << "event..........." << e.type;

        }
        //break;
    }

    btnCnt++;
    int SCREEN_WIDTH = ui.label->width()/2;
    int SCREEN_HEIGHT = ui.label->height()/2;
    int TILE_SIZE = 40;

    //Clear the window
    SDL_RenderClear(renderer);

    //Determine how many tiles we'll need to fill the screen
    int xTiles = SCREEN_WIDTH / TILE_SIZE;
    int yTiles = SCREEN_HEIGHT / TILE_SIZE;

    //if (btnCnt % 2 == 0) {
    if (0) {
        //Draw the tiles by calculating their positions
        for (int i = 0; i < xTiles * yTiles; ++i) {
            int x = i % xTiles;
            int y = i / xTiles;
            renderTexture(background.texture, renderer, x * TILE_SIZE, 
                y * TILE_SIZE , TILE_SIZE, TILE_SIZE,
                background.angle, 0, SDL_FLIP_NONE);
        }
    }
    else if (1) {
        enableBlend();

        SDL_Rect rect{ 400, 400, 100, 100 };
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, r, g, b, a);


        renderTexture(background.texture, renderer, background.leftTopX, 
            background.leftTopY, background.width, background.height,
            background.angle, 0, background.flip);

        renderTexture(image.texture, renderer, 0, 0);
        auto p = SDL_Point{400, 400};
        renderTexture(image.texture, renderer, 300, 300, 100, 100, 180, &p, SDL_FLIP_NONE);


        draw_select_sprite(&background, renderer);
    }
    else {

        //Draw our image in the center of the window
        //We need the foreground image's width to properly compute the position
        //of it's top left corner so that the image will be centered
        int iW, iH;
        SDL_QueryTexture(image.texture, NULL, NULL, &iW, &iH);
        int x = SCREEN_WIDTH / 2 - iW / 2;
        int y = SCREEN_HEIGHT / 2 - iH / 2;
        renderTexture(image.texture, renderer, x, y);
    }

    //Update the screen
    SDL_RenderPresent(renderer);
}