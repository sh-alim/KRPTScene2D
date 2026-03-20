//########################################################################################################################
//#
//########################################################################################################################

#include "MainProcess.h"

//########################################################################################################################
//#
//########################################################################################################################

MainProcess::MainProcess(QWidget *parent)
    : QMainWindow(parent)
{
//    _sceneWidget = new KRPTSceneWidget(this);
//    setCentralWidget(_sceneWidget);

    _view = new KRPTSceneView(this);
    _scene = new KRPTScene(_view);
    _view->setTranslateEvents(true);

    setGeometry(400, 50, 2000, 1000);

    auto _root = _scene->addItem<KRPTSceneRectItem>();
    _root->setGeometry(QRectF(10, 10, 1000, 1200));

    _root->addMust
    (
            KRPTSceneItem::Must::NoClipPainter,
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

    auto child = _root->addChild<KRPTSceneRectItem>();
    child->setGeometry(QRectF(50, 50, 150, 150));
    child->setBackgroundColor(QColor(0, 255, 0));

    child->addMust
    (
//            KRPTSceneItem::Must::NoClipChilds,
            KRPTSceneItem::Must::NoClipPainter,
 
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

    child->setTag(1);
}

MainProcess::~MainProcess()
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void MainProcess::resizeEvent(QResizeEvent *value)
{
    _view->setGeometry(200, 10, width() - 210, height() - 20);
}

void MainProcess::mousePressEvent(QMouseEvent *e)
{
//    const auto &items = _scene->selectedItems();
//    if(items.empty())return;
//    auto item = items.front();
//    item->setBorderColor(QColor(255, 0, 0));

    if(e->buttons() & Qt::MouseButton::RightButton)
    {
        auto item = _scene->mousePressedItem();
        if(!item)return;
        QPointF p = item->mapFromScene(e->position());
        auto child = item->addChild<KRPTSceneRectItem>();
        child->setGeometry(p, QSizeF(50, 50));
        child->setBackgroundColor(QColor(0, 255, 0));
        child->addMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
            KRPTSceneItem::Must::NoClipPainter,
            KRPTSceneItem::Must::NoSceneRotate,
            KRPTSceneItem::Must::NoSceneScale,
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

}

void MainProcess::mouseReleaseEvent(QMouseEvent *e)
{
}

void MainProcess::mouseMoveEvent(QMouseEvent *e)
{
}

void MainProcess::wheelEvent(QWheelEvent *e)
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************
