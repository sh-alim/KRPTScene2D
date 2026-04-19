//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneBtnGroupItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneBtnGroupItem::KRPTSceneBtnGroupItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneScrolledAreaItem(scene, parent, geometry), _mousePressItem(nullptr)
{
    _scrollPolicy.up(ScrollPolicy::MouseChildOver);

    _margin = QPointF(10, 10);
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
    if(_mousePressItem)
    {
    #if 0
        if(_mouseMoveDistance < 2)
            _mousePressItem->setChecked(!_mousePressItem->checked());
    #endif
        _mousePressItem = nullptr;
    }

    if(!_expanded)
    {
//        setSize(60, 360, 1000, QEasingCurve::Linear);
        translateArea(5, 5, 1000, QEasingCurve::Linear);
//        setSize(60, 360, 500);
//        translateArea(5, 5, 300);
    }
    else
    {
//        setSize(60, 60, 1000, QEasingCurve::Linear);
        translateArea(-5, -5, 1000, QEasingCurve::Linear);

//        setSize(60, 60, 500);
//        translateArea(-5, -5, 300);
    }

    _expanded = !_expanded;

}

void KRPTSceneBtnGroupItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::mouseMoveImpl(e);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnGroupItem::areaChildMousePressEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMousePressEvent(item, e);
    _mousePressItem = item;
}

void KRPTSceneBtnGroupItem::areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMouseReleaseEvent(item, e);
    _mousePressItem = nullptr;
}

void KRPTSceneBtnGroupItem::areaChildMouseMoveEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    KRPTSceneScrolledAreaItem::areaChildMouseMoveEvent(item, e);
    _mousePressItem = item;
}
