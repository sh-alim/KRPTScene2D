//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneCanvasItem : public KRPTSceneItem
{
friend class KRPTSceneCanvasItemClient;
public:
    KRPTSceneCanvasItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
    ~KRPTSceneCanvasItem()                                       noexcept;
protected:
    void outlineImpl     ()                                  noexcept override;
    void paintBackground (QPainter &painter, uint32_t stage) noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage) noexcept override;
protected:
    void addChildImpl    (KRPTSceneItem::Ptr item, 
                          KRPTSceneItem::Ptr parent)         noexcept override;
protected:
    KRPTSceneCanvasItemClient *_client;
};
