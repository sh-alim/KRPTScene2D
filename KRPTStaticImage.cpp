//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTStaticImage.h"
#include <QXmlStreamReader>
#include <QPainter>
#include <QFile>

#include <QDebug>

//########################################################################################################################
//#
//########################################################################################################################

static constexpr double _PI = 3.14159265358979323846;

constexpr static inline double degToRad(double degrees)
{
    return degrees * (_PI / 180);
}

static QRectF fitRect(const QRectF &r0, const QRectF &r1, double &s) noexcept
{
    double k = std::min(r0.width() / r1.width(), r0.height() / r1.height());
    QRectF r(0, 0, r1.width() * k, r1.height() * k);
    r.moveCenter(r0.center());
    s = (std::max(r.width(), r.height())) / (std::max(r1.width(), r1.height()));
    return r;
}

static bool isVector(const QByteArray &src)
{
    return src.contains("xml") && src.contains("svg");
}

static bool isRaster(const QByteArray &src)
{
    if(src.startsWith("\x89PNG\r\n\x1A\n") || // PNG
       src.startsWith("\xFF\xD8\xFF")      || // JPEG
       src.startsWith("BM")                || // BMP
       src.startsWith("\x00\x00\x01\x00")  || // ICO
      (src.startsWith("RIFF") && src.mid(8, 4) == "WEBP")) // WebP (RIFF....WEBP)
    return true;
    return false;
}

#if 0
static QString toCppArray(const QByteArray &data, const QString &name)
{
    QString out;
    out += "static const unsigned char " + name + "[] = {\n    ";
    for (int i = 0; i < data.size(); ++i) 
    {
        unsigned char c = static_cast<unsigned char>(data[i]);
        out += QString("0x%1").arg(c, 2, 16, QLatin1Char('0')).toUpper();
        if(i + 1 != data.size())out += ", ";
        if((i + 1) % 16 == 0)out += "\n    ";
    }
    out += "\n};\n";
    out += "static const int " + name + "_size = " + QString::number(data.size()) + ";\n";
    return out;
}
#endif

//########################################################################################################################
//#
//########################################################################################################################

KRPTStaticImage::KRPTStaticImage() noexcept
    : _stroke(false), _fill(true), _maxStrokeWidth(1), _pos(0), _sourceType(SourceType::Uncknown),
      _useCache(false), _isFile(false), _dirtySource(true), _dirtyPixmap(false)
{
}

KRPTStaticImage::KRPTStaticImage(const QByteArray &src, bool useCache) noexcept
    : _stroke(false), _fill(true), _maxStrokeWidth(1), _pos(0), _sourceType(SourceType::Uncknown),
      _useCache(useCache), _src(src), _isFile(QFile(src).exists()), _dirtySource(true), _dirtyPixmap(false)
{
}

KRPTStaticImage::~KRPTStaticImage() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

const KRPTStaticImage::Image* KRPTStaticImage::pixmap(const QRectF &rect, const QColor &color) noexcept
{
    QSize szi = rect.size().toSize();
    Image *pixmap = nullptr;
    if(_useCache)
    {
        uint16_t w = (uint16_t)szi.width ();
        uint16_t h = (uint16_t)szi.height();
        uint32_t c = (uint32_t(color.red  ()) << 24) | (uint32_t(color.green()) << 16) |
                     (uint32_t(color.blue ()) << 8)  | (uint32_t(color.alpha()));
        uint64_t key = (uint64_t(w) << 48) | (uint64_t(h) << 32) | uint64_t(c);
        auto find = _cache.find(key);
        _dirtyPixmap = find == _cache.end();
    #ifdef _STATIC_IMAGE_USE_PXIMAP
        pixmap = !_dirtyPixmap ? find->second.get() : 
            _cache.emplace(key, std::make_unique<Image>(szi)).first->second.get();
    #else
        pixmap = !_dirtyPixmap ? find->second.get() : 
            _cache.emplace(key, std::make_unique<Image>(szi, QImage::Format_ARGB32_Premultiplied)).first->second.get();
//        if(_dirtyPixmap)qDebug() << _cache.size() << (_cache.size() * szi.width() * szi.height() * 4) / 1024;
    #endif
    }else
    {
        if(_dirtySize !=  szi || _dirtyColor != color)_dirtyPixmap = true;
        _dirtySize = szi;
        _dirtyColor = color;
    #ifdef _STATIC_IMAGE_USE_PXIMAP
        if(!_pixmap)_pixmap = std::make_unique<Image>(szi);
    #else
        if(!_pixmap)_pixmap = std::make_unique<Image>(szi, QImage::Format_ARGB32_Premultiplied);
    #endif
        pixmap = _pixmap.get();
    }
    if(_dirtyPixmap)
    {
        _dirtyPixmap = false;
        if(pixmap && _sourceType != SourceType::Uncknown)
        {
            if(pixmap->isNull() || pixmap->size() != szi)
            {
            #ifdef _STATIC_IMAGE_USE_PXIMAP
                Image tmp(szi);
            #else
                Image tmp(szi, QImage::Format_ARGB32_Premultiplied);
            #endif
                pixmap->swap(tmp);
            }
        #if 1
            pixmap->fill(QColor(0, 0, 255));
        #else
            pixmap->fill(Qt::transparent);
        #endif
            QPainter pntr(pixmap);
            switch(_sourceType)
            {
                case SourceType::Vector : drawVector(pntr, rect, color); break;
                case SourceType::Raster : drawRaster(pntr, rect       ); break;
                default : break;
            }
        }
    }
    return pixmap;
}

bool KRPTStaticImage::draw(QPainter &painter, const QRectF &rect, const QColor &color) noexcept
{
    if(_dirtySource)
    {
        _dirtySource = false;
        if(!prepare())return false;
    }
    double dpr = painter.device()->devicePixelRatioF();
    const QTransform &t = painter.transform();
    double sx = dpr, sy = dpr;
    if(t.isScaling())
    {
        if(t.isRotating())
        {
            sx *= std::sqrt(t.m11() * t.m11() + t.m12() * t.m12());
            sy *= std::sqrt(t.m21() * t.m21() + t.m22() * t.m22());
        }else
        {
            sx *= t.m11();
            sy *= t.m22();
        }
    }
    QRectF r(0, 0, rect.width() * sx, rect.height() * sy);
    const Image *pixmap = this->pixmap(r, color);
    if(pixmap && !pixmap->isNull())
    {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing         , true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        QTransform t = painter.transform();
        t.scale(1.0 / sx, 1.0 / sy);
        t.translate(rect.x() * sx, rect.y() * sy);
        painter.setTransform(t);
    #ifdef _STATIC_IMAGE_USE_PXIMAP
        painter.drawPixmap(0, 0, *pixmap);
    #else
        painter.drawImage(r, *pixmap);
    #endif
        painter.restore();
        return true;
    }
    return false;
}

bool KRPTStaticImage::setSource(const QByteArray &src) noexcept
{
    resetState();
    _isFile = QFile(src).exists();
    _dirtySource = true;
    _dirtyPixmap = true;
    _src = src;
    _cache.clear();
    return true;
}

void KRPTStaticImage::setUseCache(bool useCache) noexcept
{
    if(useCache == _useCache)return;
    _useCache = useCache;
    _dirtyPixmap = true;
    if(_useCache)_pixmap.reset(); else _cache.clear();
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

bool KRPTStaticImage::drawVector(QPainter &painter, const QRectF &rect, const QColor &color) noexcept
{
    if(_dirtySource)
    {
        _dirtySource = false;
        if(!prepare())return false;
    }
    double s = 1;
    QRectF r = fitRect(rect, _viewBox, s);
    QTransform t;
    t.translate(r.x(), r.y());
    t.scale(s, s);
    QPen pen;
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setTransform(t, true);
    for(auto &item : _vectorItems)
    {
        painter.setOpacity(item.data.opacity);
        if(item.data.fill)painter.setBrush(color);
        else painter.setBrush(Qt::NoBrush);
        if(item.data.stroke)
        {
            pen.setColor(color);
            pen.setWidthF(item.data.strokeWidth);
            switch(item.data.lineCap)
            {
                case VectorItem::LineCap::ButtCap    : pen.setCapStyle(Qt::FlatCap   ); break;
                case VectorItem::LineCap::RoundCap   : pen.setCapStyle(Qt::RoundCap  ); break;
                case VectorItem::LineCap::SquareCap  : pen.setCapStyle(Qt::SquareCap ); break; 
            }
            switch(item.data.lineJoin)
            {
                case VectorItem::LineJoin::MiterJoin : pen.setJoinStyle(Qt::MiterJoin); break;
                case VectorItem::LineJoin::RoundJoin : pen.setJoinStyle(Qt::RoundJoin); break;
                case VectorItem::LineJoin::BevelJoin : pen.setJoinStyle(Qt::BevelJoin); break; 
            }
            painter.setPen(pen);
        }else painter.setPen(Qt::NoPen);
//        p.setClipPath(path.path);
        painter.drawPath(item.path);
    }
    painter.restore();
    return true;
}

bool KRPTStaticImage::drawRaster(QPainter &painter, const QRectF &rect) noexcept
{
    if(_dirtySource)
    {
        _dirtySource = false;
        if(!prepare())return false;
    }
    if(_cachePixmap.isNull())return false;
    double s;
    QRectF r = fitRect(rect, _viewBox, s);
    QSize sz(_cachePixmap.width() * s + 0.5, _cachePixmap.height() * s + 0.5);
#ifdef _STATIC_IMAGE_USE_PXIMAP
    painter.drawPixmap(r.topLeft(), _cachePixmap.scaled(sz, Qt::AspectRatioMode::IgnoreAspectRatio,
        Qt::TransformationMode::SmoothTransformation));
#else
    painter.drawImage(r.topLeft(), _cachePixmap.scaled(sz, Qt::AspectRatioMode::IgnoreAspectRatio,
        Qt::TransformationMode::SmoothTransformation));
#endif
    return true;
}

bool KRPTStaticImage::prepare() noexcept
{
    resetState();
    QByteArray src;
    if(!_isFile)src = _src; else
    {
        QFile file(_src);
        if(!file.open(QIODevice::ReadOnly))return false;
        src = file.readAll();
        file.close();
    }
    _sourceType = SourceType::Uncknown;
    if(isVector(src))_sourceType = SourceType::Vector; else
    if(isRaster(src) && _cachePixmap.loadFromData(src))
    {
        _sourceType = SourceType::Raster;
        _viewBox.setWidth (_cachePixmap.width ());
        _viewBox.setHeight(_cachePixmap.height());
    }
    if(_sourceType == SourceType::Vector)
    {
        QXmlStreamReader xml(src);
        int itemsCount = 0;
        int noneFillItemsCount = 0;
        bool hasViewBox = false;
        QRectF bBox;
        VectorItem item;
        std::vector<VectorItem::Data> stack;
        while(!xml.atEnd()) 
        {
            xml.readNext();
            if(xml.isStartElement()) 
            {
                QString tag = xml.name().toString();
                if(tag == "svg") 
                {
                    auto attrs = xml.attributes();
                    if(attrs.hasAttribute("viewBox")) 
                    {
                        QStringList vb = attrs.value("viewBox").toString().split(" ");
                        if(vb.size() == 4) 
                        {
                            _viewBox = QRectF(vb[0].toDouble(), vb[1].toDouble(), 
                                              vb[2].toDouble(), vb[3].toDouble());
                            hasViewBox = true;
                        }
                    }
                    VectorItem::Data data;
                    parseItemData(&attrs, data);
                    stack.emplace_back(data);
                }else
                if(tag == "g") 
                {
                    QXmlStreamAttributes attrs = xml.attributes();
                    VectorItem::Data data;
                    if(!stack.empty())data = stack.back();
                    parseItemData(&attrs, data);
                    stack.emplace_back(data);
                }else
                if(tag == "path" || tag == "polygon" || tag == "rect" || tag == "circle" || tag == "ellipse")
                {
                    QXmlStreamAttributes attrs = xml.attributes();
                    if((tag == "path"    && !attrs.hasAttribute("d")) || 
                       (tag == "polygon" && !attrs.hasAttribute("points")))continue;
                    ++itemsCount;
                    if(tag == "path")parsePath(attrs.value("d").toString(), item.path); else
                    if(tag == "polygon")parsePolygon(attrs.value("points").toString(), item.path); else
                    if(tag == "rect")
                    {
                        double x = attrs.hasAttribute("x"     ) ? attrs.value("x"     ).toDouble() : 0;
                        double y = attrs.hasAttribute("y"     ) ? attrs.value("y"     ).toDouble() : 0;
                        double w = attrs.hasAttribute("width" ) ? attrs.value("width" ).toDouble() : 0;
                        double h = attrs.hasAttribute("height") ? attrs.value("height").toDouble() : 0;
                        item.path.addRect(x, y, w, h);
                    }else
                    if(tag == "circle")
                    {
                        double cx = attrs.hasAttribute("cx") ? attrs.value("cx").toDouble() : 0;
                        double cy = attrs.hasAttribute("cy") ? attrs.value("cy").toDouble() : 0;
                        double r  = attrs.hasAttribute("r" ) ? attrs.value("r" ).toDouble() : 0;
                        QRectF rect(cx - r, cy - r, r * 2, r * 2);
                        item.path.addEllipse(rect);
                    }else
                    if(tag == "ellipse")
                    {
                        double cx = attrs.hasAttribute("cx") ? attrs.value("cx").toDouble() : 0;
                        double cy = attrs.hasAttribute("cy") ? attrs.value("cy").toDouble() : 0;
                        double rx = attrs.hasAttribute("rx") ? attrs.value("rx").toDouble() : 0;
                        double ry = attrs.hasAttribute("ry") ? attrs.value("ry").toDouble() : 0;
                        QRectF rect(cx - rx, cy - ry, rx * 2, ry * 2);
                        item.path.addEllipse(rect);
                    }
                    if(!stack.empty())item.data = stack.back();
                    parseItemData(&attrs, item.data);
                    if(!item.data.fill)++noneFillItemsCount;
                    if(item.data.stroke)_stroke = true;
                    if(item.data.strokeWidth > _maxStrokeWidth)_maxStrokeWidth = item.data.strokeWidth;
                    if(item.data.fillRule == VectorItem::Rule::EvenOdd)item.path.setFillRule(Qt::OddEvenFill);
                    else item.path.setFillRule(Qt::WindingFill);
                    if(!item.data.transform.isIdentity())
                        item.path = item.data.transform.map(item.path);
                    bBox = bBox.united(item.path.boundingRect());
                }
            }
            if(xml.isEndElement())
            {
                if(xml.name() == "svg" || xml.name() == "g") 
                {
                    if(!stack.empty())stack.pop_back();
                }else
                if(xml.name() == "path" || xml.name() == "polygon" || 
                   xml.name() == "rect" || xml.name() == "circle"  ||
                   xml.name() == "ellipse") 
                {
                    _vectorItems.emplace_back(item);
                    item.path.clear();
                }
            }
        }
        if(xml.hasError())
        {
            resetState();
            return false;
        }
        if(itemsCount == noneFillItemsCount)_fill = false;
        if(!hasViewBox)_viewBox = bBox;
    #if 1
        double scale = 256.0 / (std::max(_viewBox.width(), _viewBox.height()));
        QTransform t;
        t.scale(scale, scale);
        for(auto &item : _vectorItems)
        {
            item.path = t.map(item.path);
            item.data.strokeWidth *= scale;
        }
        _viewBox.setWidth (_viewBox.width () * scale);
        _viewBox.setHeight(_viewBox.height() * scale);
    #endif
    }
    _src.clear();
    return _sourceType != SourceType::Uncknown;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

std::vector<double> KRPTStaticImage::parseArguments(const QString &src) noexcept
{
    std::vector<double> ret;
    bool delimer = false;
    bool last = false;
    QString cur;
    for(auto &c : src)
    {
        if(c != " " && c != ",")
        {
            cur += c;
            delimer = false;
            last = true;
            continue;
        }
        if(!delimer)
        {
            bool ok;
            double v = cur.toDouble(&ok); 
            if(ok)ret.push_back(v);
            cur.clear();
            delimer = true;
            last = false;
        }
    }
    if(last)
    {
        bool ok;
        double v = cur.toDouble(&ok); 
        if(ok)ret.push_back(v);
    }
    return ret;
}

QTransform KRPTStaticImage::parseTransform(const QString &src) noexcept
{
    static std::vector<QString> names = {"matrix", "translate", "rotate", "scale", "skewX", "skewY"};
    QTransform transform;
    QString in = src;
    int pos = 0;
    while(true)
    {
        int b = -1, e = -1, id = 0;
        for(int i = 0; i < (int)names.size(); ++i)
        {
            int p = in.indexOf(names[i], pos);
            if(p < 0)continue;
            if(b == -1 || p < b){b = p; id = i;}
        }
        if(b < 0)break;
        pos = b + names[id].size();
        b = in.indexOf('(', pos);
        if(b < 0)break;
        ++b;
        pos = b;
        e = in.indexOf(')', pos);
        if(e < 0)break;
        pos = e;
        std::vector<double> args = parseArguments(src.mid(b, e - b));
        switch(id)
        {
            case 0 :
                if(args.size() == 6)
                    transform = QTransform(args[0], args[1], args[2], args[3], args[3], args[4]);
                break;
            case 1 :
                if(args.size() == 2)
                    transform.translate(args[0], args[1]);
                break;
            case 2 :
                if(args.size() == 1)
                    transform.rotate(args[0]);
                break;
            case 3 :
                if(args.size() == 2)
                    transform.scale(args[0], args[1]);
                break;
            case 4 :
                if(args.size() == 1)
                    transform.shear(std::tan(degToRad(args[0])), 0);
                break;
            case 5 :
                if(args.size() == 1)
                    transform.shear(0, std::tan(degToRad(args[0])));
                break;
        }
    }
    return transform;
}

void KRPTStaticImage::parseItemData(QXmlStreamAttributes *attrs, VectorItem::Data &data) noexcept
{
    if(attrs->hasAttribute("fill"))
    {
        QString value = attrs->value("fill").toString();
        data.fill = value != "none";
    }
    if(attrs->hasAttribute("stroke"))
    {
        QString value = attrs->value("stroke").toString();
        data.stroke = value != "none";
    }
    if(attrs->hasAttribute("stroke-width"))
    {
        QString value = attrs->value("stroke-width").toString();
        bool ok;
        data.strokeWidth = value.toFloat(&ok);
        if(!ok)data.strokeWidth = 1;
    }
    if(attrs->hasAttribute("opacity"))
    {
        QString value = attrs->value("opacity").toString();
        bool ok;
        data.opacity = value.toFloat(&ok);
        if(!ok)data.opacity = 1;
    }
    if(attrs->hasAttribute("fill-rule"))
    {
        QString value = attrs->value("fill-rule").toString();
        if(value == "nonzero")data.fillRule = VectorItem::Rule::NoneZero; else
        if(value == "evenodd")data.fillRule = VectorItem::Rule::EvenOdd;
    }
    if(attrs->hasAttribute("clip-rule"))
    {
        QString value = attrs->value("clip-rule").toString();
        if(value == "nonzero")data.clipRule = VectorItem::Rule::NoneZero; else
        if(value == "evenodd")data.clipRule = VectorItem::Rule::EvenOdd;
    }
    if(attrs->hasAttribute("stroke-linecap"))
    {
        QString value = attrs->value("stroke-linecap").toString();
        if(value == "butt"  )data.lineCap = VectorItem::LineCap::ButtCap  ; else
        if(value == "round" )data.lineCap = VectorItem::LineCap::RoundCap ; else
        if(value == "square")data.lineCap = VectorItem::LineCap::SquareCap;
    }
    if(attrs->hasAttribute("stroke-linejoin"))
    {
        QString value = attrs->value("stroke-linejoin").toString();
        if(value == "miter")data.lineJoin = VectorItem::LineJoin::MiterJoin; else
        if(value == "round")data.lineJoin = VectorItem::LineJoin::RoundJoin; else
        if(value == "bevel")data.lineJoin = VectorItem::LineJoin::BevelJoin;
    }
    if(attrs->hasAttribute("transform"))
    {
        QString value = attrs->value("transform").toString();
        QTransform transform = parseTransform(value);
        if(!transform.isIdentity())
            data.transform *= transform;
    }
}

bool KRPTStaticImage::parsePolygon(const QString &src, QPainterPath &path) noexcept
{
    _data = src;
    _pos  = 0;
    _lastPoint = QPointF();
    _lastControl = QPointF();
    bool first = true;
    while(!end())
    {
        skipSpaces();
        if(end())break;
        if(first)moveToAbs(path);
        else lineToAbs(path);
        first = false;
    }
    return true;
}

bool KRPTStaticImage::parsePath(const QString &src, QPainterPath &path) noexcept
{
    _data = src;
    _pos  = 0;
    _lastPoint = QPointF();
    _lastControl = QPointF();
    QChar cmd = QChar(0);
    while(!end())
    {
        skipSpaces();
        if(end())break;
        QChar c = _data[_pos];
        if(c.isLetter()){cmd = c; _pos++;}
        switch(cmd.toLatin1())
        {
            case 'M': moveToAbs     (path); skipSpaces(); break;
            case 'm': moveToRel     (path); skipSpaces(); break;
            case 'L': lineToAbs     (path); skipSpaces(); break;
            case 'l': lineToRel     (path); skipSpaces(); break;
            case 'H': hLineAbs      (path); skipSpaces(); break;
            case 'h': hLineRel      (path); skipSpaces(); break;
            case 'V': vLineAbs      (path); skipSpaces(); break;
            case 'v': vLineRel      (path); skipSpaces(); break;
            case 'C': cubicAbs      (path); skipSpaces(); break;
            case 'c': cubicRel      (path); skipSpaces(); break;
            case 'S': smoothCubicAbs(path); skipSpaces(); break;
            case 's': smoothCubicRel(path); skipSpaces(); break;
            case 'Q': quadAbs       (path); skipSpaces(); break;
            case 'q': quadRel       (path); skipSpaces(); break;
            case 'T': smoothQuadAbs (path); skipSpaces(); break;
            case 't': smoothQuadRel (path); skipSpaces(); break;
            case 'A': arcAbs        (path); skipSpaces(); break;
            case 'a': arcRel        (path); skipSpaces(); break;
            case 'Z':
            case 'z':
                path.closeSubpath();
                _lastPoint = path.currentPosition();
                skipComma();
                break;
            default: _pos++; break;
        }
        if(cmd == 'M')cmd = 'L'; else
        if(cmd == 'm')cmd = 'l';
    }
    return true;
}

void KRPTStaticImage::resetState() noexcept 
{
    _stroke         = false;
    _fill           = true;
    _pos            = 0;
    _maxStrokeWidth = 1;
    _dirtyPixmap    = true;
    _lastPoint      = QPointF();
    _lastControl    = QPointF();
    _viewBox        = QRectF ();
    _cachePixmap    = Image();
    _pixmap.reset     ();
    _data.clear       ();
    _vectorItems.clear();
}

bool KRPTStaticImage::end() const noexcept 
{
    return _pos >= _data.size();
}

void KRPTStaticImage::skipSpaces() noexcept
{
    while(!end() && _data[_pos].isSpace())_pos++;
}

void KRPTStaticImage::skipComma() noexcept
{
    skipSpaces();
    if(!end() && _data[_pos] == ',')_pos++;
    skipSpaces();
}

double KRPTStaticImage::readNumber() noexcept
{
    skipSpaces();
    int start = _pos;
    bool hasSign = false;
    bool hasDot = false;
    bool hasExp = false;
    bool first = true;
    while(!end())
    {
        QChar c = _data[_pos];
        if(c.isDigit())_pos++; else 
        if((c == '+' || c == '-') && !hasSign)
        {
            if(first){hasSign = true; _pos++;}else 
            if(hasExp){hasExp = false;_pos++;}else break;
        }else 
        if(c == '.' && !hasDot){hasDot = true; _pos++;}else 
        if((c == 'e' || c == 'E') && !hasExp){hasExp = true; hasSign = false; _pos++;}
        else break;
        first = false;
    }
    return _data.mid(start, _pos - start).toDouble();
}

QPointF KRPTStaticImage::readPoint() noexcept
{
    double x = readNumber(); skipComma();
    double y = readNumber(); skipComma();
    return QPointF(x, y);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTStaticImage::moveToAbs(QPainterPath &path) noexcept
{
    QPointF p = readPoint();
    path.moveTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::moveToRel(QPainterPath &path) noexcept
{
    QPointF p = _lastPoint + readPoint();
    path.moveTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::lineToAbs(QPainterPath &path) noexcept
{
    QPointF p = readPoint();
    path.lineTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::lineToRel(QPainterPath &path) noexcept
{
    QPointF p = _lastPoint + readPoint();
    path.lineTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::hLineAbs(QPainterPath &path) noexcept
{
    double x = readNumber();
    QPointF p(x, _lastPoint.y());
    path.lineTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::hLineRel(QPainterPath &path) noexcept
{
    double dx = readNumber();
    QPointF p(_lastPoint.x() + dx, _lastPoint.y());
    path.lineTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::vLineAbs(QPainterPath &path) noexcept
{
    double y = readNumber();
    QPointF p(_lastPoint.x(), y);
    path.lineTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::vLineRel(QPainterPath &path) noexcept
{
    double dy = readNumber();
    QPointF p(_lastPoint.x(), _lastPoint.y() + dy);
    path.lineTo(p);
    _lastPoint = p;
}

void KRPTStaticImage::cubicAbs(QPainterPath &path) noexcept
{
    QPointF c1 = readPoint();
    QPointF c2 = readPoint();
    QPointF p  = readPoint();
    path.cubicTo(c1, c2, p);
    _lastControl = c2;
    _lastPoint = p;
}

void KRPTStaticImage::cubicRel(QPainterPath &path) noexcept
{
    QPointF c1 = _lastPoint + readPoint();
    QPointF c2 = _lastPoint + readPoint();
    QPointF p  = _lastPoint + readPoint();
    path.cubicTo(c1, c2, p);
    _lastControl = c2;
    _lastPoint = p;
}

void KRPTStaticImage::smoothCubicAbs(QPainterPath &path) noexcept
{
    QPointF c1 = reflect(_lastControl, _lastPoint);
    QPointF c2 = readPoint();
    QPointF p  = readPoint();
    path.cubicTo(c1, c2, p);
    _lastControl = c2;
    _lastPoint = p;
}

void KRPTStaticImage::smoothCubicRel(QPainterPath &path) noexcept
{
    QPointF c1 = reflect(_lastControl, _lastPoint);
    QPointF c2 = _lastPoint + readPoint();
    QPointF p  = _lastPoint + readPoint();
    path.cubicTo(c1, c2, p);
    _lastControl = c2;
    _lastPoint = p;
}

void KRPTStaticImage::quadAbs(QPainterPath &path) noexcept
{
    QPointF c(readPoint());
    QPointF p(readPoint());
    path.quadTo(c, p);
    _lastControl = c;
    _lastPoint = p;
}

void KRPTStaticImage::quadRel(QPainterPath &path) noexcept
{
    QPointF c = _lastPoint + readPoint();
    QPointF p = _lastPoint + readPoint();
    path.quadTo(c, p);
    _lastControl = c;
    _lastPoint = p;
}

void KRPTStaticImage::smoothQuadAbs(QPainterPath &path) noexcept
{
    QPointF c = reflect(_lastControl, _lastPoint);
    QPointF p = readPoint();
    path.quadTo(c, p);
    _lastControl = c;
    _lastPoint = p;
}

void KRPTStaticImage::smoothQuadRel(QPainterPath &path) noexcept
{
    QPointF c = reflect(_lastControl, _lastPoint);
    QPointF p = _lastPoint + readPoint();
    path.quadTo(c, p);
    _lastControl = c;
    _lastPoint = p;
}

QPointF KRPTStaticImage::reflect(const QPointF& c, const QPointF& p) noexcept
{
    return QPointF(2 * p.x() - c.x(), 2 * p.y() - c.y());
}

void KRPTStaticImage::arcAbs(QPainterPath &path) noexcept
{
    double rx  = readNumber(); skipComma();
    double ry  = readNumber(); skipComma();
    double rot = readNumber(); skipComma();
    int large  = int(readNumber()); skipComma();
    int sweep  = int(readNumber()); skipComma();
    QPointF p(readPoint());
    arcToBezier(path, path.currentPosition(), p, rx, ry, rot, large, sweep);
    _lastPoint = p;
}

void KRPTStaticImage::arcRel(QPainterPath &path) noexcept
{
    double rx = readNumber(); skipComma();
    double ry = readNumber(); skipComma();
    double rot = readNumber(); skipComma();
    int large = int(readNumber()); skipComma();
    int sweep = int(readNumber()); skipComma();
    QPointF dp(readPoint());
    QPointF p = _lastPoint + dp;
    arcToBezier(path, path.currentPosition(), p, rx, ry, rot, large, sweep);
    _lastPoint = p;
}

void KRPTStaticImage::arcToBezier(QPainterPath &path, const QPointF& p0, const QPointF& p1, double rx, double ry, 
    double xAxisRotation, bool largeArcFlag, bool sweepFlag) noexcept
{
    if(rx == 0 || ry == 0) {path.lineTo(p1); return;}
    double phi = degToRad(xAxisRotation);
    double cosPhi = std::cos(phi);
    double sinPhi = std::sin(phi);
    double dx = (p0.x() - p1.x()) / 2.0;
    double dy = (p0.y() - p1.y()) / 2.0;
    double x1 = cosPhi * dx + sinPhi * dy;
    double y1 = -sinPhi * dx + cosPhi * dy;
    double rx2 = rx * rx;
    double ry2 = ry * ry;
    double x12 = x1 * x1;
    double y12 = y1 * y1;
    double lambda = x12 / rx2 + y12 / ry2;
    if(lambda > 1.0) 
    {
        double scale = std::sqrt(lambda);
        rx *= scale;
        ry *= scale;
        rx2 = rx * rx;
        ry2 = ry * ry;
    }
    double sign = (largeArcFlag == sweepFlag) ? -1 : 1;
    double coef = sign * std::sqrt(std::max(0.0,
        (rx2 * ry2 - rx2 * y12 - ry2 * x12) / (rx2 * y12 + ry2 * x12)));
    double cx1 = coef * (rx * y1 / ry);
    double cy1 = coef * (-ry * x1 / rx);
    double cx = cosPhi * cx1 - sinPhi * cy1 + (p0.x() + p1.x()) / 2.0;
    double cy = sinPhi * cx1 + cosPhi * cy1 + (p0.y() + p1.y()) / 2.0;
    auto angle = [](double ux, double uy, double vx, double vy) 
    {
        double dot = ux * vx + uy * vy;
        double len = std::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
        double ang = std::acos(std::max(-1.0, std::min(1.0, dot / len)));
        if(ux * vy - uy * vx < 0)ang = -ang;
        return ang;
    };
    double theta1 = angle(1, 0, (x1 - cx1) / rx, (y1 - cy1) / ry);
    double dtheta = angle((x1 - cx1) / rx, (y1 - cy1) / ry,
                          (-x1 - cx1) / rx, (-y1 - cy1) / ry);
    if(!sweepFlag && dtheta > 0)dtheta -= 2 * _PI;else 
    if(sweepFlag && dtheta < 0)dtheta += 2 * _PI;
    int segments = std::ceil(std::abs(dtheta) / (_PI / 2));
    double delta = dtheta / segments;
    for(int i = 0; i < segments; i++) 
    {
        double t1 = theta1 + i * delta;
        double t2 = t1 + delta;
        arcSegmentToBezier(path, cx, cy, rx, ry, phi, t1, t2);
    }
}

void KRPTStaticImage::arcSegmentToBezier(QPainterPath &path, double cx, double cy, double rx, double ry,
    double phi, double t1, double t2) noexcept
{
    double cosPhi = std::cos(phi);
    double sinPhi = std::sin(phi);
    auto point = [&](double t) 
    {
        double x = rx * std::cos(t);
        double y = ry * std::sin(t);
        return QPointF(cx + cosPhi * x - sinPhi * y, cy + sinPhi * x + cosPhi * y);
    };
    auto tangent = [&](double t) 
    {
        double x = -rx * std::sin(t);
        double y = ry * std::cos(t);
        return QPointF(cosPhi * x - sinPhi * y, sinPhi * x + cosPhi * y);
    };
    QPointF p0 = point(t1);
    QPointF p3 = point(t2);
    QPointF d0 = tangent(t1);
    QPointF d3 = tangent(t2);
    double dt = t2 - t1;
    double k = std::tan(dt / 4) * 4 / 3;
    QPointF c1 = p0 + d0 * k;
    QPointF c2 = p3 - d3 * k;
    path.cubicTo(c1, c2, p3);
}
