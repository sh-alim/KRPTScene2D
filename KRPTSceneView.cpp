//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneView.h"
#include "KRPTScene.h"
#include <QGraphicsView>
#include <QSurfaceFormat>
#include <QOpenGLWidget>

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneViewPriv : public QGraphicsView
{
friend class KRPTSceneView;
public:
    KRPTSceneViewPriv(QWidget *parent, KRPTSceneView *owner) noexcept;
    ~KRPTSceneViewPriv()               noexcept;
private:
    void resizeEvent      (QResizeEvent *e) override;
    void mousePressEvent  (QMouseEvent  *e) override;
    void mouseReleaseEvent(QMouseEvent  *e) override;
    void mouseMoveEvent   (QMouseEvent  *e) override;
    void wheelEvent       (QWheelEvent  *e) override;
    void paintEvent       (QPaintEvent  *e) override;
private:
    KRPTSceneView *_owner;
};

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneViewPriv::KRPTSceneViewPriv(QWidget *parent, KRPTSceneView *owner) noexcept
    : QGraphicsView(parent), _owner(owner)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setMouseTracking(false);
    setCacheMode(QGraphicsView::CacheNone);
}

KRPTSceneViewPriv::~KRPTSceneViewPriv() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneViewPriv::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    _owner->resizeEventImpl(e);
}

void KRPTSceneViewPriv::mousePressEvent(QMouseEvent *e)
{
    QGraphicsView::mousePressEvent(e);
    _owner->mousePressEventImpl(e);
    if(_owner->_translateEvents && parentWidget())
        QCoreApplication::sendEvent(parentWidget(), e);
}

void KRPTSceneViewPriv::mouseReleaseEvent(QMouseEvent *e)
{
    QGraphicsView::mouseReleaseEvent(e);
    _owner->mouseReleaseEventImpl(e);
    if(_owner->_translateEvents && parentWidget())
        QCoreApplication::sendEvent(parentWidget(), e);
}

void KRPTSceneViewPriv::mouseMoveEvent(QMouseEvent *e)
{
    QGraphicsView::mouseMoveEvent(e);
    _owner->mouseMoveEventImpl(e);
    if(_owner->_translateEvents && parentWidget())
        QCoreApplication::sendEvent(parentWidget(), e);
}

void KRPTSceneViewPriv::wheelEvent(QWheelEvent *e)
{
    QGraphicsView::wheelEvent(e);
    _owner->wheelEventImpl(e);
    if(_owner->_translateEvents && parentWidget())
        QCoreApplication::sendEvent(parentWidget(), e);
}

void KRPTSceneViewPriv::paintEvent(QPaintEvent *e)
{
    QGraphicsView::paintEvent(e);
    _owner->paintEventImpl(e);
}

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneView::KRPTSceneView(QWidget *parent, KRPTScene *scene) noexcept
    : QObject(parent), _scene(scene), _translateEvents(false)
{
    _p = new KRPTSceneViewPriv(parent, this);
    _p->show();
    QSurfaceFormat fmt;
    fmt.setSamples(8);
    fmt.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(fmt);
    _p->setViewport(new QOpenGLWidget);
}

KRPTSceneView::~KRPTSceneView() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

QRectF KRPTSceneView::geometry() const noexcept
{
    return _p->geometry();
}

KRPTScene *KRPTSceneView::scene() const noexcept
{
    return _scene;
}

bool KRPTSceneView::translateEvents() const noexcept
{
    return _translateEvents;
}

void KRPTSceneView::setGeometry(const QRect &geometry) noexcept
{
    _p->setGeometry(geometry); 
}

void KRPTSceneView::setGeometry(int ax, int ay, int aw, int ah) noexcept
{
    setGeometry(QRect(ax, ay, aw, ah)); 
}

void KRPTSceneView::setScene(KRPTScene *scene) noexcept
{
    _scene = scene;
}

void KRPTSceneView::setTranslateEvents(bool translate) noexcept
{
    _translateEvents = translate;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneView::update() noexcept
{
    _p->viewport()->update();
}

void KRPTSceneView::resizeEvent(QResizeEvent *e)
{
}

void KRPTSceneView::mousePressEvent(QMouseEvent *e)
{
}

void KRPTSceneView::mouseReleaseEvent(QMouseEvent *e)
{
}

void KRPTSceneView::mouseMoveEvent(QMouseEvent *e)
{
}

void KRPTSceneView::wheelEvent(QWheelEvent *e)
{
}

void KRPTSceneView::paintEvent(QPainter &p)
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

 void KRPTSceneView::resizeEventImpl(QResizeEvent *e) noexcept
 {
//    if(_scene)_scene->setGeometry(QRectF(100, 100, _p->width() - 200, _p->height() - 200));
    
    if(_scene)_scene->setGeometry(QRectF(0, 0, _p->width() - 3, _p->height() - 3));
    resizeEvent(e);
 }

void KRPTSceneView::mousePressEventImpl(QMouseEvent *e) noexcept
{
    if(!_scene)return;
    auto event = createMouseSceneEvent(e);
    _scene->mousePressEvent(event.get());
    mousePressEvent(e);
}

void KRPTSceneView::mouseReleaseEventImpl(QMouseEvent *e) noexcept
{
    if(!_scene)return;
    auto event = createMouseSceneEvent(e);
    _scene->mouseReleaseEvent(event.get());
    mouseReleaseEvent(e);
}

void KRPTSceneView::mouseMoveEventImpl(QMouseEvent *e) noexcept
{
    if(!_scene)return;
    auto event = createMouseSceneEvent(e);
    _scene->mouseMoveEvent(event.get());
    mouseMoveEvent(e);
}

void KRPTSceneView::wheelEventImpl(QWheelEvent *e) noexcept
{
    if(!_scene)return;
    auto event = createWheelSceneEvent(e);
    _scene->whellEvent(event.get());
    wheelEvent(e);
}

void KRPTSceneView::paintEventImpl(QPaintEvent *e) noexcept
{
    if(!_scene)return;
    QPainter painter(_p->viewport());
    _scene->paintEvent(painter);
    paintEvent(painter);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

SceneMouseEvent::Ptr KRPTSceneView::createMouseSceneEvent(QSinglePointEvent *e) noexcept
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

SceneMouseEvent::Ptr KRPTSceneView::createWheelSceneEvent(QWheelEvent *e) noexcept
{
    auto ret = createMouseSceneEvent(e);
    ret->setDelta(e->angleDelta());
    return ret;
}
