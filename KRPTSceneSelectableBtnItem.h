//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTSceneScrolledAreaItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneSelectableBtnItem : public KRPTSceneScrolledAreaItem
{
public:
    enum class ExpandDirect{Left, Top, Right, Bottom};
public:
    KRPTSceneSelectableBtnItem (KRPTScene *scene, KRPTSceneItem *parent, 
                                const QRectF &geometry = QRectF(0, 0, 100, 100))                     noexcept;
    ~KRPTSceneSelectableBtnItem()                                                                    noexcept;
public:
    ExpandDirect expandDirect              ()                                             const noexcept;
    QSizeF       childSize                 ()                                             const noexcept;
    void         setExpandDirect           (ExpandDirect direct) noexcept;
    void         setChildSize              (const QSizeF &size) noexcept;
protected:
    void         addChildImpl              (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept override;
    void         mousePressImpl            (SceneMouseEvent *e)                                 noexcept override;
    void         mouseReleaseImpl          (SceneMouseEvent *e)                                 noexcept override;
    void         mouseMoveImpl             (SceneMouseEvent *e)                                 noexcept override;
    void         mouseOutImpl              (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void         areaChildMousePressEvent  (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void         areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void         areaChildMouseMoveEvent   (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
protected:
    enum class Expanded{No, Yes, To, From};
protected:
    QSizeF       childFullSize             (size_t xCount, size_t yCount)                       noexcept;
    void         updateChildGeometry       ()                                                   noexcept;
    void         expand                    (bool expand)                                        noexcept;
protected:
    KRPTSceneItem::Ptr _mousePressedItem;
    KRPTSceneItem::Ptr _selectedItem    ;
    QSizeF             _childSize       ;
    QRectF             _expandedRect[2] ;
    Expanded           _expanded        ;
    ExpandDirect       _expandDirect    ;
};
