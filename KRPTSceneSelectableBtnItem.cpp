//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneSelectableBtnItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneSelectableBtnItem::KRPTSceneSelectableBtnItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneScrolledAreaItem(scene, parent, geometry), _mousePressedItem(nullptr), _selectedItem(nullptr),
      _childSize(40, 40), _expanded(Expanded::No), _expandDirect(ExpandDirect::Bottom)
{
    _scrollPolicy.up(ScrollPolicy::MouseChildOver);
    setExpandDirect(ExpandDirect::Bottom);
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor( 155,  155,  155));
    setCornerRadius(5, 5);
    setMargin(2, 2);
    _posGrid = QPointF(_childSize.width() + _margin.y(), _childSize.height() + _margin.y());
    setSize(childFullSize(1, 1));
}

KRPTSceneSelectableBtnItem::~KRPTSceneSelectableBtnItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneSelectableBtnItem::ExpandDirect KRPTSceneSelectableBtnItem::expandDirect() const noexcept
{
    return _expandDirect;
}

void KRPTSceneSelectableBtnItem::setExpandDirect(ExpandDirect direct) noexcept
{
    if(direct == _expandDirect)return;
    _expandDirect = direct;
    updateChildGeometry();
}

QSizeF KRPTSceneSelectableBtnItem::childSize() const noexcept
{
    return _childSize;
}

void KRPTSceneSelectableBtnItem::setChildSize(const QSizeF &size) noexcept
{
    if(qFuzzyCompare(_childSize, size))return;
    _childSize = size;
    updateChildGeometry();
    setSize(_expandedRect[0].size());
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

QSizeF KRPTSceneSelectableBtnItem::childFullSize(size_t xCount, size_t yCount) noexcept // move to protected
{
    QSizeF ret(_childSize.width () * xCount + _margin.y() * (xCount > 0 ? xCount - 1 : 0) + _margin.x() * 2,
               _childSize.height() * yCount + _margin.y() * (yCount > 0 ? yCount - 1 : 0) + _margin.y() * 2);
    return ret;
}

void KRPTSceneSelectableBtnItem::updateChildGeometry() noexcept
{
    const auto &childs = areaChildItems();
    size_t sz = childs.size();
    switch(_expandDirect)
    {
        case ExpandDirect::Left   :
        case ExpandDirect::Right  : 
            _scrollPolicy.up  (ScrollPolicy::Horisontal);
            _scrollPolicy.down(ScrollPolicy::Vertical);
            _expandedRect[0].setSize(childFullSize(1, 1));
            _expandedRect[1].setSize(childFullSize(sz, 1));
            setInvertWheel(true);
            break;
        case ExpandDirect::Top    :
        case ExpandDirect::Bottom :
            _scrollPolicy.up  (ScrollPolicy::Vertical);
            _scrollPolicy.down(ScrollPolicy::Horisontal);
            _expandedRect[0].setSize(childFullSize(1, 1));
            _expandedRect[1].setSize(childFullSize(1, sz));
            setInvertWheel(false);
            break;
        break;
    }
    _posGrid = QPointF(_childSize.width() + _margin.y(), _childSize.height() + _margin.y());
    QPointF p(_margin.x(), _margin.y());
    for(auto &child : childs)
    {
        child->lockUpdate(true);
        child->setGeometry(p, _childSize);
        switch(_expandDirect)
        {
            case ExpandDirect::Left   :
            case ExpandDirect::Right  : p.setX(p.x() + _childSize.width () + _margin.x()); break;
            case ExpandDirect::Top    :
            case ExpandDirect::Bottom : p.setY(p.y() + _childSize.height() + _margin.y()); break;
        }
        child->lockUpdate(false);
    }
}

void KRPTSceneSelectableBtnItem::expand(bool exp) noexcept
{
    QPointF p;
    if(_selectedItem)
    {
        p.setX(_selectedItem->pos().x() - _margin.x());
        p.setY(_selectedItem->pos().y() - _margin.y());
    }
    if(exp)
    {
        QRectF r = _expandedRect[1];

        switch(_expandDirect)
        {
            case ExpandDirect::Top    :
            case ExpandDirect::Bottom : setAreaPos(0, -p.y(), 500); break;
            case ExpandDirect::Left   :
            case ExpandDirect::Right  : setAreaPos(-p.x(), 0, 500); break;
        }

        QSizeF s = childFullSize(5, 5);
        switch(_expandDirect)
        {
            case ExpandDirect::Top    :
            case ExpandDirect::Bottom : r.setHeight(std::min(r.height() , s.height())); break;
            case ExpandDirect::Left   :
            case ExpandDirect::Right  : r.setWidth(std::min(r.width() , s.width())); break;
        }

        setSize(r.size(), 500);
        _expanded = Expanded::Yes;
    }else
    {
        switch(_expandDirect)
        {
            case ExpandDirect::Top    :
            case ExpandDirect::Bottom : setAreaPos(0, -p.y(), 500); break;
            case ExpandDirect::Left   :
            case ExpandDirect::Right  : setAreaPos(-p.x(), 0, 500); break;
        }

        setSize(_expandedRect[0].size(), 500);
        _expanded = Expanded::No;
    }
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneSelectableBtnItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    KRPTSceneScrolledAreaItem::addChildImpl(item, parent);

    item->lockUpdate(true);
    item->downMust(Must::MouseChecked);
    size_t sz = areaChildItems().size();

    double x = 0;
    double y = 0;

    if(_expandDirect == ExpandDirect::Bottom)
    {
        x = _margin.x();
        y = (_childSize.height() + _margin.y()) * (sz - 1) + _margin.y();

        _expandedRect[0].setSize(childFullSize(1, 1));
        _expandedRect[1].setSize(childFullSize(1, sz));

    }else
    if(_expandDirect == ExpandDirect::Right)
    {
        x = (_childSize.width() + _margin.x()) * (sz - 1) + _margin.x();
        y = _margin.y();

        _expandedRect[0].setSize(childFullSize(1, 1));
        _expandedRect[1].setSize(childFullSize(sz, 1));

    }
    item->setGeometry(x, y, _childSize.width(), _childSize.height());

    item->setColor(0, QColor( 0,  0,   0, 0));
    item->setColor(1, QColor( 0,  0,   0, 0));

    item->setColor(2, QColor( 200,  200,   200, 255));
    item->setColor(2, QColor( 255,  255,   0, 255), State::Checked);
//    item->setColor(2, QColor( 255,  255,   0, 255), State::Checked);
//    item->setColor(2, QColor( 255,  255,   0, 255), State::Checked | State::MouseOver);
    item->setColor(2, QColor( 255,  255,   255, 255), State::MouseOver);


//    setSize(areaSize());
    item->lockUpdate(false);
}

void KRPTSceneSelectableBtnItem::mousePressImpl(SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mousePressImpl(e);
    _mousePressedItem = nullptr;
}

void KRPTSceneSelectableBtnItem::mouseReleaseImpl(SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mouseReleaseImpl(e);
    if(!_mousePressedItem)return;
    {
        if(_mouseMoveDistance < 2)
        {
            _mousePressedItem->setChecked(true);
            _selectedItem = _mousePressedItem;
            auto &childs = areaChildItems();
            for(auto &child : childs)
            {
                if(child != _mousePressedItem)
                    child->setChecked(false);
            }
            _mousePressedItem = nullptr;
        }else return;
    }
    if(_expanded == Expanded::No)expand(true);
    else expand(false);
}

void KRPTSceneSelectableBtnItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mouseMoveImpl(e);
}

void KRPTSceneSelectableBtnItem::mouseOutImpl(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mouseOutImpl(item, e);
    if(_expanded == Expanded::No)return;
    if(item && (item->parent() == (void*)_area || item->parent() == this))return;
    expand(false);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneSelectableBtnItem::areaChildMousePressEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMousePressEvent(item, e);
    _mousePressedItem = item;
}

void KRPTSceneSelectableBtnItem::areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMouseReleaseEvent(item, e);
    _mousePressedItem = nullptr;
}

void KRPTSceneSelectableBtnItem::areaChildMouseMoveEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMouseMoveEvent(item, e);
    _mousePressedItem = item;
}
