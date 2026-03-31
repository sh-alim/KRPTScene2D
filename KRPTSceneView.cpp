//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneView.h"
#include "KRPTScene.h"
#include <QGraphicsView>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QApplication>

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneViewPort : public QOpenGLWidget 
{
protected:
#if 0
    void initializeGL() override {}
#endif
};

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
    KRPTSceneView *_owner        ;
    bool           _mouseTracking;
    bool           _autoFill     ; 
    bool           _hwAccel      ; 
};

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneViewPriv::KRPTSceneViewPriv(QWidget *parent, KRPTSceneView *owner) noexcept
    : QGraphicsView(parent), _owner(owner), _mouseTracking(false), _autoFill(true), _hwAccel(false)
{
    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

KRPTSceneViewPriv::~KRPTSceneViewPriv() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneViewPriv::resizeEvent(QResizeEvent *e)
{
//    QGraphicsView::resizeEvent(e);
    _owner->resizeEventImpl(e);
}

void KRPTSceneViewPriv::mousePressEvent(QMouseEvent *e)
{
//    QGraphicsView::mousePressEvent(e);
    _owner->mousePressEventImpl(e);
    if(_owner->_translateEvents && parentWidget())
        QCoreApplication::sendEvent(parentWidget(), e);
}

void KRPTSceneViewPriv::mouseReleaseEvent(QMouseEvent *e)
{
//    QGraphicsView::mouseReleaseEvent(e);
    _owner->mouseReleaseEventImpl(e);
    if(_owner->_translateEvents && parentWidget())
        QCoreApplication::sendEvent(parentWidget(), e);
}

void KRPTSceneViewPriv::mouseMoveEvent(QMouseEvent *e)
{
//    QGraphicsView::mouseMoveEvent(e);
    _owner->mouseMoveEventImpl(e);
    if(_owner->_translateEvents && parentWidget())
        QCoreApplication::sendEvent(parentWidget(), e);
}

void KRPTSceneViewPriv::wheelEvent(QWheelEvent *e)
{
//    QGraphicsView::wheelEvent(e);
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
    : QObject(parent), _scene(scene), _p(new KRPTSceneViewPriv(parent, this)), _translateEvents(false)
{
    setHwAccel      (true);
    setAutoFill     (false);
    setMouseTracking(true);
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
    double deviceScale = QApplication::primaryScreen()->devicePixelRatio();
    _scene->setDeviceScale(deviceScale);
}

void KRPTSceneView::setAutoFill(bool autoFill) noexcept
{
    if(!_p->viewport() || _p->_autoFill == autoFill)return;
    _p->_autoFill = autoFill;
    _p->viewport()->setAttribute(Qt::WA_NoSystemBackground   , !autoFill);
    _p->viewport()->setAttribute(Qt::WA_TranslucentBackground, !autoFill);
    if(_p->_hwAccel)
        _p->viewport()->setAutoFillBackground(autoFill);
}

void KRPTSceneView::setMouseTracking(bool tracking) noexcept
{
    if(!_p->viewport() || _p->_mouseTracking == tracking)return;
    _p->_mouseTracking = tracking;
    _p->viewport()->setMouseTracking(tracking);
}

void KRPTSceneView::setHwAccel(bool accel, uint8_t amples) noexcept
{
    if(_p->_hwAccel == accel)return;
    _p->_hwAccel = accel;
    QWidget *vieport = nullptr;
    if(!accel)vieport = new QWidget(); else
    {
        QSurfaceFormat fmt;
        fmt.setSamples(amples);
        fmt.setSwapInterval(0);
        auto w = new KRPTSceneViewPort();
        w->setFormat(fmt);
        vieport = w;
    }
    _p->setViewport(vieport);
    bool mouseTracking =  _p->_mouseTracking;
    bool autoFill      =  _p->_autoFill     ; 
    _p->_mouseTracking = !_p->_mouseTracking;
    _p->_autoFill      = !_p->_autoFill     ; 
    setAutoFill     (autoFill);
    setMouseTracking(mouseTracking);
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
    if(_scene)_scene->setGeometry(QRectF(0, 0, _p->width() - 1, _p->height() - 1));
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
