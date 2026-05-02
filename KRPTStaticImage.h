//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include <QString>
#include <QPainterPath>
#include <QPixmap>
#include <QFont>

//#define _STATIC_IMAGE_USE_PXIMAP
//#define _STATIC_IMAGE_FILL_RECT

//########################################################################################################################
//#
//########################################################################################################################

class QXmlStreamAttributes;

class KRPTStaticImage
{
public:
#ifdef _STATIC_IMAGE_USE_PXIMAP
    using Image  = QPixmap;
#else
    using Image  = QImage;
#endif
    using PImage = std::unique_ptr<Image>;
    using Cache  = std::unordered_map<uint64_t, PImage>;
    enum class SourceType{Uncknown, Vector, Raster};
public:
    KRPTStaticImage()                                                            noexcept;
    KRPTStaticImage (const QByteArray &src, bool useCache = false)               noexcept;
   ~KRPTStaticImage()                                                            noexcept;
public:
    bool            useCache   ()                                          const noexcept {return _useCache;}
    QRectF          viewBox    ()                                          const noexcept {return _viewBox ;}
    const Image   * pixmap     (const QRectF &rect, const QColor &color)         noexcept;
    bool            setSource  (const QByteArray &src)                           noexcept;
    void            setUseCache(bool use)                                        noexcept;
    bool            draw       (QPainter &painter, const QRectF &rect, 
                                const QColor &color)                             noexcept;
private:
    bool    drawVector         (QPainter &painter, const QRectF &rect, 
                                const QColor &color)                             noexcept;
    bool    drawRaster         (QPainter &painter, const QRectF &rect)           noexcept;
    bool    prepare            ()                                                noexcept;
private:
    bool    parsePolygon       (const QString &src, QPainterPath &path)          noexcept;
    bool    parsePath          (const QString &src, QPainterPath &path)          noexcept;
    void    resetState         ()                                                noexcept;
    bool    end                ()                                          const noexcept;
    void    skipSpaces         ()                                                noexcept;
    void    skipComma          ()                                                noexcept;
    double  readNumber         ()                                                noexcept;
    QPointF readPoint          ()                                                noexcept;
    void    moveToAbs          (QPainterPath &path)                              noexcept;
    void    moveToRel          (QPainterPath &path)                              noexcept;
    void    lineToAbs          (QPainterPath &path)                              noexcept;
    void    lineToRel          (QPainterPath &path)                              noexcept;
    void    hLineAbs           (QPainterPath &path)                              noexcept;
    void    hLineRel           (QPainterPath &path)                              noexcept;
    void    vLineAbs           (QPainterPath &path)                              noexcept;
    void    vLineRel           (QPainterPath &path)                              noexcept;
    void    cubicAbs           (QPainterPath &path)                              noexcept;
    void    cubicRel           (QPainterPath &path)                              noexcept;
    void    smoothCubicAbs     (QPainterPath &path)                              noexcept;
    void    smoothCubicRel     (QPainterPath &path)                              noexcept;
    void    quadAbs            (QPainterPath &path)                              noexcept;
    void    quadRel            (QPainterPath &path)                              noexcept;
    void    smoothQuadAbs      (QPainterPath &path)                              noexcept;
    void    smoothQuadRel      (QPainterPath &path)                              noexcept;
    QPointF reflect            (const QPointF &c, const QPointF &p)              noexcept;
    void    arcAbs             (QPainterPath &path)                              noexcept;
    void    arcRel             (QPainterPath &path)                              noexcept;
    void    arcToBezier        (QPainterPath &path, 
                                const QPointF& p0, const QPointF& p1,
                                double rx, double ry, double xAxisRotation,
                                bool largeArcFlag, bool sweepFlag)               noexcept;                     
    void    arcSegmentToBezier(QPainterPath &path, double cx, double cy, 
                               double rx, double ry, 
                               double phi, double t1, double t2)                 noexcept;        
private:
    struct VectorItem
    {
        enum Rule    {NoneZero , EvenOdd             };
        enum LineCap {ButtCap  , RoundCap , SquareCap};
        enum LineJoin{MiterJoin, RoundJoin, BevelJoin};
        struct Data
        {
            Rule       fillRule    = Rule::NoneZero;
            Rule       clipRule    = Rule::NoneZero;
            LineCap    lineCap     = LineCap::ButtCap;
            LineJoin   lineJoin    = LineJoin::MiterJoin;
            double     strokeWidth = 1;
            double     opacity     = 1;
            bool       fill        = true;
            bool       stroke      = false;
            QTransform transform;
        };
        Data         data;
        QPainterPath path;
    };
    using VectorItems = std::vector<VectorItem>;
    std::vector<double> parseArguments(const QString &src)                       noexcept;
    QTransform          parseTransform(const QString &src)                       noexcept;
    void                parseItemData(QXmlStreamAttributes *attrs, 
                                      VectorItem::Data &data)                    noexcept;
public:
    VectorItems _vectorItems   ;
    PImage      _pixmap        ;
    Cache       _cache         ;
    Image       _cachePixmap   ;
    QRectF      _viewBox       ;
    QPointF     _lastPoint     ;
    QPointF     _lastControl   ;
    bool        _stroke        ;
    bool        _fill          ;
    double      _maxStrokeWidth;
    QString     _data          ;
    int         _pos           ;
    SourceType  _sourceType    ;
    bool        _useCache      ;
    QByteArray  _src           ;
    bool        _isFile        ;
    bool        _dirtySource   ;
    bool        _dirtyPixmap   ;
    QSize       _dirtySize     ;
    QColor      _dirtyColor    ;
};
