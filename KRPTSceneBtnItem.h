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
    void    setImageRect     (const QRectF &rect) noexcept;
    void    setImage         (const QByteArray &src, const QRectF &rect)              noexcept;
protected:
    void    stateChangeEvent (const FState &newState, const FState &oldState)         noexcept override;
    void    mousePressEvent  (SceneMouseEvent *e)                                     noexcept override;
    void    mouseReleaseEvent(SceneMouseEvent *e)                                     noexcept override;
    void    mouseEnterEvent  (bool enter)                                             noexcept override;
protected:
    void    outlineImpl      ()                                                       noexcept override;
    void    animImpl         (uint32_t id, const std::vector<double> &value, 
                               uint32_t time, bool completed, int loop)               noexcept override;
    void    paintBackground  (QPainter &painter, uint32_t stage)                      noexcept override;
    void    paintForeground  (QPainter &painter, uint32_t stage)                      noexcept override;
protected:
    KRPTSceneBtnItemData *_d;
    double   _cornerRadius;
    QRectF   _imageRect;
};
