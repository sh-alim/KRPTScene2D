//####################################################################################################
//#
//####################################################################################################

#include "KRPTScene.h"

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneRoot : public KRPTSceneItem
{
public:
    KRPTSceneRoot(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
protected:
    void paintBackground(QPainter &painter) noexcept override;
    void paintForeground(QPainter &painter) noexcept override;
};

KRPTSceneRoot::KRPTSceneRoot(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent)
{
}

void KRPTSceneRoot::paintBackground(QPainter &painter) noexcept
{
    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneRoot::paintForeground(QPainter &painter) noexcept
{
    QPen pen(_borderColor);
    painter.setPen(pen);
    painter.drawRect(_rect);
}

//####################################################################################################
//#
//####################################################################################################

KRPTScene::KRPTScene(QWidget *canvas) noexcept
    : _canvas(canvas) , _item(new KRPTSceneRoot(this, nullptr)), _mousePressedItem(nullptr)
{
}

KRPTScene::~KRPTScene() noexcept
{
    delete _item;
}

//****************************************************************************************************
//*
//****************************************************************************************************

const QRectF& KRPTScene::geometry() const noexcept 
{
    return _item->geometry();
}

const QRectF& KRPTScene::rect() const noexcept 
{
    return _item->rect();
}

QPointF KRPTScene::pos() const noexcept 
{
    return _item->pos();
}

QSizeF KRPTScene::size() const noexcept 
{
    return _item->size();
}

double KRPTScene::x() const noexcept 
{
    return _item->x();
}

double KRPTScene::y() const noexcept 
{
    return _item->y();
}

double KRPTScene::width() const noexcept 
{
    return _item->width();
}

double KRPTScene::height() const noexcept 
{
    return _item->height();
}

QPointF KRPTScene::center() const noexcept 
{
    return _item->center();
}

const QTransform& KRPTScene::transform() const noexcept 
{
    return _item->transform();
}

const QTransform& KRPTScene::sceneTransform() const noexcept 
{
    return _item->sceneTransform();
}

QColor KRPTScene::borderColor() const noexcept 
{
    return _item->_borderColor;
}

QColor KRPTScene::backgroundColor() const noexcept 
{
    return _item->_backgroundColor;
}

void KRPTScene::setGeometry(const QRectF &geometry) noexcept
{
    bool isMove   = !qFuzzyCompare(_item->geometry().topLeft(), geometry.topLeft());
    bool isResize = !qFuzzyCompare(_item->geometry().size(), geometry.size());
    if(!isMove && ! isResize)return;
    QRectF oldGeometry = _item->geometry();
    _item->setGeometry(geometry);
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(geometry, oldGeometry, 
        _item->angle(), _item->angle(), _item->scale(), _item->scale(), 
        isMove, isResize, false, false);
    transformEvent(e.get());
}

void KRPTScene::setPos(const QPointF &pos) noexcept
{
    setGeometry(QRectF(pos, _item->size()));
}

void KRPTScene::setPos(double x, double y) noexcept
{
    setPos(QPointF(x, y));
}

void KRPTScene::setSize(const QSizeF &size) noexcept
{
    setGeometry(QRectF(_item->pos(), size));
}

void KRPTScene::setSize(double width, double height) noexcept
{
    setSize(QSizeF(width, height));
}

void KRPTScene::setX(double x) noexcept
{
    setPos(QPointF(x, _item->y()));
}

void KRPTScene::setY(double y) noexcept
{
    setPos(QPointF(_item->x(), y));
}

void KRPTScene::setWidth(double width) noexcept
{
    setSize(QSizeF(width, _item->height()));
}

void KRPTScene::setHeight(double height) noexcept
{
    setSize(QSizeF(_item->width(), height));
}

void KRPTScene::setBorderColor(const QColor &color) noexcept
{
    _item->setBorderColor(color);
}

void KRPTScene::setBackgroundColor(const QColor &color) noexcept
{
    _item->setBackgroundColor(color);
}

//****************************************************************************************************
//*
//****************************************************************************************************

KRPTSceneItem* KRPTScene::itemFromPos(const QPointF &pos, CompFn comp) noexcept
{
    QPointF p = _item->transformInv().map(pos);
    return itemFromPosImpl(p, comp, _item);
}

KRPTScene::Items KRPTScene::itemsFromPos(const QPointF &pos, CompFn comp, bool one) noexcept
{
    QPointF p = _item->transformInv().map(pos);
    return std::move(itemsFromPosImpl(p, comp, _item, one));
}

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTScene::update() noexcept
{
    _canvas->update();
}

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTScene::transformEvent(SceneTransformEvent *e) noexcept
{
}

void KRPTScene::mousePressEvent(SceneMouseEvent *e) noexcept
{
    QPointF mousePos = e->pos();
    auto item = itemFromPos(mousePos, [](KRPTSceneItem *item)
    {
        return item->must(KRPTSceneItem::Must::MousePressEvent);
    });
//    if(!item)item = _item;
    if(item)
    {
    #if 0
        item->mousePressImpl(SceneMouseEvent::get(item->mapFromScene(mousePos), e->btns(), mousePos).get());
        _mousePressedItem = item;

//        QPointF p = item->mapFromScene(mousePos);
//        item->mousePressImpl(SceneMouseEvent::get(p, e->btns()).get());
//        _mousePressedItemPos = item->pos() - item->mapToParent(p);

    #else
        QPointF p = item->mapFromScene(mousePos);
        item->mousePressImpl(SceneMouseEvent::get(item->mapFromScene(mousePos), e->btns(), 
            mousePos, e->keyModifers(), e->delta()).get());
        _mousePressedItem = item;
        _mousePressedItemPos = item->pos() - item->mapToParent(p);

        _mousePressedItem->_borderColor = QColor(255, 0, 0);


//        auto itm = item->addChild<KRPTSceneItem>();
//        itm->setGeometry(QRectF(p.x(), p.y(), 20, 20));
//        if(item->parent())
//            item->parent()->delChild(item);
//        _mousePressedItem = nullptr;

//        item->setPos(item->pos(), true);
//        item->rotate(45, true);
//        item->scaleMul(1.1, true);

    #endif
    #if 0
        QPointF p0 = _mousePressedItem->mapFromScene(mousePos);
        QPointF p1 = _mousePressedItem->mapToParent(p0);
        qDebug() << mousePos << p0;
    #endif
    }
    _lastMousePos = mousePos;
    update();
}

void KRPTScene::mouseReleaseEvent(SceneMouseEvent *e) noexcept
{
    QPointF mousePos = e->pos();
    if(_mousePressedItem)
    {
        if(_mousePressedItem->must(KRPTSceneItem::Must::MouseReleaseEvent))
        {
            _mousePressedItem->mouseReleaseImpl(
                SceneMouseEvent::get(_mousePressedItem->mapFromScene(mousePos), e->btns(), 
                    mousePos, e->keyModifers(), e->delta()).get());
        }
        _mousePressedItem->_borderColor = QColor(255, 255, 255);
        _mousePressedItem = nullptr;
    }
    update();
}

void KRPTScene::mouseMoveEvent(SceneMouseEvent *e) noexcept
{
    QPointF mousePos = e->pos();
    if(_mousePressedItem)
    {
        if(_mousePressedItem->must(KRPTSceneItem::Must::MouseMoveEvent))
        {
            _mousePressedItem->mouseMoveImpl(
                SceneMouseEvent::get(_mousePressedItem->mapFromScene(mousePos), e->btns(), 
                    mousePos, e->keyModifers(), e->delta()).get());
        }
    #if 1
        QPointF p0 = _mousePressedItem->mapFromScene(mousePos);
        QPointF p1 = _mousePressedItem->mapToParent(p0);
        _mousePressedItem->setPos(p1 + _mousePressedItemPos, 1000, QEasingCurve::Linear);
//        _mousePressedItem->setPos(p1 + _mousePressedItemPos);
    #endif
    }
    update();
}

void KRPTScene::whellEvent(SceneMouseEvent *e) noexcept
{
    QPointF mousePos = e->pos();
#if 0
    if(_mousePressedItem)
    {
        if(_mousePressedItem->must(KRPTSceneItem::Must::WhellEvent))
        {
            _mousePressedItem->whellImpl(
                SceneMouseEvent::get(_mousePressedItem->mapFromScene(mousePos), e->btns(), e->delta()).get());
        }
    }
#else
    auto item = itemFromPos(mousePos, [](KRPTSceneItem *item)
    {
        return item->must(KRPTSceneItem::Must::WhellEvent);
    });
    if(item)
    {
        item->whellImpl(SceneMouseEvent::get(item->mapFromScene(mousePos), e->btns(), 
            mousePos, e->keyModifers(), e->delta()).get());
        if(e->keyModifers()[SceneMouseEvent::KeyModifer::Ctrl])
        {
            item->rotateAround((e->delta().y() > 0 ? 5 : -5), mousePos, KRPTSceneItem::TransSrc::Scene, 500);
//            item->rotate((e->delta().y() > 0 ? 5 : -5), 500);
        }
        if(e->keyModifers()[SceneMouseEvent::KeyModifer::Alt])
        {
            item->scaleFromPoint((e->delta().x() > 0 ? 1.1 : 0.9), mousePos, KRPTSceneItem::TransSrc::Scene, 500);
//            item->scaleMul((e->delta().x() > 0 ? 1.1 : 0.9), 500);
        }


//        item->rotate(e->delta() > 0 ? 1 : -1);
//        item->setSize(item->width() + (e->delta() > 0 ? 10 : -10), item->height());
//        double angle = item->angle() + (e->delta().y() > 0 ? 1 : -1);
//        double scale = item->scale() * e->delta() > 0 ? 1.1 : 0.9;
//        item->scaleFromPoint(scale, mousePos, KRPTSceneItem::TransSrc::Scene);
//        double opaq = item->opaq() * (e->delta().y() > 0 ? 1.1 : 0.1);
//        item->setOpaq(opaq, 1000);
    }
#endif
    update();
}

void KRPTScene::paintEvent(QPainter &painter) noexcept
{
    paintImpl(painter, _item);
}

//****************************************************************************************************
//*
//****************************************************************************************************

KRPTSceneItem* KRPTScene::itemFromPosImpl(const QPointF &pos, CompFn comp, KRPTSceneItem *item) noexcept
{
    auto res = itemsFromPosImpl(pos, comp, item, false);
    return !res.empty() ? res.back() : nullptr;
}

KRPTScene::Items KRPTScene::itemsFromPosImpl(const QPointF &pos, CompFn comp, KRPTSceneItem *item, 
    bool one, uint32_t level) noexcept
{
    if(item->must(KRPTSceneItem::Must::NoMouseEventTranslate))return KRPTScene::Items();
    const Items &childs = item->visibleChildItems();
    Items res;
    auto it = childs.crbegin();
    for(; it != childs.crend(); ++it)
    {
        KRPTSceneItem *child = *it;
        QPointF p = child->mapFromParent(pos);
        if(child->must(KRPTSceneItem::Must::NoClipChilds))
            res.splice(res.begin(), std::move(itemsFromPosImpl(p, comp, child, one, level + 1)));
        if(child->contains(p))
        {
            if(!child->must(KRPTSceneItem::Must::NoClipChilds))
                res.splice(res.begin(), std::move(itemsFromPosImpl(p, comp, child, one, level + 1)));
            if(comp(child))res.push_front(child);
            if(one)break;
        }
    }
    return std::move(res);
}

void KRPTScene::paintImpl(QPainter &painter, KRPTSceneItem *item) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    painter.setTransform(item->transform(), true);
    if(!item->must(KRPTSceneItem::Must::NoClipChilds))
    {
        if(!item->must(KRPTSceneItem::Must::AccuracyClip))
            painter.setClipRect(item->_rect.adjusted(0, 0, 0.5, 0.5), Qt::ClipOperation::IntersectClip);
        else painter.setClipPath(item->outline(), Qt::ClipOperation::IntersectClip);
    }
    painter.setOpacity(painter.opacity() * item->opaq());
    if(item->needPaint())
        item->paintBackground(painter);
    if(item->needChildPaint())
    {
        const auto &items = item->visibleChildItems();
        for(auto &item : items)
        {
            paintImpl(painter, item);
        }
    }
    if(item->needPaint())
        item->paintForeground(painter);
    painter.restore();
}

