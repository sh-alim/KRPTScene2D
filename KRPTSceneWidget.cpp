//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneWidget.h"

#if 0
//####################################################################################################
//#
//####################################################################################################

KRPTSceneWidget::KRPTSceneWidget(QWidget *parent) noexcept
    : QWidget(parent), _scene(nullptr)
{
    _scene = new KRPTScene(this);
//    setAttribute(Qt::WA_DontShowOnScreen);

#if 0
    _root = _scene->addItem<KRPTSceneRectItem>();
    _root->setGeometry(QRectF(10, 10, 1000, 1000));
    _root->addMust
    (
            KRPTSceneItem::Must::Anim,
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
    for(int i = 0; i < 50000; ++i)
//    for(int i = 0; i < 20; ++i)
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

#else
//####################################################################################################
//#
//####################################################################################################

#include <QOpenGLWidget>
//#include <QtGui/private/qopengl2paintengineex_p.h>
//D:\projects\libs\qt\6.8.0\win\include\QtGui\6.8.0\QtGui\private
//#include <QtGui/private/qpaintengineex_p.h>
//#include <QtOpenGL/private/qopenglpaintengine_p.h>


KRPTSceneWidget::KRPTSceneWidget(QWidget *parent) noexcept
    : QGraphicsView(parent), _scene(nullptr)
{
#if 0
    _scene = new KRPTScene(this);
//    setAttribute(Qt::WA_DontShowOnScreen);
    QSurfaceFormat fmt;
    fmt.setSamples(4);
    fmt.setSwapInterval(0);

    QSurfaceFormat::setDefaultFormat(fmt);

    setViewport(new QOpenGLWidget);

//    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);


    setAttribute(Qt::WA_NoMouseReplay);
    viewport()->setAttribute(Qt::WA_NoMouseReplay);
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
#endif

#if 0
    _root = _scene->addItem<KRPTSceneRectItem>();
    _root->setGeometry(QRectF(10, 10, 1000, 1000));
    _root->addMust
    (
            KRPTSceneItem::Must::Anim,
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


#else

    _item = _scene->addItem<KRPTSceneRectItem>();
    _item->setGeometry(QRectF(10, 10, 2000, 1000));
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
    for(int i = 0; i < 5000; ++i)
//    for(int i = 0; i < 20; ++i)
    {
        auto child = _item->addChild<KRPTSceneEllipseItem>();
        child->setGeometry(QRectF(x, y, 10, 10));
        child->setBackgroundColor(QColor(0, 255, 0));

//        child->setScale(0.8);
//        child->setAngle(12);

        child->setTag(i + 1);

        child->addMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
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

        x += child->width() + 5;
//        if( ((i + 1) % 200) == 0)
        if( x > _item->width())
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

//    setSceneRect(0, 0, width(), height());
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

#if 1
void KRPTSceneWidget::paintEvent(QPaintEvent *e)
{
    if(!_scene)return;
//    QPainter painter(this);
    QGraphicsView::paintEvent(e);
    QPainter painter(viewport());
    _scene->paintEvent(painter);
}
#endif

void KRPTSceneWidget::update() 
{
    viewport()->update();
//    viewport()->repaint();
}

//void KRPTSceneWidget::drawForeground(QPainter* p, const QRectF&)
//{
//    if(!_scene)return;
//    qDebug() << "===============";
//}

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
#endif