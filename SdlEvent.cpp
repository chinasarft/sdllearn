#include "SdlEvent.h"

SdlEvent::SdlEvent(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    isSDLInitOk = false;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        isSDLInitOk = true;
        qDebug() << "SDL_Init Error: " << SDL_GetError();
    }
    init_sprite(&background);
    init_sprite(&direction);
    init_sprite(&image);
    memset(&canvas, 0, sizeof(Canvas));
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
    cleanup(direction.texture, NULL);
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

void SdlEvent::on_btnCatchCanvas_clicked()
{
    canvas.catchCanvas = 1;
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

    SDL_Renderer * r;
    r = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (r == nullptr) {
        logSDLError("CreateRenderer");
        return;
    }

    if (init_canvas(&canvas, r, 1280, 720, ui.label->width(), ui.label->height()) != 0) {
        qDebug() << "init_canvas fail";
        return;
    }
    set_canvas_pad_color(&canvas, 57, 58, 57);

    Uint32 winPf = SDL_GetWindowPixelFormat(window);
    qDebug() << "winPf:" << winPf << "pixel format name:"<<SDL_GetPixelFormatName(winPf);


    //The textures we'll be using
    const std::string resPath = "C:\\d\\render\\sdl_learn\\sdl_project\\SdlEvent\\";
    background.texture = loadTexture(resPath + "1.png", canvas.renderer);
    direction.texture = loadTexture(resPath + "direction.png", canvas.renderer);
    image.texture = loadTexture(resPath + "image.png", canvas.renderer);
    //Make sure they both loaded ok
    if (background.texture == nullptr || image.texture == nullptr || direction.texture == nullptr) {
        cleanup(background.texture, image.texture);
        return ;
    }

    Uint32 f1, f2, f3;
    SDL_QueryTexture(background.texture, &f1, NULL, &background.width, &background.height);
    SDL_QueryTexture(image.texture, &f3, NULL, &image.width, &image.height);
    SDL_QueryTexture(direction.texture, &f2, NULL, &direction.width, &direction.height);
    qDebug() << "background:" << SDL_GetPixelFormatName(f1) << background.width << background.height;
    qDebug() << "     image:" << SDL_GetPixelFormatName(f2) << direction.width << direction.height;
    qDebug() << " direction:" << SDL_GetPixelFormatName(f3) << image.width << image.height;

    add_sprite_to_canvas(&canvas, &background);
    add_sprite_to_canvas(&canvas, &image);
    add_sprite_to_canvas(&canvas, &direction);

    return ;
}

void SdlEvent::enableBlend()
{
    if(canvas.renderer != nullptr)
    SDL_SetRenderDrawBlendMode(canvas.renderer, SDL_BLENDMODE_BLEND);
    if (background.texture != nullptr) {
        SDL_SetTextureBlendMode(background.texture, SDL_BLENDMODE_BLEND);
    }
    if (direction.texture != nullptr) {
        SDL_SetTextureBlendMode(direction.texture, SDL_BLENDMODE_BLEND);
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
    if (canvas.renderer == nullptr) {
        qDebug() << "render................";
        return;
    }

    Sprite * selectedSprite = nullptr;
    
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
            canvas_response_mouse_press(&canvas, &e.motion);
            break;
        case SDL_MOUSEBUTTONUP:
            canvas_response_mouse_up(&canvas, &e.motion);
            qDebug() << "event mouseup===============";
            
            break;
        case SDL_MOUSEMOTION:
            canvas_response_mouse_move(&canvas, &e.motion);

            break;
        default:
            qDebug() << "event..........." << e.type;

        }
        //break;
    }

    enableBlend();

    draw_canvas(&canvas);
}