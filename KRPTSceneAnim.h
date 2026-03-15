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
    using Values = std::vector<double>;
    using Event  = std::function<void(uint32_t, int, double, int, int)>;
public:
    template<typename T> inline static T interpolate(const T &f, const T &t, qreal progress)
    {
        return T(f + (t - f) * progress);
    }
    inline static void interpolate(const std::vector<double> &start, const std::vector<double> &end, 
        double progress, std::vector<double> &current)
    {
        assert(start.size() == end.size());
        assert(start.size() == current.size());
//        double discrepancy = 0;
        for(size_t i = 0; i < start.size(); ++i)
        {
            current[i] = interpolate(start[i], end[i], progress);
//            discrepancy += std::abs(current[i] - end[i]);
        }
    }
    inline static void valuesTo(const Values &values, double &value)
    {
        assert(values.size() > 0);
        value = values[0];
    }
    inline static size_t valuesFrom(double value, Values &values)
    {
        if(values.size() < 1)values.resize(1);
        values[0] = value;
        return values.size();
    }
    inline static void valuesTo(const Values &in, Values &out)
    {
        assert(out.size() < in.size());
        memcpy(out.data(), in.data(), in.size() * sizeof(double));
    }
    inline static size_t valuesFrom(const Values &in, Values &out)
    {
        if(out.size() < in.size())out.resize(in.size());
        memcpy(out.data(), in.data(), in.size() * sizeof(double));
        return in.size();
    }
    inline static void valuesTo(const Values &values, QPointF &point)
    {
        assert(values.size() > 1);
        point.setX(values[0]); point.setY(values[1]);
    }
    inline static size_t valuesFrom(const QPointF &point, Values &values)
    {
        if(values.size() < 2)values.resize(2);
        values[0] = point.x(); values[1] = point.y();
        return values.size();
    }
    inline static void valuesTo(const Values &values, QSizeF &size)
    {
        assert(values.size() > 1);
        size.setWidth(values[0]); size.setHeight(values[1]);
    }
    inline static size_t valuesFrom(const QSizeF &size, Values &values)
    {
        if(values.size() < 2)values.resize(2);
        values[0] = size.width(); values[1] = size.height();
        return values.size();
    }
    inline static void valuesTo(const Values &values, QRectF &rect)
    {
        assert(values.size() > 3);
        rect.setX(values[0]); rect.setY(values[1]); rect.setWidth(values[2]); rect.setHeight(values[3]);
    }
    inline static size_t valuesFrom(const QRectF &rect, Values &values)
    {
        if(values.size() < 4)values.resize(4);
        values[0] = rect.x(); values[1] = rect.y(); values[2] = rect.width (); values[3] = rect.height();
        return values.size();
    }
    inline static void valuesTo(const Values &values, QColor &color)
    {
        assert(values.size() > 3);
        color.setRedF(values[0]); color.setGreenF(values[1]); color.setBlueF(values[2]); color.setAlphaF(values[3]);
    }
    inline static size_t valuesFrom(const QColor &color, Values &values)
    {
        if(values.size() < 4)values.resize(4);
        values[0] = color.redF(); values[1] = color.greenF(); values[2] = color.blueF(); values[3] = color.alphaF();
        return values.size();
    }
    template<typename T>
    inline static T valuesTo(const Values &values) noexcept
    {
        T res;
        valuesTo(values, res);
        return res;
    }
public:
    KRPTSceneAnim(uint32_t id, const Event &event, 
        int duration = 1000, 
        QEasingCurve easingCurve = QEasingCurve::OutExpo, 
        int count = 1)                                          noexcept;
   ~KRPTSceneAnim()                                             noexcept;
public:
    int          duration      ()                         const override;
    QEasingCurve easingCurve   ()                         const noexcept;
    uint32_t     id            ()                         const noexcept;
    void         setDuration   (int duration)                   noexcept;
    void         setEasingCurve(QEasingCurve easingCurve)       noexcept;
protected:
    void updateCurrentTime(int time)                            override;
private:
    const Event   & _event      ;
    uint32_t        _id         ;
    int             _duration   ;
    QEasingCurve    _easingCurve;
};
