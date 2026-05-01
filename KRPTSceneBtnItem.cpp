//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneBtnItem.h"
#include "KRPTScene.h"

#include "KRPTImageCache.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneBtnItem::KRPTSceneBtnItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneItem(scene, parent, geometry,
        Must::NoClipPainter         | 
        Must::NoCheckChildVisibled  |
        Must::Anim                  |
        Must::AccuracyCheckContains |
        Must::Checked               |
        Must::MouseChecked          | 
        Must::MouseTracking         |
        Must::StateChangeEvent      |
        Must::MousePressEvent       |
        Must::MouseMoveEvent        |
        Must::MouseEnterEvent
    )
{
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,

#if 0
    setColor(0, QColor(  0,  50,   0, 255));
    setColor(0, QColor(  0, 100,   0, 255), State::MouseOver);
    setColor(0, QColor(  0, 100,   0, 255), State::MousePressed);
    setColor(0, QColor(  0,  50,   0, 255), State::MouseOver | State::MousePressed);
    setColor(0, QColor(  0, 180,   0, 255), State::Checked);
    setColor(0, QColor(  0, 150,   0, 255), State::Checked | State::MouseOver);
    setColor(0, QColor(  0,  50,   0, 255), State::Checked | State::MouseOver | State::MousePressed);
    setColor(1, QColor(  0, 255,   0, 255));
    setColor(1, QColor(  0, 255,   0, 255), State::MouseOver);
    setColor(1, QColor(  0, 255,   0, 255), State::MouseOver | State::MousePressed);
    setColor(1, QColor(255, 255,   0, 255), State::Checked);
    setColor(1, QColor(255, 255,   0, 255), State::Checked | State::MouseOver);
    setColor(1, QColor(255, 255,   0, 255), State::Checked | State::MouseOver | State::MousePressed);
#else

    setColor(0, QColor(  50, 50, 50, 255));
    setColor(0, QColor(  32, 43, 54, 255), State::MouseOver);
    setColor(0, QColor(  32, 43, 54, 255), State::MouseOver | State::MousePressed);

    setColor(0, QColor(  82, 136, 193, 255), State::Checked);
    setColor(0, QColor(  82, 136, 193, 255), State::Checked | State::MouseOver);
    setColor(0, QColor(  82, 136, 193, 255), State::Checked | State::MouseOver | State::MousePressed);

//    setColor(0, QColor(  32, 43, 54, 255));


    setColor(1, QColor(  255,  255,   255, 255));


    setColor(2, QColor(  200, 200, 200, 255));
    setColor(2, QColor(  210, 210, 210, 255), State::MouseOver);
    setColor(2, QColor(  210, 210, 210, 255), State::MouseOver | State::MousePressed);

    setColor(2, QColor(  255, 255,   0, 255), State::Checked);
    setColor(2, QColor(  255, 255,   0, 255), State::Checked | State::MouseOver);
    setColor(2, QColor(  255, 255,   0, 255), State::Checked | State::MouseOver | State::MousePressed);


//    setTransformAnchor(TransformAnchor::LeftTop);

//    _image.setSource("D:/Desktop/иконки/3/chart.svg");
//    _image.setSource("D:/Desktop/иконки/3/star.svg");
//    _image.setSource("D:/Desktop/иконки/3/setting 3.svg");
//    _image.setSource("D:/Desktop/иконки/1/Expand/104-Map.svg");

//    _image.setSource("D:/Desktop/иконки/4/Line/Settings.svg");
//    _image.setSource("D:/Desktop/иконки/4/Line/Quill.svg");
//    _image.setSource("D:/Desktop/иконки/4/Line/Bug.svg");

//    _image.setSource("D:/Desktop/иконки/6/broken/broken-command.svg");
//    _image.setSource("D:/Desktop/иконки/6/broken/broken-location-1.svg");
//    _image.setSource("D:/Desktop/иконки/6/broken/broken-settings.svg");

//    _image.setSource("D:/Desktop/иконки/7/activity.svg");
//    _image.setSource("D:/Desktop/иконки/7/cursor.svg");
    _image.setSource("D:/Desktop/иконки/8/brush.svg");

    
#endif
}

KRPTSceneBtnItem::~KRPTSceneBtnItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::stateChangeEvent(const FState &newState, const FState &oldState) noexcept
{
    KRPTSceneItem::stateChangeEvent(newState, oldState);
};

void KRPTSceneBtnItem::mousePressEvent(SceneMouseEvent *e) noexcept 
{
};

void KRPTSceneBtnItem::mouseReleaseEvent(SceneMouseEvent *e) noexcept 
{
};

void KRPTSceneBtnItem::mouseEnterEvent(bool enter) noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::outlineImpl() noexcept
{
//    _outline.addRect(_rect);
    _outline.addRoundedRect(_rect, _radius, _radius);
}

void KRPTSceneBtnItem::animImpl(uint32_t id, const std::vector<double> &value, 
    uint32_t time, bool completed, int loop) noexcept
{
    KRPTSceneItem::animImpl(id, value, time, completed, loop);
}

void KRPTSceneBtnItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0)); 
    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);

//    painter.drawRect(_rect);

    painter.setBrush(Qt::NoBrush); 

    QPen pen(QColor(255, 0, 0), 1.0);
    pen.setCosmetic(true);
    painter.setPen(pen); 

//    KRPTSceneImageCache::Image image = KRPTSceneImageCache::get(0, _rect.size());
//    painter.drawImage(_rect, *image);

    QRectF r = _rect.adjusted(4.0, 4.0, -4.0, -4.0);
    _image.draw(painter, r, color(2));
}

void KRPTSceneBtnItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setRenderHint(QPainter::Antialiasing, false);
    QPen pen(color(1), 1.5);
//    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
//    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);
//    painter.drawRoundedRect(_rect, _radius, _radius);
//    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
//    painter.drawRect(_rect.toRect());
//    painter.drawRect(_rect);
}
