#include "SdlEvent.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SdlEvent w;
    w.show();
    return a.exec();
}
