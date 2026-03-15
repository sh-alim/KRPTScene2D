//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "KRPTSceneEvent.h"

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneViewPriv;
class KRPTScene;

class KRPTSceneView : public QObject
{
friend class KRPTSceneViewPriv;
public:
    KRPTSceneView(QWidget *parent, KRPTScene *scene = nullptr) noexcept;
   ~KRPTSceneView() noexcept;
public:
    void setGeometry(int ax, int ay, int aw, int ah) noexcept;
    void setScene(KRPTScene *scene) noexcept;

public:
    virtual void update() noexcept;

    virtual void resizeEvent      (QResizeEvent *e);
    virtual void mousePressEvent  (QMouseEvent  *e);
    virtual void mouseReleaseEvent(QMouseEvent  *e);
    virtual void mouseMoveEvent   (QMouseEvent  *e);
    virtual void wheelEvent       (QWheelEvent  *e);
    virtual void paintEvent       (QPainter     &p);

private:
    virtual void resizeEventImpl      (QResizeEvent *e) noexcept;
    virtual void mousePressEventImpl  (QMouseEvent  *e) noexcept;
    virtual void mouseReleaseEventImpl(QMouseEvent  *e) noexcept;
    virtual void mouseMoveEventImpl   (QMouseEvent  *e) noexcept;
    virtual void wheelEventImpl       (QWheelEvent  *e) noexcept;
    virtual void paintEventImpl       (QPaintEvent  *e) noexcept;

private:
    SceneMouseEvent::Ptr createMouseSceneEvent(QSinglePointEvent *e) noexcept;
    SceneMouseEvent::Ptr createWheelSceneEvent(QWheelEvent       *e) noexcept;

private:
    KRPTSceneViewPriv *_p;
    KRPTScene         *_scene;
};
