//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTScene.h"
#include "KRPTSceneView.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneRoot : public KRPTSceneItem
{
friend class KRPTScene;
public:
    KRPTSceneRoot(KRPTScene *scene, KRPTSceneItem *parent)  noexcept;
protected:
    void paintBackground(QPainter &painter, uint32_t stage) noexcept override;
    void paintForeground(QPainter &painter, uint32_t stage) noexcept override;
private:
    QString _debugString;
};

KRPTSceneRoot::KRPTSceneRoot(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent)
{
    upMust(KRPTSceneItem::Must::NoClipPainter);
}

void KRPTSceneRoot::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.fillRect(_rect, color(0));
}

void KRPTSceneRoot::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing, false);
    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect.adjusted(0.5, 0.5, -1.5, -1.5));

    painter.drawText(10, 40, _debugString);
}

//########################################################################################################################
//#
//########################################################################################################################

KRPTScene::KRPTScene(KRPTSceneView *view) noexcept
    : _view(view) , _item(new KRPTSceneRoot(this, nullptr)), _deviceScale(1), 
      _mousePressedItem(nullptr), _mouseOverItem(nullptr)
{
}

KRPTScene::~KRPTScene() noexcept
{
    delete _item;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

const QRectF &KRPTScene::geometry() const noexcept 
{
    return _item->geometry();
}

const QRectF &KRPTScene::rect() const noexcept 
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

double KRPTScene::scale() const noexcept
{
    return _item->scale();
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

KRPTSceneItem::Ptr KRPTScene::mousePressedItem() const noexcept
{
    return _mousePressedItem;
}

QColor KRPTScene::backgroundColor() const noexcept 
{
    return _item->color(0);
}

QColor KRPTScene::borderColor() const noexcept 
{
    return _item->color(1);
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

void KRPTScene::setScale(double scale) noexcept
{
    _item->setScale(scale);
}

void KRPTScene::setDeviceScale(double scale) noexcept
{
    if(qFuzzyCompare(_deviceScale, scale))return;
    _deviceScale = scale;
    deviceScaleEvent(scale);
}

void KRPTScene::setBackgroundColor(const QColor &color) noexcept
{
    _item->setColor(0, color);
}

void KRPTScene::setBorderColor(const QColor &color) noexcept
{
    _item->setColor(1, color);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneItem::Ptr KRPTScene::itemFromPos(const QPointF &pos, CompFn comp) noexcept
{
    QPointF p = _item->transformInv().map(pos);
    return itemFromPosImpl(p, comp, _item);
}

KRPTScene::Items KRPTScene::itemsFromPos(const QPointF &pos, CompFn comp, bool one) noexcept
{
    QPointF p = _item->transformInv().map(pos);
    return std::move(itemsFromPosImpl(p, comp, _item, one));
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTScene::update() noexcept
{
    _view->update();
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTScene::mousePressEvent(SceneMouseEvent *e) noexcept
{
    QPointF mousePos = e->pos();
    auto item = itemFromPos(mousePos, [](KRPTSceneItem *item)
    {
        return item->must(KRPTSceneItem::Must::MousePressEvent);
    });
    if(item == _mousePressedItem)return;
    KRPTSceneItem::FState oldState;
    if(!item && _mousePressedItem)
    {
        oldState = _mousePressedItem->_state;
        _mousePressedItem->_state -= KRPTSceneItem::State::MousePressed;
        mouseOverCheck(e);
        if(_mousePressedItem->_state != oldState)
            _mousePressedItem->stateChangeImpl(_mousePressedItem->_state, oldState);
    }
    _mousePressedItem = item;
    if(!_mousePressedItem)return;
    QPointF p = _mousePressedItem->mapFromScene(mousePos);
    _mousePressedItemPos = item->pos() - item->mapToParent(p);
    oldState = _mousePressedItem->_state;
    _mousePressedItem->_state += KRPTSceneItem::State::MousePressed;
    if(_mousePressedItem->must(KRPTSceneItem::Must::Checked))
    {
        if(_mousePressedItem->_state[KRPTSceneItem::State::Checked])
            _mousePressedItem->_state -= KRPTSceneItem::State::Checked;
        else _mousePressedItem->_state += KRPTSceneItem::State::Checked;
    }
    mouseOverCheck(e);
    if(_mousePressedItem->_state != oldState)
        _mousePressedItem->stateChangeImpl(_mousePressedItem->_state, oldState);
    _mousePressPos = mousePos;
}

void KRPTScene::mouseReleaseEvent(SceneMouseEvent *e) noexcept
{
    QPointF mousePos = e->pos();
    if(_mousePressedItem)
    {
        _mousePressedItem->mouseReleaseImpl(SceneMouseEvent::get(_mousePressedItem->mapFromScene(mousePos), 
            e->btns(), mousePos, e->keyModifers(), e->delta()).get());
        if(_mousePressedItem)
        {
            KRPTSceneItem::FState oldState = _mousePressedItem->_state;
            _mousePressedItem->_state -= KRPTSceneItem::State::MousePressed;
            _mousePressedItem->stateChangeImpl(_mousePressedItem->_state, oldState);
        }
        _mousePressedItem = nullptr;
    }
}

void KRPTScene::mouseMoveEvent(SceneMouseEvent *e) noexcept
{
    _mousePos = e->pos();
//    if(e->btns() == SceneMouseEvent::Btn::No)
    {
        mouseOverCheck(e);
//        return;
    }
    if(_mousePressedItem)
    {
        _mousePressedItem->mouseMoveImpl(SceneMouseEvent::get(_mousePressedItem->mapFromScene(_mousePos), 
            e->btns(), _mousePos, e->keyModifers(), e->delta()).get());
        if(e->btns()[SceneMouseEvent::Btn::Left])
        {
            if(_mousePressedItem->must(KRPTSceneItem::Must::MouseMoved))
            {
                _mousePressedItem->setPos(_mousePressedItem->mapToParent(
                    _mousePressedItem->mapFromScene(_mousePos)) + _mousePressedItemPos);
            }
        }
    }
}

void KRPTScene::whellEvent(SceneMouseEvent *e) noexcept
{
    QPointF mousePos = e->pos();
    KRPTSceneItem *item = _mouseOverItem;
    if(item)
    {
        item->whellImpl(SceneMouseEvent::get(item->mapFromScene(mousePos), e->btns(), 
            mousePos, e->keyModifers(), e->delta()).get());
    #if 0
        if(e->keyModifers()[SceneMouseEvent::KeyModifer::Ctrl])
        {
//            QPointF p = mousePos;
//            KRPTSceneItem::TransSrc trn = KRPTSceneItem::TransSrc::Scene;
//            QPointF p = item->mapFromScene(mousePos);
//            KRPTSceneItem::TransSrc trn = KRPTSceneItem::TransSrc::Self;
            QPointF p = item->parent()->mapFromScene(mousePos);
            KRPTSceneItem::TransSrc trn = KRPTSceneItem::TransSrc::Parent;
//            item->setAngle((e->delta().y() > 0 ? item->angle() + 5 : item->angle() -5), mousePos, KRPTSceneItem::TransSrc::Scene);
//            item->rotate((e->delta().y() > 0 ? 5 : -5));

            if(item->_parent && item->_parent != _item)
//                item->_parent->setScale((e->delta().x() > 0 ? item->_parent->scale() * 1.1 : item->_parent->scale() * 0.9), p, trn);
                item->_parent->setAngle((e->delta().y() > 0 ? item->_parent->angle() + 5 : item->_parent->angle() -5));

        }else
        if(e->keyModifers()[SceneMouseEvent::KeyModifer::Alt])
        {
//            QPointF p = mousePos;
//            KRPTSceneItem::TransSrc trn = KRPTSceneItem::TransSrc::Scene;

//            QPointF p = item->mapFromScene(mousePos);
//            KRPTSceneItem::TransSrc trn = KRPTSceneItem::TransSrc::Self;

            QPointF p = item->parent()->mapFromScene(mousePos);
            KRPTSceneItem::TransSrc trn = KRPTSceneItem::TransSrc::Parent;
//            item->setScale((e->delta().x() > 0 ? item->scale() * 1.1 : item->scale() * 0.9), p, trn);

            if(item->_parent && item->_parent != _item)
//                item->_parent->setScale((e->delta().x() > 0 ? item->_parent->scale() * 1.1 : item->_parent->scale() * 0.9), p, trn);
                item->_parent->setScale((e->delta().x() > 0 ? item->_parent->scale() * 1.1 : item->_parent->scale() * 0.9));

        }else
        {
            item->setSize((e->delta().y() > 0) ? item->width() + 10 : item->width() - 10, item->height());
        }
    #endif
    }
}

void KRPTScene::deviceScaleEvent(double scale) noexcept
{
}

void KRPTScene::paintEvent(QPainter &painter) noexcept
{
    if(_printDebug)_debugTimer.start();

    paintImpl(painter, _item);
//    qDebug().noquote() << "elapsed : " << t.elapsed();

    if(_printDebug)
    {
        auto item = static_cast<KRPTSceneRoot*>(_item);
        item->_debugString = QString("%1").arg(_debugTimer.elapsed(), 5);
    }
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneItem::Ptr KRPTScene::itemFromPosImpl(const QPointF &pos, CompFn comp, KRPTSceneItem *item) noexcept
{
    auto res = itemsFromPosImpl(pos, comp, item, false);
    return !res.empty() ? res.back() : nullptr;
}

KRPTScene::Items KRPTScene::itemsFromPosImpl(const QPointF &pos, CompFn comp, KRPTSceneItem *item, 
    bool one, uint32_t level) noexcept
{
    if(!_item->visible() || item->must(KRPTSceneItem::Must::NoMouseEventTranslate))return KRPTScene::Items();
    const Items &childs = item->visibleChildItems();
    Items res;
    if(childs.empty())return std::move(res);
    auto it = childs.crbegin();
    for(; it != childs.crend(); ++it)
    {
        KRPTSceneItem *child = *it;
        if(!child->visible())continue;
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

void KRPTScene::paintImpl(QPainter &painter, KRPTSceneItem *item, uint32_t stage) noexcept
{
#if 0
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    painter.setTransform(item->transform(), true);
    if(!item->must(KRPTSceneItem::Must::NoClipChilds, KRPTSceneItem::Must::NoClipPainter))
    {
    #if 1
        if(!item->must(KRPTSceneItem::Must::AccuracyClip))
            painter.setClipRect(item->_rect.adjusted(0, 0, 0.5, 0.5), Qt::ClipOperation::IntersectClip);
        else painter.setClipPath(item->outline(), Qt::ClipOperation::IntersectClip);
    #endif
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
#else
    if(!item->visible())return;
    const auto &childs = item->visibleChildItems();
    painter.save();
    painter.setTransform(item->transform(), true);
    painter.setOpacity(painter.opacity() * item->opaq());
    if(item->needPaint())
        item->paintBackground(painter, stage);
//    painter.save();
    if(!childs.empty() && !item->must(KRPTSceneItem::Must::NoClipChilds, KRPTSceneItem::Must::NoClipPainter))
    {
        if(!item->must(KRPTSceneItem::Must::AccuracyClip))
            painter.setClipRect(item->_rect.adjusted(0, 0, 0, 0), Qt::ClipOperation::IntersectClip);
        else painter.setClipPath(item->outline(), Qt::ClipOperation::IntersectClip);
    }
    if(!childs.empty() && item->needChildPaint())
    {
        KRPTSceneItem::List ch; 
        uint32_t st = 0;
        for(auto &child : childs)
        {
            if(child->_paintStageCount > st + 1)
                ch.emplace_back(child);
            paintImpl(painter, child, st);
        }
        while(!ch.empty())
        {
            KRPTSceneItem::List tch;
            ++st;
            for(auto &child : ch)
            {
                if(child->_paintStageCount > st + 1)
                    tch.emplace_back(child);
                paintImpl(painter, child, st);
            }
            ch.clear();
            ch.splice(ch.end(), tch);
        }
    }
//    painter.restore();
    if(item->needPaint())
        item->paintForeground(painter, stage);
    painter.restore();
#endif
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTScene::mouseOverCheck(SceneMouseEvent *e) noexcept
{
    auto item = itemFromPos(_mousePos, [](KRPTSceneItem *item)
    {
        return item->must(KRPTSceneItem::Must::MouseTracking);
    });
    if(item != _mouseOverItem)mouseOverUpdate(item, e);
}

void KRPTScene::mouseOverUpdate(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    if(item == _mouseOverItem)return;
    KRPTSceneItem::FState oldState;
    if(_mouseOverItem)
    {
        if(_mouseOverItem->_state[KRPTSceneItem::State::MouseOver])
        {
            oldState = _mouseOverItem->_state;
            _mouseOverItem->_state -= KRPTSceneItem::State::MouseOver;
            _mouseOverItem->_state -= KRPTSceneItem::State::MousePressed;
            _mouseOverItem->stateChangeImpl(_mouseOverItem->_state, oldState);
        }
        KRPTSceneItem::Ptr commonParent = _mouseOverItem->commonParent(item);
        KRPTSceneItem::Ptr parent = _mouseOverItem->_parent;
        while(parent && parent != _item)
        {
            if(parent == commonParent && parent != item)break;
            if(parent->_state[KRPTSceneItem::State::ChildMouseOver])
            {
                oldState = parent->_state;
                parent->_state -= KRPTSceneItem::State::ChildMouseOver;
                parent->stateChangeImpl(parent->_state, oldState);
            }
            parent = parent->_parent;
        }
    }
    _mouseOverItem = item;
    if(_mouseOverItem)
    {
        if(!_mouseOverItem->_state[KRPTSceneItem::State::MouseOver] || 
            _mouseOverItem->_state[KRPTSceneItem::State::ChildMouseOver])
        {
            oldState = _mouseOverItem->_state;
            _mouseOverItem->_state += KRPTSceneItem::State::MouseOver;
            _mouseOverItem->_state -= KRPTSceneItem::State::ChildMouseOver;
            if(e && e->btns() != SceneMouseEvent::Btn::No)
                _mouseOverItem->_state += KRPTSceneItem::State::MousePressed;
            _mouseOverItem->stateChangeImpl(_mouseOverItem->_state, oldState);
        }
        KRPTSceneItem::Ptr parent = _mouseOverItem->_parent;
        while(parent && parent != _item)
        {
            if(!parent->_state[KRPTSceneItem::State::ChildMouseOver])
            {
                oldState = parent->_state;
                parent->_state += KRPTSceneItem::State::ChildMouseOver;
                parent->stateChangeImpl(parent->_state, oldState);
            }
            parent = parent->_parent;
        }
    }
}
