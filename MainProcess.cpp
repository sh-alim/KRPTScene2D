#include "MainProcess.h"

MainProcess::MainProcess(QWidget *parent)
    : QMainWindow(parent)
{
    _sceneWidget = new KRPTSceneWidget(this);
    setCentralWidget(_sceneWidget);
    setGeometry(300, 100, 2000, 1300);
}

MainProcess::~MainProcess()
{
}

void MainProcess::resizeEvent(QResizeEvent *value)
{
}
