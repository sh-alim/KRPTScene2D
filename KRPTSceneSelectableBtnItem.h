//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTSceneScrolledAreaItem.h"
#include "KRPTSceneBtnItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneSelectableBtnItem : public KRPTSceneScrolledAreaItem
{
public:
    enum class ExpandDirect{Left, Top, Right, Bottom};
public:
    KRPTSceneSelectableBtnItem (KRPTScene *scene, KRPTSceneItem *parent, 
                                const QRectF &geometry = QRectF(0, 0, 100, 100))                noexcept;
    ~KRPTSceneSelectableBtnItem()                                                               noexcept;

public:
    KRPTSceneBtnItem * addItem(const QByteArray &imageSrc, const QString &text) noexcept;

public:
    ExpandDirect expandDirect              ()                                             const noexcept;
    void         setExpandDirect           (ExpandDirect direct)                                noexcept;
    void         setSelectedItem           (KRPTSceneItem::Ptr item, bool anim = true)          noexcept;
protected:
    void         addChildImpl              (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept override;
    void         transformImpl             (SceneTransformEvent *e)                             noexcept override;
    void         mousePressImpl            (SceneMouseEvent *e)                                 noexcept override;
    void         mouseReleaseImpl          (SceneMouseEvent *e)                                 noexcept override;
    void         mouseMoveImpl             (SceneMouseEvent *e)                                 noexcept override;
    void         mouseOutImpl              (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void         animImpl                  (uint32_t id, const std::vector<double> &value, 
                                            uint32_t time, bool completed, int loop)            noexcept override;
    void         areaChildMousePressEvent  (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void         areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void         areaChildMouseMoveEvent   (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void         setMarginImpl             (const QPointF &margin)                              noexcept override;
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
    uint32_t           _expandAnimTime  ; 
};
