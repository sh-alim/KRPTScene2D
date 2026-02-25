//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneItem.h"
#include <QObject>

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneAnim;

class KRPTSceneAnimItem : public KRPTSceneItem 
{
public:
    KRPTSceneAnimItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
   ~KRPTSceneAnimItem()                                        noexcept;
public:

protected:
    virtual void animEvent(KRPTSceneAnim *anim, uint32_t id, double value) noexcept;

    void transformEvent   (SceneTransformEvent *e) noexcept override;
    void mousePressEvent  (SceneMouseEvent     *e) noexcept override;
    void mouseReleaseEvent(SceneMouseEvent     *e) noexcept override;
    void mouseMoveEvent   (SceneMouseEvent     *e) noexcept override;
    void whellEvent       (SceneMouseEvent     *e) noexcept override;
    void paintBackground  (QPainter &painter     ) noexcept override;
    void paintForeground  (QPainter &painter     ) noexcept override;
private:
    KRPTSceneAnim *_anim;
};

