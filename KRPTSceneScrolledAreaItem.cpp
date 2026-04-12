//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneScrolledAreaItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneAreaItem : public KRPTSceneItem
{
friend class KRPTSceneScrolledAreaItem;
public:
    KRPTSceneAreaItem(KRPTScene *scene, KRPTSceneItem *parent)                   noexcept;
    ~KRPTSceneAreaItem()                                                         noexcept;
protected:
    void paintBackground (QPainter &painter, uint32_t stage)                     noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage)                     noexcept override;
protected:
    void addChildImpl       (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept override;
#if 1
    void childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)    noexcept override;
#endif
private:
    KRPTSceneScrolledAreaItem *_owner;
};

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneAreaItem::KRPTSceneAreaItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent), _owner(static_cast<KRPTSceneScrolledAreaItem*>(parent))
{
    upMust
    (
        KRPTSceneItem::Must::NoPaint,
//        KRPTSceneItem::Must::NoClipChilds,
        KRPTSceneItem::Must::NoClipPainter,
        KRPTSceneItem::Must::Anim,
        KRPTSceneItem::Must::MouseTracking,
        KRPTSceneItem::Must::TransformToParentEvent
    );
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));

#if 0
    setColor(0, QColor(  50,  50,   50, 255));
    setColor(0, QColor(  70, 70,   70, 255), State::MouseOver);
//    setColor(0, QColor(  70, 70,   70, 255), State::MousePressed);
//    setColor(0, QColor(  70, 70,   70, 255), State::MouseOver | State::MousePressed);
    setColor(0, QColor(  70, 70,   70, 255), State::ChildMouseOver);
//    setColor(0, QColor(  0, 180,   0, 255), State::Checked);
//    setColor(0, QColor(  0, 150,   0, 255), State::Checked | State::MouseOver);
//    setColor(0, QColor(  0,  50,   0, 255), State::Checked | State::MouseOver | State::MousePressed);
#endif
}

KRPTSceneAreaItem::~KRPTSceneAreaItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneAreaItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
#if 0
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0)); 
    painter.drawRect(_rect);
    painter.setBrush(Qt::NoBrush); 
#endif
}

void KRPTSceneAreaItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
#if 0
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
#endif
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneAreaItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    KRPTSceneItem::addChildImpl(item, parent);
}

#if 1
void KRPTSceneAreaItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e) noexcept 
{
    _owner->childTransformEvent(item, e);
};
#endif

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneScrolledAreaItem::KRPTSceneScrolledAreaItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent), _area(nullptr)
{
#if 1
    upMust
    (
//        KRPTSceneItem::Must::NoClipPainter,
//        KRPTSceneItem::Must::NoClipChilds,
        KRPTSceneItem::Must::AccuracyClip,
        KRPTSceneItem::Must::Anim,

//        KRPTSceneItem::Must::AccuracyCheckContains,
//        KRPTSceneItem::Must::MouseMoved,
//        KRPTSceneItem::Must::MouseTracking,

        KRPTSceneItem::Must::TransformEvent,
        KRPTSceneItem::Must::MousePressEvent,
        KRPTSceneItem::Must::MouseMoveEvent,
        KRPTSceneItem::Must::WhellEvent
    );
#endif
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));
    _area = addChild<KRPTSceneAreaItem>();

    _areaRect = QRectF(0, 0, 500, 500);

//    _area->setGeometry(10, 10, 50, 50);
}

KRPTSceneScrolledAreaItem::~KRPTSceneScrolledAreaItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

QPointF KRPTSceneScrolledAreaItem::areaPos() const noexcept
{
    return _areaRect.topLeft();
}

double  KRPTSceneScrolledAreaItem::areaX() const noexcept
{
    return _areaRect.x();
}

double  KRPTSceneScrolledAreaItem::areaY() const noexcept
{
    return _areaRect.y();
}

QSizeF  KRPTSceneScrolledAreaItem::areaSize() const noexcept
{
    return _areaRect.size();
}

double  KRPTSceneScrolledAreaItem::areaWidth() const noexcept
{
    return _areaRect.width();
}

double  KRPTSceneScrolledAreaItem::areaHeight() const noexcept
{
    return _areaRect.height();
}

bool KRPTSceneScrolledAreaItem::setAreaGeometry(const QRectF &geometry, 
    uint32_t time, QEasingCurve curve) noexcept
{
    QRectF rect;
    QPointF dp;
    if(!checkAreaRect(geometry, rect, dp))return false;
    _areaRect.setSize(geometry.size());
    _areaRect.moveTopLeft(geometry.topLeft() + dp);
    _area->setGeometry(rect, time, curve);
    return true;
}

bool KRPTSceneScrolledAreaItem::setAreaGeometry(const QPointF &p, const QSizeF &size, 
    uint32_t time, QEasingCurve curve) noexcept
{
    return setAreaGeometry(QRectF(p, size), time, curve);
}

bool KRPTSceneScrolledAreaItem::setAreaGeometry (double x, double y, double w, double h, 
    uint32_t time, QEasingCurve curve) noexcept
{
    return setAreaGeometry(QRectF(x, y, w, h), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaPos(const QPointF &p, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaGeometry(QRectF(p, _areaRect.size()), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaPos(double x, double y, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(QPointF(x, y), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaX(double x, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(x, _areaRect.y(), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaY(double y, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(_areaRect.x(), y, time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaSize(const QSizeF &size, uint32_t time, QEasingCurve curve) noexcept
{
    if(qFuzzyCompare(_areaRect.size(), size))return;
    setAreaGeometry(_areaRect.topLeft(), size, time, curve);
    _trans.dirtyArea = true;
    updateAreaRect();
}

void KRPTSceneScrolledAreaItem::setAreaSize(double w, double h, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaSize(QSizeF(w, h), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaWidth(double w, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaSize(w, _areaRect.height(), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaHeight(double h, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaSize(_areaRect.width(), h, time, curve);
}

void KRPTSceneScrolledAreaItem::translateArea(const QPointF &p, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(areaPos() + p, time, curve);
}

void KRPTSceneScrolledAreaItem::translateArea(double x, double y, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(areaX() + x, areaY() + y, time, curve);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

QPointF KRPTSceneScrolledAreaItem::mapToArea(const QPointF &point) noexcept
{
    return _area->mapFromParent(point);
}

QPointF KRPTSceneScrolledAreaItem::mapFromArea(const QPointF &point) noexcept
{
    return _area->mapToParent(point);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
//    painter.fillRect(_rect, color(0));

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0)); 
    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);
    painter.setBrush(Qt::NoBrush); 
}

void KRPTSceneScrolledAreaItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 2);
    pen.setCosmetic(true);
    painter.setPen(pen);
//    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));

    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);

//    pen.setColor(QColor(0, 255, 0));
//    painter.setPen(pen);
//    painter.drawRect(_r0);
//    pen.setColor(QColor(0, 255, 255));
//    painter.setPen(pen);
//    painter.drawRect(_r1);
//    painter.drawPolygon(_p0);
//    painter.drawRect(_area->_geometry);
//    painter.drawRect(_areaRect);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaItem::outlineImpl() noexcept
{
//    _outline.addRect(_rect);
    _outline.addRoundedRect(_rect, _radius, _radius);

}

void KRPTSceneScrolledAreaItem::transformImpl(SceneTransformEvent *e) noexcept
{
    KRPTSceneItem::transformImpl(e);
    if(e->resized())
    {
        _trans.dirtySize = true;
        updateAreaRect();
    }
}

void KRPTSceneScrolledAreaItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)noexcept 
{
    if(e->scaled() || e->rotated())
    {
        _trans.dirtyArea = true;
        updateAreaRect();
    }
}

void KRPTSceneScrolledAreaItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    if(!_area)
    {
        KRPTSceneItem::addChildImpl(item, parent);
        return;
    }
    _area->addChildImpl(item, _area);
    item->upMust  (KRPTSceneItem::Must::TransformToParentEvent);
    item->downMust(KRPTSceneItem::Must::MouseMoved);
}

void KRPTSceneScrolledAreaItem::mousePressImpl(SceneMouseEvent *e) noexcept
{
    _areaMousePos = e->pos();
    _area->stopAnimImpl(KRPTSceneItem::AnimDst::Geometry);
    _area->stopAnimImpl(KRPTSceneItem::AnimDst::Angle);
    _area->stopAnimImpl(KRPTSceneItem::AnimDst::Scale);
}

void KRPTSceneScrolledAreaItem::mouseReleaseImpl(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneScrolledAreaItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
    QPointF dp = e->pos() - _areaMousePos;
    if(e->btns() == SceneMouseEvent::Btn::Left)
    {
        translateArea(dp);
    }
    _areaMousePos = e->pos();
}

void KRPTSceneScrolledAreaItem::whellImpl(SceneMouseEvent *e) noexcept
{
#if 1
    double dx = e->delta().x() == 0 ? 0 : e->delta().x() > 0 ? 60 : -60;
    double dy = e->delta().y() == 0 ? 0 : e->delta().y() > 0 ? 60 : -60;
//    translateArea(dx, dy, 1000, QEasingCurve::Linear);
    translateArea(dx, dy, 300);
#else
    double dx = e->delta().x() == 0 ? 0 : e->delta().x() > 0 ? 1 : -1;
    double dy = e->delta().y() == 0 ? 0 : e->delta().y() > 0 ? 0.5 : -0.5;
    _area->rotate(dy);
#endif
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

bool KRPTSceneScrolledAreaItem::checkAreaRect(const QRectF &src, QRectF &dst, QPointF &dp) noexcept
{
    if(_trans.dirtyArea)
    {
        double angleRad = _area->_angle * 0.017453292519943295769;
        _trans.sin = std::sin(angleRad);
        _trans.cos = std::cos(angleRad);
    }
    if(_trans.dirtySize || _trans.dirtyArea)
    {
        double tsin = _trans.sin < 0 ? -_trans.sin : _trans.sin;
        double tcos = _trans.cos < 0 ? -_trans.cos : _trans.cos;
        _trans.rect = QRectF(_rect.center().x(), _rect.center().y(), 
                             _rect.height() * tsin + _rect.width() * tcos, 
                             _rect.height() * tcos + _rect.width() * tsin);
        _trans.rect.moveCenter(_rect.center());
    }
    _trans.dirtySize = false;
    _trans.dirtyArea = false;
    dst = QRectF(src.x(), src.y(), 
        std::max(_trans.rect.width (), src.width ()),
        std::max(_trans.rect.height(), src.height()));
    QPointF dc = dst.center() - _rect.center();
    QRectF r = dst;
    r.moveCenter(QPointF(dc.x() *  _trans.cos - dc.y() * -_trans.sin + _rect.center().x(), 
                         dc.x() * -_trans.sin + dc.y() *  _trans.cos + _rect.center().y()));
    double left   = _trans.rect.left  () < r.left  () ? _trans.rect.left  () - r.left  () : 0;
    double top    = _trans.rect.top   () < r.top   () ? _trans.rect.top   () - r.top   () : 0;
    double right  = _trans.rect.right () > r.right () ? _trans.rect.right () - r.right () : 0;
    double bottom = _trans.rect.bottom() > r.bottom() ? _trans.rect.bottom() - r.bottom() : 0;
    QPointF dp0(left + right, top + bottom);
    dp.setX(0); dp.setY(0);
    if(!qFuzzyIsNull(dp0))
    {
        dp.setX(dp0.x() * _trans.cos - dp0.y() * _trans.sin);
        dp.setY(dp0.x() * _trans.sin + dp0.y() * _trans.cos);
        dst.translate(dp);
    }
    return !qFuzzyCompare(src, dst) || qFuzzyIsNull(dp);
}

void KRPTSceneScrolledAreaItem::updateAreaRect() noexcept
{
    QRectF rect;
    QPointF dp;
    if(checkAreaRect(_areaRect, rect, dp))
    {
        _area->setGeometry(rect);
        _areaRect.translate(dp);
    }
}
