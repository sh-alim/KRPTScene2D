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
public:
    QPointF mapToClient(const QPointF &point) noexcept;

protected:
    void paintBackground (QPainter &painter, uint32_t stage)    noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage)    noexcept override;
protected:
    void outlineImpl        ()                                  noexcept override;
    void transformImpl      (SceneTransformEvent *e)            noexcept override;
    void childTransformEvent(KRPTSceneItem::Ptr item, 
                             SceneTransformEvent *e)            noexcept override;
    void addChildImpl       (KRPTSceneItem::Ptr item, 
                             KRPTSceneItem::Ptr parent)         noexcept override;

    void mousePressImpl     (SceneMouseEvent *e)                noexcept override;
    void mouseReleaseImpl   (SceneMouseEvent *e)                noexcept override;
    void mouseMoveImpl      (SceneMouseEvent *e)                noexcept override;

private:
    void resetMinMax() noexcept;
    void updateMinMax(KRPTSceneItem::Ptr item = nullptr) noexcept;
    void updateClentRect() noexcept;

protected:
    struct MinMax
    {
        KRPTSceneItem::Ptr item  = nullptr;
        double             value = 0      ;
    };
    std::array<MinMax, 4> _minMax;
protected:
    KRPTSceneCanvasItemClient *_client;
    QRectF                     _clientRect; 

    QPointF              _clientMousePressedPos      ;


    QPointF              _p0;
    
};
