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

class KRPTSceneBtnItemData;

class KRPTSceneBtnItem : public KRPTSceneItem
{
public:
    KRPTSceneBtnItem         (KRPTScene *scene, KRPTSceneItem *parent, 
                              const QRectF &geometry = QRectF(0, 0, 100, 100))        noexcept;
    ~KRPTSceneBtnItem        ()                                                       noexcept;
public:
    double  cornerRadius     ()                                                 const noexcept {return _cornerRadius;}
    QRectF  imageRect        ()                                                 const noexcept {return _imageRect   ;}

    void    setCornerRadius  (double radius)                                          noexcept;
    void    setImageSrc      (const QByteArray &src)                                  noexcept;
protected:
    void    stateChangeEvent (const FState &newState, const FState &oldState)         noexcept override;
    void    mousePressEvent  (SceneMouseEvent *e)                                     noexcept override;
    void    mouseReleaseEvent(SceneMouseEvent *e)                                     noexcept override;
    void    mouseEnterEvent  (bool enter)                                             noexcept override;
protected:
    void    transformImpl    (SceneTransformEvent *e)                                 noexcept override;
    void    outlineImpl      ()                                                       noexcept override;
    bool    stateChangeImpl  (const FState &cur, const FState &old)                   noexcept override;
    bool    setCheckedImpl   (bool checked)                                           noexcept override;

    void    animImpl         (uint32_t id, const std::vector<double> &value, 
                               uint32_t time, bool completed, int loop)               noexcept override;
    void    paintBackground  (QPainter &painter, uint32_t stage)                      noexcept override;
    void    paintForeground  (QPainter &painter, uint32_t stage)                      noexcept override;
protected:
    KRPTSceneBtnItemData *_d           ;
    double                _cornerRadius;
    QPointF               _imageMargin ;
    QRectF                _imageRect   ;
};
