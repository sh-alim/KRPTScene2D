#include "MainProcess.h"
#include <QtWidgets/QApplication>

//#include <vld.h>

int main(int argc, char *argv[])
{
//    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
//    QCoreApplication::setAttribute(Qt::AA_Use96Dpi);

    qputenv("QT_SCALE_FACTOR", "1");
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_SCREEN_SCALE_FACTORS", "1");


    QApplication a(argc, argv);
    MainProcess w;
    w.show();
    return a.exec();
}
