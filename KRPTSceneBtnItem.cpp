//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneBtnItem.h"
#include "KRPTScene.h"
#include "KRPTImageCache.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneBtnItemData
{
    friend KRPTSceneBtnItem;
    KRPTImageCacheKeyType _imageKey;
};

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneBtnItem::KRPTSceneBtnItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneItem(scene, parent, geometry,
        Must::NoClipPainter          | 
        Must::NoCheckChildVisibled   |
        Must::TransformAnim          |
//        Must::ColorAnim              |
//        Must::AccuracyClip           |
        Must::AccuracyCheckContains  |
        Must::Checked                |
        Must::MouseChecked           | 
        Must::MouseTracking          |
        Must::TransformEvent         |
        Must::StateChangeEvent       |
        Must::MousePressEvent        |
        Must::MouseMoveEvent         |
        Must::MouseEnterEvent        |
        Must::MousePressToParentEvent|
        Must::MousePressToSceneEvent |
        Must::CheckedToParentEvent   |
        Must::CheckedToSceneEvent), _d(new KRPTSceneBtnItemData()), _cornerRadius(6),
       _imageMargin(5, 5),
       _imageRect(_imageMargin.x(), _imageMargin.y(), geometry.width() - _imageMargin.x() * 2, geometry.height() - _imageMargin.y() * 2)
{
#if 0
    setColor(0,  50,  50,  50, 255);
    setColor(1, 100, 100, 100, 255);
    setColor(2, 200, 200, 200, 255);
    setColor(0,  60,  60,  60, 255, State::MouseOver);
    setColor(1, 110, 110, 110, 255, State::MouseOver);
    setColor(2, 210, 210, 210, 255, State::MouseOver);
    setColor(0,  70,  70,  70, 255, State::MousePressed);
    setColor(1, 120, 120, 120, 255, State::MousePressed);
    setColor(2, 220, 220, 220, 255, State::MousePressed);
    setColor(2, 101, 203, 255, 255, State::Checked);
#else
    setColor(0,  50,  50,  50, 255);
    setColor(1, 100, 100, 100, 255);
    setColor(2, 200, 200, 200, 255);
    setColor(2, 0, 255, 0, 255, State::Checked);
#endif
}

KRPTSceneBtnItem::~KRPTSceneBtnItem() noexcept
{
    delete _d;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::setCornerRadius(double radius) noexcept
{
    if(qFuzzyCompare(_cornerRadius, radius))return;
    _cornerRadius = radius;
    update();
}

void KRPTSceneBtnItem::setImageSrc(const QByteArray &src) noexcept
{
    _d->_imageKey = KRPTImageCache::key(src);
}

void KRPTSceneBtnItem::setImageRect(const QRectF &rect) noexcept
{
    if(qFuzzyCompare(_imageRect, rect))return;
    _imageRect = rect;
    update();
}

void KRPTSceneBtnItem::setImage(const QByteArray &src, const QRectF &rect) noexcept
{
    if(qFuzzyCompare(_imageRect, rect))return;
    _d->_imageKey = KRPTImageCache::key(src);
    _imageRect = rect;
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
    (void)e;
};

void KRPTSceneBtnItem::mouseReleaseEvent(SceneMouseEvent *e) noexcept 
{
    (void)e;
};

void KRPTSceneBtnItem::mouseEnterEvent(bool enter) noexcept
{
    (void)enter;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::transformImpl(SceneTransformEvent *e) noexcept
{
    if(e->resized())
    {
       _imageRect = QRectF(_imageMargin.x(), _imageMargin.y(), 
                           _geometry.width() - _imageMargin.x() * 2, _geometry.height() - _imageMargin.y() * 2);
    }
    KRPTSceneItem::transformImpl(e);
}

void KRPTSceneBtnItem::outlineImpl() noexcept
{
    if(qFuzzyIsNull(_cornerRadius))_outline.addRect(_rect);
    else _outline.addRoundedRect(_rect, _cornerRadius, _cornerRadius);
}

bool KRPTSceneBtnItem::stateChangeImpl(const FState &cur, const FState &old) noexcept
{
    return KRPTSceneItem::stateChangeImpl(cur, old);
}

bool KRPTSceneBtnItem::setCheckedImpl(bool checked) noexcept
{
    return KRPTSceneItem::setCheckedImpl(checked);
}

void KRPTSceneBtnItem::animImpl(uint32_t id, const std::vector<double> &value, 
    uint32_t time, bool completed, int loop) noexcept
{
    KRPTSceneItem::animImpl(id, value, time, completed, loop);
}

void KRPTSceneBtnItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0));
    if(qFuzzyIsNull(_cornerRadius))painter.drawRect(_rect);
    else painter.drawRoundedRect(_rect, _cornerRadius, _cornerRadius);
    painter.setBrush(Qt::NoBrush); 

//    QPen pen(QColor(255, 0, 0), 1.0);
//    pen.setCosmetic(true);
//    painter.setPen(pen); 


    if(_d->_imageKey.enable())
        KRPTImageCache::draw(_d->_imageKey, painter, _imageRect, color(2));
}

void KRPTSceneBtnItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setRenderHint(QPainter::Antialiasing, false);
    QPen pen(color(1), 1.5);
//    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
//    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);
    if(qFuzzyIsNull(_cornerRadius))painter.drawRect(_rect);
    else painter.drawRoundedRect(_rect, _cornerRadius, _cornerRadius);
//    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
//    painter.drawRect(_rect.toRect());
//    painter.drawRect(_imageRect);
}
