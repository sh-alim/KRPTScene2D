#include "MainProcess.h"

MainProcess::MainProcess(QWidget *parent)
    : QMainWindow(parent)
{
//    _sceneWidget = new KRPTSceneWidget(this);
//    setCentralWidget(_sceneWidget);

    _view = new KRPTSceneView(this);

    _scene = new KRPTScene(_view);

    setGeometry(300, 100, 2000, 1300);

    auto _root = _scene->addItem<KRPTSceneRectItem>();
    _root->setGeometry(QRectF(10, 10, 1000, 1000));

    _root->addMust
    (
//            KRPTSceneItem::Must::NoClipChilds,
//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
            KRPTSceneItem::Must::MouseMoveble,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
    );

}

MainProcess::~MainProcess()
{
    delete _scene;
}

void MainProcess::resizeEvent(QResizeEvent *value)
{
    _view->setGeometry(100, 100, width() - 200, height() - 200);
}
