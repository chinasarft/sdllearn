#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SdlEvent.h"
#include <QDebug>
#include <QThread>
#define SDL_MAIN_HANDLED  
#include <SDL.h>
#include <SDL_image.h>

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    explicit WorkerThread(QObject *parent = 0)
        : QThread(parent)
    {
        qDebug() << "Worker Thread : " << QThread::currentThreadId();
    }
    void SetExit() { exit_ = 1; }

protected:
    virtual void run() Q_DECL_OVERRIDE {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        while (!exit_)
        {
            msleep(25);
            emit resultReady();
        }
        qDebug() << "Worker Thread quit: " << QThread::currentThreadId();
    }
signals:
    void resultReady();

private:
    int exit_ = 0;
};


class SdlEvent : public QMainWindow
{
    Q_OBJECT

public:
    SdlEvent(QWidget *parent = Q_NULLPTR);
    ~SdlEvent();

private:
    Ui::SdlEventClass ui;
    bool isSDLInitOk;
    int btnCnt;
    SDL_Window * window = nullptr;
    SDL_Renderer *renderer = nullptr;
    WorkerThread thread;
    SDL_Texture *image = nullptr;
    SDL_Texture *background = nullptr;
    int leftTopX = 0;
    int leftTopY = 0;
    bool mouseDown = false;
    int prex, prey;
    int transparentValue=180;

protected:
    void closeEvent(QCloseEvent *event);

private:
    /*
    * Loads an image into a texture on the rendering device
    * @param file The image file to load
    * @param ren The renderer to load the texture onto
    * @return the loaded texture, or nullptr if something went wrong.
    */
    SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);

    /*
    * Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
    * width and height
    * @param tex The source texture we want to draw
    * @param rend The renderer we want to draw too
    * @param x The x coordinate to draw too
    * @param y The y coordinate to draw too
    * @param w The width of the texture to draw
    * @param h The height of the texture to draw
    */
    void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);

    /*
    * Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
    * the texture's width and height
    * @param tex The source texture we want to draw
    * @param rend The renderer we want to draw too
    * @param x The x coordinate to draw too
    * @param y The y coordinate to draw too
    */
    void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);

    void cleanup(SDL_Window * win);
    void cleanup(SDL_Renderer * render);
    void cleanup(SDL_Texture* tex1, SDL_Texture * tex2);
    void logSDLError(std::string str);

    void enableBlend(); //transparent texture

private slots:
    void on_pushButton_clicked();
    void on_spinBox_valueChanged(int arg1);
    void slot_render();
};