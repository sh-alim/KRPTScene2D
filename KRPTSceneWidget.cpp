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

#if 1
    _root = _scene->addItem<KRPTSceneRectItem>();
    _root->setGeometry(QRectF(10, 10, 1000, 1000));
    _root->addMust
    (
//            KRPTSceneItem::Must::NoClipChilds,
//            KRPTSceneItem::Must::NoMouseEventTranslate,
//            KRPTSceneItem::Must::AccuracyClip,
 
            KRPTSceneItem::Must::MouseMoveble,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
//        ,KRPTSceneItem::Must::NoCheckChildVisibled
    );
    _root->setTag(_tag++);


    _item = _root->addChild<KRPTSceneRectItem>();
    _item->setGeometry(QRectF(50, 50, 800, 800));
    _item->addMust
    (
//            KRPTSceneItem::Must::NoClipChilds,
//            KRPTSceneItem::Must::NoMouseEventTranslate,
//            KRPTSceneItem::Must::AccuracyClip,
 
            KRPTSceneItem::Must::MouseMoveble,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
//        ,KRPTSceneItem::Must::NoCheckChildVisibled
    );
    _item->setTag(_tag++);

    auto child = _item->addChild<KRPTSceneRectItem>();
    child->setGeometry(QRectF(5, 5, 150, 150));
    child->setBackgroundColor(QColor(0, 255, 0));

    child->setTag(_tag++);

    child->addMust
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


#else

    _item = _scene->addItem<KRPTSceneRectItem>();
    _item->setGeometry(QRectF(10, 10, 1000, 1000));
    _item->addMust
    (
//            KRPTSceneItem::Must::NoClipChilds,
//            KRPTSceneItem::Must::NoMouseEventTranslate,
//            KRPTSceneItem::Must::AccuracyClip,
            KRPTSceneItem::Must::MouseMoveble,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
//        ,KRPTSceneItem::Must::NoCheckChildVisibled
    );

    int x = 5;
    int y = 5;
    for(int i = 0; i < 100000; ++i)
//    for(int i = 0; i < 100; ++i)
    {
        auto child = _item->addChild<KRPTSceneRectItem>();
        child->setGeometry(QRectF(x, y, 50, 50));
        child->setBackgroundColor(QColor(0, 255, 0));

//        child->setScale(0.8);
//        child->setAngle(12);

        child->setTag(i + 1);

        child->addMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
//            KRPTSceneItem::Must::NoSceneRotate,
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

        x += child->width() + 5;
        if( ((i + 1) % 1000) == 0)
        {
            x = 5;
            y += child->height();
        }



    }


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

    if(event->btns()[SceneMouseEvent::Btn::Right])
    {

    auto child = _item->addChild<KRPTSceneRectItem>();
    child->setGeometry(QRectF(50, 50, 150, 150));
    child->setBackgroundColor(QColor(0, 255, 0));

    child->setTag(_tag++);

    child->addMust
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
