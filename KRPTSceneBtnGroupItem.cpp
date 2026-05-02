//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneBtnGroupItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneBtnGroupItem::KRPTSceneBtnGroupItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneScrolledAreaItem(scene, parent, geometry), _mousePressedItem(nullptr), _selectedItem(nullptr)
{
    _scrollPolicy.up(ScrollPolicy::MouseChildOver);
    _scrollPolicy.down(ScrollPolicy::Horisontal);

    _margin = QPointF(5, 5);
//    setAreaSize(500, 500);
}

KRPTSceneBtnGroupItem::~KRPTSceneBtnGroupItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnGroupItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    item->downMust(Must::MouseChecked);
    KRPTSceneScrolledAreaItem::addChildImpl(item, parent);
}

void KRPTSceneBtnGroupItem::mousePressImpl(SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mousePressImpl(e);
}

void KRPTSceneBtnGroupItem::mouseReleaseImpl(SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mouseReleaseImpl(e);
    if(_expanded && _mousePressedItem)
    {
    #if 1
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

//            qDebug() << _mouseMoveDistance;

        }else
            return;

    #endif
//        _mousePressedItem = nullptr;
    }


    if(!_expanded)
    {
        double d = 0;
        if(_selectedItem)
        {
//            d = _mousePressedItem->pos().y() - _margin.y() - _mousePressedItem->height() / 2;

//            d = _selectedItem->pos().y() + _selectedItem->height() / 2;
            d = _selectedItem->pos().y() - _margin.y();
        }

//        setSize(60, 360, 1000, QEasingCurve::Linear);
//        setAreaPos(0, -d + 180, 1000, QEasingCurve::Linear);

        double h = (50 + _margin.y()) * 10 + _margin.y();

//        d -= h / 2;

        setSize(60, h, 500);
        setAreaPos(0, -d, 500);
    }
    else
    {
        double d = 0;
        if(_selectedItem)
        {
            d = _selectedItem->pos().y() - _margin.y();
        }

//        setSize(60, 60, 1000, QEasingCurve::Linear);
//        setAreaPos(0, -d, 1000, QEasingCurve::Linear);

        setSize(60, 60, 500);
        setAreaPos(0, -d, 500);
    }

    _expanded = !_expanded;

}

void KRPTSceneBtnGroupItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mouseMoveImpl(e);
}

void KRPTSceneBtnGroupItem::mouseOutImpl(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mouseOutImpl(item, e);

    if(!_expanded)return;

//    qDebug() << "=====";

#if 1
        double d = 0;
        if(_selectedItem)
        {
            d = _selectedItem->pos().y() - _margin.y();
        }

//        setSize(60, 60, 1000, QEasingCurve::Linear);
//        setAreaPos(0, -d, 1000, QEasingCurve::Linear);

        setSize(60, 60, 500);
        setAreaPos(0, -d, 500);
    _expanded = !_expanded;
#endif
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnGroupItem::areaChildMousePressEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMousePressEvent(item, e);
    _mousePressedItem = item;
}

void KRPTSceneBtnGroupItem::areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMouseReleaseEvent(item, e);
    _mousePressedItem = nullptr;
}

void KRPTSceneBtnGroupItem::areaChildMouseMoveEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMouseMoveEvent(item, e);
    _mousePressedItem = item;
}
