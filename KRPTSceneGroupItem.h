//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTSceneAreaItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneGroupItem : public KRPTSceneAreaItem
{
public:
    KRPTSceneGroupItem             (KRPTScene *scene, KRPTSceneItem *parent, 
                                   const QRectF &geometry = QRectF(0, 0, 100, 100))       noexcept;
public:
protected:
    void       paintBackground    (QPainter &painter, uint32_t stage)                     noexcept override;
    void       paintForeground    (QPainter &painter, uint32_t stage)                     noexcept override;
protected:
    void       outlineImpl        ()                                                      noexcept override;
    void       transformImpl      (SceneTransformEvent *e)                                noexcept override;
    void       childTransformEvent(KRPTSceneItem::Ptr item, 
                                   SceneTransformEvent *e)                                noexcept override;
    void       addChildImpl       (KRPTSceneItem::Ptr item, 
                                   KRPTSceneItem::Ptr parent)                             noexcept override;
};
