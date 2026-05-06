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
    enum class SizePolicy
    {
        None,
        AutoSize,
        AutoPosSize,
    };
public:
    KRPTSceneAreaItem               (KRPTScene *scene, KRPTSceneItem *parent, 
                                     const QRectF &geometry = QRectF(0, 0, 100, 100), 
                                     FMust must = Must::No)                                 noexcept;
public:
    QPointF      margin             ()                                                const noexcept;
    SizePolicy   sizePolicy         ()                                                const noexcept;
    double       cornerRadius       ()                                                const noexcept;
    void         setMargin          (const QPointF &margin)                                 noexcept;
    void         setMargin          (double x, double y)                                    noexcept;
    void         setSizePolicy      (SizePolicy policy)                                     noexcept;
    void         setCornerRadius    (double radius)                                         noexcept;
    void         lockAutoUpdate     (bool update)                                           noexcept;
protected:
    void         paintBackground    (QPainter &painter, uint32_t stage)                     noexcept override;
    void         paintForeground    (QPainter &painter, uint32_t stage)                     noexcept override;
protected:
    void         outlineImpl        ()                                                      noexcept override;
//    void         transformImpl      (SceneTransformEvent *e)                                noexcept override;
    void         childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)       noexcept override;
    void         addChildImpl       (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent)    noexcept override;
protected:
    void         resetMinMax        ()                                                      noexcept;
    void         updateMinMax       (KRPTSceneItem::Ptr item = nullptr)                     noexcept;
    void         updateClentRect    ()                                                      noexcept;
protected:
    virtual void setMarginImpl      (const QPointF &margin)                                 noexcept;
protected:
    struct MinMax
    {
        KRPTSceneItem::Ptr item  = nullptr;
        double             value = 0      ;
    };
    std::array<MinMax, 4> _minMax;
protected:
    QPointF    _margin        ;
    SizePolicy _sizePolicy    ;
    double     _cornerRadius  ;
    bool       _lockAutoUpdate;
};
