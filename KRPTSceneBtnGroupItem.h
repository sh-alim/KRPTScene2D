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

class KRPTSceneBtnGroupItem : public KRPTSceneScrolledAreaItem
{
public:
    KRPTSceneBtnGroupItem (KRPTScene *scene, KRPTSceneItem *parent, 
                           const QRectF &geometry = QRectF(0, 0, 100, 100))             noexcept;
    ~KRPTSceneBtnGroupItem()                                                            noexcept;
protected:
    void addChildImpl              (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept override;
    void mousePressImpl            (SceneMouseEvent *e)                                 noexcept override;
    void mouseReleaseImpl          (SceneMouseEvent *e)                                 noexcept override;
    void mouseMoveImpl             (SceneMouseEvent *e)                                 noexcept override;
    void mouseOutImpl              (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void areaChildMousePressEvent  (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void areaChildMouseMoveEvent   (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
protected:
    KRPTSceneItem::Ptr _mousePressedItem;
    KRPTSceneItem::Ptr _selectedItem    ;


    bool _expanded = false;

};
