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
#if 1
    upMust
    (
        KRPTSceneItem::Must::NoClipPainter,
        KRPTSceneItem::Must::NoClipChilds,
        KRPTSceneItem::Must::TransformToParentEvent

//        KRPTSceneItem::Must::NoCheckChildVisibled,

//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
//            KRPTSceneItem::Must::MousePressEvent,
//            KRPTSceneItem::Must::MouseMoveEvent,
//            KRPTSceneItem::Must::WhellEvent
    );
#endif

    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));
}

KRPTSceneAreaItem::~KRPTSceneAreaItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneAreaItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
}

void KRPTSceneAreaItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
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
        KRPTSceneItem::Must::NoClipPainter,
        KRPTSceneItem::Must::NoClipChilds,
//        KRPTSceneItem::Must::NoCheckChildVisibled,

            KRPTSceneItem::Must::TransformEvent,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
//            KRPTSceneItem::Must::MouseMoved,
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

void KRPTSceneScrolledAreaItem::setAreaGeometry(const QRectF &geometry, 
    uint32_t time, QEasingCurve curve) noexcept
{
//    checkAreaRect();
    if(qFuzzyCompare(_areaRect, geometry))return;
    _areaRect = geometry;
    updateAreaRect();
}

void KRPTSceneScrolledAreaItem::setAreaPos(const QPointF &p) noexcept
{
    if(qFuzzyCompare(_areaRect.topLeft(), p))return;
    _areaRect.moveTopLeft(p);
    updateAreaRect();
}

void KRPTSceneScrolledAreaItem::setAreaPos(double x, double y) noexcept
{
    setAreaPos(QPointF(x, y));
}

void KRPTSceneScrolledAreaItem::setAreaX(double x) noexcept
{
    setAreaPos(x, _areaRect.y());
}

void KRPTSceneScrolledAreaItem::setAreaY(double y) noexcept
{
    setAreaPos(_areaRect.x(), y);
}

void KRPTSceneScrolledAreaItem::setAreaSize(const QSizeF &s) noexcept
{
    if(qFuzzyCompare(_areaRect.size(), s))return;
    _areaRect.setSize(s);
    _trans.dirtyAreaSize = true;
    updateAreaRect();
}

void KRPTSceneScrolledAreaItem::setAreaSize(double w, double h) noexcept
{
    setAreaSize(QSizeF(w, h));
}

void KRPTSceneScrolledAreaItem::setAreaWidth(double w) noexcept
{
    setAreaSize(w, _areaRect.height());
}

void KRPTSceneScrolledAreaItem::setAreaHeight(double h) noexcept
{
    setAreaSize(_areaRect.width(), h);
}

void KRPTSceneScrolledAreaItem::translateArea(const QPointF &p) noexcept
{
    setAreaPos(areaPos() + p);
}

void KRPTSceneScrolledAreaItem::translateArea(double x, double y) noexcept
{
    setAreaPos(areaX() + x, areaY() + y);
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
    painter.fillRect(_rect, color(0));
}

void KRPTSceneScrolledAreaItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));

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
    _outline.addRect(_rect);
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
        _trans.dirtyAreaTrans = true;
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
    item->upMust
    (
        KRPTSceneItem::Must::TransformToParentEvent
    );
    item->downMust
    (
        KRPTSceneItem::Must::MouseMoved
    );
}

void KRPTSceneScrolledAreaItem::mousePressImpl(SceneMouseEvent *e) noexcept
{
    _areaMousePressedPos = _area->pos() - e->pos();
}

void KRPTSceneScrolledAreaItem::mouseReleaseImpl(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneScrolledAreaItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
    if(e->btns() == SceneMouseEvent::Btn::Left)
    {
        setAreaPos(e->pos() + _areaMousePressedPos);
    }
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

bool KRPTSceneScrolledAreaItem::checkAreaRect(const QRectF &src, QRectF &dst, QPointF &dp) noexcept
{
    if(_trans.dirtyAreaTrans)
    {
        double angleRad = _area->_angle * 0.017453292519943295769;
        _trans.sin = std::sin(angleRad);
        _trans.cos = std::cos(angleRad);
    }
    if(_trans.dirtySize || _trans.dirtyAreaTrans)
    {
        double tsin = _trans.sin < 0 ? -_trans.sin : _trans.sin;
        double tcos = _trans.cos < 0 ? -_trans.cos : _trans.cos;
        _trans.rect = QRectF(_rect.center().x(), _rect.center().y(), 
                             _rect.height() * tsin + _rect.width() * tcos, 
                             _rect.height() * tcos + _rect.width() * tsin);
        _trans.rect.moveCenter(_rect.center());
    }
    _trans.dirtySize      = false;
    _trans.dirtyAreaTrans = false;
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
#if 0
    double angleRad = _area->_angle * 0.017453292519943295769;
    double sin = std::sin(angleRad);
    double cos = std::cos(angleRad);
    double tsin = sin < 0 ? -sin : sin;
    double tcos = cos < 0 ? -cos : cos;
    QRectF tr1(0.0, 0.0, _rect.height() * tsin + _rect.width() * tcos, 
                         _rect.height() * tcos + _rect.width() * tsin);
    tr1.moveCenter(_rect.center());
    QRectF res(_areaRect.x(), _areaRect.y(), 
        std::max(tr1.width (), _areaRect.width ()),
        std::max(tr1.height(), _areaRect.height()));
    QRectF tr2 = res;
    QPointF dc = res.center() - _rect.center();
    tr2.moveCenter(QPointF(dc.x() *  cos - dc.y() * -sin + _rect.center().x(), 
                           dc.x() * -sin + dc.y() *  cos + _rect.center().y()));
    double left   = tr1.left  () < tr2.left  () ? tr1.left  () - tr2.left  () : 0;
    double top    = tr1.top   () < tr2.top   () ? tr1.top   () - tr2.top   () : 0;
    double right  = tr1.right () > tr2.right () ? tr1.right () - tr2.right () : 0;
    double bottom = tr1.bottom() > tr2.bottom() ? tr1.bottom() - tr2.bottom() : 0;
    QPointF dx(left + right, top + bottom);
    QPointF dp(dx.x() * cos - dx.y() * sin, dx.x() * sin + dx.y() * cos);
    res.translate(dp);
//    _areaRect.moveTopLeft(res.topLeft());
    _area->setGeometry(res);
    _areaMousePressedPos += dx;
#else

    #if 1
    QRectF rect;
    QPointF dp;
    if(checkAreaRect(_areaRect, rect, dp))
    {
        _area->setGeometry(rect);
        _areaMousePressedPos += dp;
    }

    #else
    if(_trans.dirtyAreaTrans)
    {
        double angleRad = _area->_angle * 0.017453292519943295769;
        _trans.sin = std::sin(angleRad);
        _trans.cos = std::cos(angleRad);
    }
    if(_trans.dirtySize || _trans.dirtyAreaTrans)
    {
        double tsin = _trans.sin < 0 ? -_trans.sin : _trans.sin;
        double tcos = _trans.cos < 0 ? -_trans.cos : _trans.cos;
        _trans.rect = QRectF(_rect.center().x(), _rect.center().y(), 
                             _rect.height() * tsin + _rect.width() * tcos, 
                             _rect.height() * tcos + _rect.width() * tsin);
        _trans.rect.moveCenter(_rect.center());
    }
    _trans.dirtySize      = false;
    _trans.dirtyAreaTrans = false;
    QRectF rect(_areaRect.x(), _areaRect.y(), 
        std::max(_trans.rect.width (), _areaRect.width ()),
        std::max(_trans.rect.height(), _areaRect.height()));
    QPointF dc = rect.center() - _rect.center();
    QRectF r = rect;
    r.moveCenter(QPointF(dc.x() *  _trans.cos - dc.y() * -_trans.sin + _rect.center().x(), 
                         dc.x() * -_trans.sin + dc.y() *  _trans.cos + _rect.center().y()));
    double left   = _trans.rect.left  () < r.left  () ? _trans.rect.left  () - r.left  () : 0;
    double top    = _trans.rect.top   () < r.top   () ? _trans.rect.top   () - r.top   () : 0;
    double right  = _trans.rect.right () > r.right () ? _trans.rect.right () - r.right () : 0;
    double bottom = _trans.rect.bottom() > r.bottom() ? _trans.rect.bottom() - r.bottom() : 0;
    QPointF dx(left + right, top + bottom);
    if(!qFuzzyIsNull(dx))
    {
        QPointF dp(dx.x() * _trans.cos - dx.y() * _trans.sin, dx.x() * _trans.sin + dx.y() * _trans.cos);
        rect.translate(dp);
    }
    _area->setGeometry(rect);
    _areaMousePressedPos += dx;

    #endif
#endif
}
