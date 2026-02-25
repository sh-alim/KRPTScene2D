//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QPointF>
#include <QRectF>
#include <QColor>

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneAnim : public QAbstractAnimation
{
public:
    using Event = std::function<void(uint32_t, int, double)>;
public:
    template<typename T> inline static T interpolate(const T &f, const T &t, qreal progress)
    {
        return T(f + (t - f) * progress);
    }
#if 0
    template<> inline static QPointF interpolate(const QPointF &p1, const QPointF &p2, qreal progress)
    {
        return QPointF(interpolate(p1.x(), p2.x(), progress), interpolate(p1.y(), p2.y(), progress));
    }
    template<> inline static QSizeF interpolate(const QSizeF &s1, const QSizeF &s2, qreal progress)
    {
        return QSizeF(interpolate(s1.width(), s2.width(), progress), 
            interpolate(s1.height(), s2.height(), progress));
    }
    template<> inline static QRectF interpolate(const QRectF &r1, const QRectF &r2, qreal progress)
    {
        return QRectF(interpolate(r1.topLeft(), r2.topLeft(), progress), 
            interpolate(r1.size(), r2.size(), progress));
    }
#endif

    inline static void interpolate(QRectF &rect, const std::vector<std::pair<double, double>> &value, qreal progress)
    {
        rect.setX     (interpolate(value[0].first, value[0].second, progress));
        rect.setY     (interpolate(value[1].first, value[1].second, progress));
        rect.setWidth (interpolate(value[2].first, value[2].second, progress));
        rect.setHeight(interpolate(value[3].first, value[3].second, progress));
    }

    inline static QRectF interpolate(const std::vector<std::pair<double, double>> &v0, qreal progress)
    {
        QRectF res;
        interpolate(res, v0, progress);
        return res;
    }

    inline static void interpolate(const std::vector<std::pair<double, double>> &in, 
        std::vector<double> &out, qreal progress)
    {
        out.resize(in.size());
        for(size_t i = 0; i < in.size(); ++i)
            out[i] = interpolate(in[i].first, in[i].second, progress);
    }

#if 0
    template<> inline static QColor interpolate(const QColor &c1, const QColor &c2, qreal progress)
    {
        QColor ret;
        ret.setRed  (interpolate(c1.redF  (), c2.redF  (), progress));
        ret.setGreen(interpolate(c1.greenF(), c2.greenF(), progress));
        ret.setBlue (interpolate(c1.blueF (), c2.blueF (), progress));
        ret.setAlpha(interpolate(c1.alphaF(), c2.alphaF(), progress));
        return ret;
    }
#endif
public:
    KRPTSceneAnim(uint32_t id, const Event &event)                  noexcept;
   ~KRPTSceneAnim() noexcept;
public:
    int  duration         ()                                  const override;
    void setDuration      (int duration)                            noexcept;  
protected:
    void updateCurrentTime(int time)                                override;
    void updateDirection  (QAbstractAnimation::Direction direction) override;
    void updateState      (QAbstractAnimation::State newState, 
                           QAbstractAnimation::State oldState)      override;
private:
    const Event   & _event   ;
    uint32_t        _id      ;
    int             _duration;
    QEasingCurve    _easing  ;
};
