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

class KRPTSceneAreaItem : public KRPTSceneItem
{
public:
    KRPTSceneAreaItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
   ~KRPTSceneAreaItem()                                        noexcept;
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
    
    QPointF _margin = QPointF(20, 20);

    QRectF _r;
};
