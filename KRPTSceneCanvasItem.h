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
    void paintBackground (QPainter &painter, uint32_t stage) noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage) noexcept override;
protected:
    void outlineImpl     ()                                  noexcept override;
    void transformImpl(SceneTransformEvent *e) noexcept override;
    void childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)  noexcept override;


    void addChildImpl    (KRPTSceneItem::Ptr item, 
                          KRPTSceneItem::Ptr parent)         noexcept override;
private:
    void updateMinMaxItems() noexcept;

    void updateMinClientRect(KRPTSceneItem::Ptr item) noexcept;
    void updateClent() noexcept;

protected:
    KRPTSceneCanvasItemClient *_client;
    QRectF                     _clientRect; 



    std::array<KRPTSceneItem::Ptr, 4> _minMaxItems;
};
