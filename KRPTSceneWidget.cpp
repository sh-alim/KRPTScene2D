//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneWidget.h"

//####################################################################################################
//#
//####################################################################################################

KRPTSceneWidget::KRPTSceneWidget(QWidget *parent) noexcept
    : QWidget(parent), _scene(nullptr)
{
    _scene = new KRPTScene(this);

//    setAttribute(Qt::WA_DontShowOnScreen);
//    show();

#if 1
    _item = _scene->addItem<KRPTSceneRectItem>();
    _item->setGeometry(QRectF(10, 10, 1000, 1000));

    _item->addMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
//            KRPTSceneItem::Must::NoMouseEventTranslate,
            KRPTSceneItem::Must::AccuracyClip,
 
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
//        ,KRPTSceneItem::Must::NoCheckChildVisibled
        );

    int x = 0;
    int y = 0;
//    for(int i = 0; i < 100000; ++i)
    for(int i = 0; i < 1; ++i)
    {
//        item->addMust(
//            SceneItem::Must::ClipChilds,
//            SceneItem::Must::MousePressEvent
//        );

//        auto child = _item->addChild<KRPTSceneEllipseItem>();
        auto child = _item->addChild<KRPTSceneRectItem>();
        child->setGeometry(QRectF(x, y, 50, 50));
        child->setBackgroundColor(QColor(0, 255, 0));

        child->addMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
            KRPTSceneItem::Must::NoSceneRotate,
            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
        );
        x += 25;
        if((i % 1000) == 0)
        {
            x = 0;
            y += 25;
        }

//        child->setAngle(i * 10);
//        child->setScale(i * 0.5);
    #if 0
        int x1 = 0;
        int y1 = 20;
        for(int i = 0; i < 100000; ++i)
//        for(int i = 0; i < 10; ++i)
        {
            if((i % 10) == 0)
            {
                x1 = 0;
                y1 += 50;
            }

            auto child1 = child->addChild<KRPTSceneItem>();
            child1->addMust(
//            KRPTSceneItem::Must::ClipChilds,
//                KRPTSceneItem::Must::NoScale,
                KRPTSceneItem::Must::MousePressEvent,
                KRPTSceneItem::Must::MouseMoveEvent,
                KRPTSceneItem::Must::WhellEvent
            );
//            child1->setAngle(i * 10);
            child1->setGeometry(QRectF(x1, y1, 50, 50));

            if(i == 9)
                child1->_ch = true;

            x1 += 50;
//            qDebug() << (i % 10);


        }
    #endif
    }
#else

    #if 0
    _item = _scene->addItem<KRPTSceneItem>();
    _item->setGeometry(QRectF(10, 10, 600, 600));
    _item->addMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
            KRPTSceneItem::Must::WhellEvent,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent
        );

    auto item = _item->addChild<KRPTSceneItem>();
    item->setGeometry(QRectF(10, 10, 400, 400));
    item->addMust
    (
//        KRPTSceneItem::Must::NoClipChilds,
        KRPTSceneItem::Must::WhellEvent,
        KRPTSceneItem::Must::MousePressEvent,
        KRPTSceneItem::Must::MouseMoveEvent
    );

    item = item->addChild<KRPTSceneItem>();
    item->setGeometry(QRectF(10, 10, 200, 200));
    item->addMust
    (
//        KRPTSceneItem::Must::NoClipChilds,
        KRPTSceneItem::Must::WhellEvent,
        KRPTSceneItem::Must::MousePressEvent,
        KRPTSceneItem::Must::MouseMoveEvent
    );

    item = item->addChild<KRPTSceneItem>();
    item->setGeometry(QRectF(10, 10, 100, 100));
    item->addMust
    (
//        KRPTSceneItem::Must::NoClipChilds,
        KRPTSceneItem::Must::WhellEvent,
        KRPTSceneItem::Must::MousePressEvent,
        KRPTSceneItem::Must::MouseMoveEvent
    );
    #endif

#endif

}

KRPTSceneWidget::~KRPTSceneWidget() noexcept
{
    delete _scene;
}

//****************************************************************************************************
//*
//****************************************************************************************************

#if 0
void KRPTSceneWidget::setScene(KRPTScene *scene) noexcept
{
//    if(_scene)delete scene;
//    _scene = new KRPTScene(this);;
}
#endif

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneWidget::resizeEvent(QResizeEvent *e)
{
    if(!_scene)return;
//    _scene->setGeometry(QRectF(0, 0, width(), height()));

    _scene->setGeometry(QRectF(100, 100, width() - 200, height() - 200));

}

void KRPTSceneWidget::mousePressEvent(QMouseEvent *e)
{
    if(!_scene)return;
    auto event = createMouseSceneEvent(e);
    _scene->mousePressEvent(event.get());
}

void KRPTSceneWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(!_scene)return;
    auto event = createMouseSceneEvent(e);
    _scene->mouseReleaseEvent(event.get());
}

void KRPTSceneWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(!_scene)return;
    auto event = createMouseSceneEvent(e);
    _scene->mouseMoveEvent(event.get());
}

void KRPTSceneWidget::wheelEvent(QWheelEvent *e)
{
    if(!_scene)return;
    auto event = createWheelSceneEvent(e);
    _scene->whellEvent(event.get());
}

void KRPTSceneWidget::paintEvent(QPaintEvent *e)
{
    if(!_scene)return;
    QPainter painter(this);
    _scene->paintEvent(painter);
}

//****************************************************************************************************
//*
//****************************************************************************************************

SceneMouseEvent::Ptr KRPTSceneWidget::createMouseSceneEvent(QSinglePointEvent *e) noexcept
{
    SceneMouseEvent::Btns btns;
    if(e->buttons() & Qt::LeftButton  )btns += SceneMouseEvent::Btn::Left  ;
    if(e->buttons() & Qt::RightButton )btns += SceneMouseEvent::Btn::Right ;
    if(e->buttons() & Qt::MiddleButton)btns += SceneMouseEvent::Btn::Middle;
    SceneMouseEvent::KeyModifers keyModifers;
    if(e->modifiers() & Qt::KeyboardModifier::ControlModifier)keyModifers += SceneMouseEvent::KeyModifer::Ctrl ;
    if(e->modifiers() & Qt::KeyboardModifier::ShiftModifier  )keyModifers += SceneMouseEvent::KeyModifer::Shift;
    if(e->modifiers() & Qt::KeyboardModifier::AltModifier    )keyModifers += SceneMouseEvent::KeyModifer::Alt  ;

    return SceneMouseEvent::get(e->position(), btns, QPointF(), keyModifers);
}

SceneMouseEvent::Ptr KRPTSceneWidget::createWheelSceneEvent(QWheelEvent *e) noexcept
{
    auto ret = createMouseSceneEvent(e);
    ret->setDelta(e->angleDelta());
    return ret;
}
