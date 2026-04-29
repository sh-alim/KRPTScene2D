//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneImageCache.h"

#if 0

#include <QSvgRenderer>
#include <QPainter>
#include <QPicture>

#include <QFile>

//#include <QDomDocument>
#include <QRegularExpression>

//########################################################################################################################
//#
//########################################################################################################################

#pragma once
#include <QPainterPath>
//#include <QDomDocument>
#include <QFile>
#include <QtMath>

class SvgPathParser
{
public:
    static QPainterPath fromSvgFile(const QString& filePath)
    {
        QFile f(filePath);
        if (!f.open(QIODevice::ReadOnly))
            return QPainterPath();
        QPainterPath result;

    #if 0
        QDomDocument doc;
        doc.setContent(&f);


        QDomNodeList paths = doc.elementsByTagName("path");
        for (int i = 0; i < paths.count(); i++)
        {
            QDomElement e = paths.at(i).toElement();
            QString d = e.attribute("d");
            if (!d.isEmpty())
                result.addPath(parsePath(d));
        }
    #endif
        return result;
    }



    static QPainterPath parsePath(const QString& d)
    {
        SvgPathParser p(d);
        return p.parseInternal();
    }

private:
    QString data;
    int pos = 0;
    QPointF lastPoint;
    QPointF lastControl;
    QPainterPath path;

    SvgPathParser(const QString& d) : data(d) {}

    bool end() const { return pos >= data.size(); }

    void skipSpaces()
    {
        while (!end() && data[pos].isSpace())
            pos++;
    }

    void skipComma()
    {
        skipSpaces();
        if (!end() && data[pos] == ',')
            pos++;
        skipSpaces();
    }

    double readNumber()
    {
        skipSpaces();

        int start = pos;
        bool hasSign = false;
        bool hasDot = false;
        bool hasExp = false;


        bool first = true;
        while (!end())
        {
            QChar c = data[pos];

        #if 0
            if (c.isDigit()) pos++;
            else if ((c == '+' || c == '-') && !hasSign) { hasSign = true; pos++; }
            else if (c == '.' && !hasDot) { hasDot = true; pos++; }
            else if ((c == 'e' || c == 'E') && !hasExp) { hasExp = true; hasSign = false; pos++; }
            else break;
        #else
            if(c.isDigit())
            {
                pos++;
            }
//            else if ((c == '+' || c == '-') && !hasSign) { hasSign = true; pos++; }
            else if ((c == '+' || c == '-') && !hasSign)
            {
                if(first)
                {
                    hasSign = true; 
                    pos++; 
                }else 
                    break;
            }
            else if (c == '.' && !hasDot) { hasDot = true; pos++; }
            else if ((c == 'e' || c == 'E') && !hasExp) { hasExp = true; hasSign = false; pos++; }
            else break;

            first = false;

        #endif
        }

//        qDebug() << data.mid(start, pos - start);

        return data.mid(start, pos - start).toDouble();
    }

    QPointF readPoint()
    {
        double x = readNumber();
        skipComma();
        double y = readNumber();
        skipComma();
        return QPointF(x, y);
    }

    // -------------------------
    // PARSER CORE
    // -------------------------

    QPainterPath parseInternal()
    {
        QChar cmd = QChar(0);

        while (!end())
        {
            skipSpaces();
            if (end()) break;

            QChar c = data[pos];

            if (c.isLetter()) {
                cmd = c;
                pos++;
            }

//            if ((cmd == 'M' || cmd == 'm') && !data[pos].isLetter())
//                cmd = (cmd == 'M') ? 'L' : 'l';

            switch (cmd.toLatin1())
            {
            case 'M': do { moveToAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 'm': do { moveToRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'L': do { lineToAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 'l': do { lineToRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'H': do { hLineAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 'h': do { hLineRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'V': do { vLineAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 'v': do { vLineRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'C': do { cubicAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 'c': do { cubicRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'S': do { smoothCubicAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 's': do { smoothCubicRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'Q': do { quadAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 'q': do { quadRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'T': do { smoothQuadAbs(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
            case 't': do { smoothQuadRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;

            case 'A': do { arcAbs();skipSpaces();} while (!end() && (data[pos].isDigit() || data[pos] == '+' || data[pos] == '-' || data[pos] == '.'));
                break;

            case 'a': do { arcRel(); skipSpaces(); } while (!end() && !data[pos].isLetter()); break;
//            case 'a': do { arcRel();skipSpaces();} while (!end() && (data[pos].isDigit() || data[pos] == '+' || data[pos] == '-' || data[pos] == '.'));
                break;

            case 'Z':
            case 'z':
                path.closeSubpath();
                lastPoint = path.currentPosition();
                skipComma();
//                pos++;
                break;

            default:
                pos++;
                break;
            }
        }

        return path;
    }

    // -------------------------
    // BASIC COMMANDS
    // -------------------------

    void moveToAbs()
    {
        QPointF p = readPoint();
        path.moveTo(p);
        lastPoint = p;
    }

    void moveToRel()
    {
        QPointF p = lastPoint + readPoint();
        path.moveTo(p);
        lastPoint = p;
    }

    void lineToAbs()
    {
        QPointF p = readPoint();
        path.lineTo(p);
        lastPoint = p;
    }

    void lineToRel()
    {
        QPointF p = lastPoint + readPoint();
        path.lineTo(p);
        lastPoint = p;
    }

    void hLineAbs()
    {
        double x = readNumber();
        QPointF p(x, lastPoint.y());
        path.lineTo(p);
        lastPoint = p;
    }

    void hLineRel()
    {
        double dx = readNumber();
        QPointF p(lastPoint.x() + dx, lastPoint.y());
        path.lineTo(p);
        lastPoint = p;
    }

    void vLineAbs()
    {
        double y = readNumber();
        QPointF p(lastPoint.x(), y);
        path.lineTo(p);
        lastPoint = p;
    }

    void vLineRel()
    {
        double dy = readNumber();
        QPointF p(lastPoint.x(), lastPoint.y() + dy);
        path.lineTo(p);
        lastPoint = p;
    }

    // -------------------------
    // CUBIC
    // -------------------------

    void cubicAbs()
    {
        QPointF c1 = readPoint(); skipComma();
        QPointF c2 = readPoint(); skipComma();
        QPointF p  = readPoint();

        path.cubicTo(c1, c2, p);
        lastControl = c2;
        lastPoint = p;
    }

    void cubicRel()
    {
        QPointF c1 = lastPoint + readPoint(); skipComma();
        QPointF c2 = lastPoint + readPoint(); skipComma();
        QPointF p  = lastPoint + readPoint();

        path.cubicTo(c1, c2, p);
        lastControl = c2;
        lastPoint = p;
    }

    void smoothCubicAbs()
    {
        QPointF c1 = reflect(lastControl, lastPoint);
        QPointF c2 = readPoint(); skipComma();
        QPointF p  = readPoint();

        path.cubicTo(c1, c2, p);
        lastControl = c2;
        lastPoint = p;
    }

    void smoothCubicRel()
    {
        QPointF c1 = reflect(lastControl, lastPoint);
        QPointF c2 = lastPoint + readPoint(); skipComma();
        QPointF p  = lastPoint + readPoint();

        path.cubicTo(c1, c2, p);
        lastControl = c2;
        lastPoint = p;
    }

    // -------------------------
    // QUADRATIC
    // -------------------------

    void quadAbs()
    {
        QPointF c = readPoint(); skipComma();
        QPointF p = readPoint();

        path.quadTo(c, p);
        lastControl = c;
        lastPoint = p;
    }

    void quadRel()
    {
        QPointF c = lastPoint + readPoint(); skipComma();
        QPointF p = lastPoint + readPoint();

        path.quadTo(c, p);
        lastControl = c;
        lastPoint = p;
    }

    void smoothQuadAbs()
    {
        QPointF c = reflect(lastControl, lastPoint);
        QPointF p = readPoint();

        path.quadTo(c, p);
        lastControl = c;
        lastPoint = p;
    }

    void smoothQuadRel()
    {
        QPointF c = reflect(lastControl, lastPoint);
        QPointF p = lastPoint + readPoint();

        path.quadTo(c, p);
        lastControl = c;
        lastPoint = p;
    }

    QPointF reflect(const QPointF& c, const QPointF& p)
    {
        return QPointF(2*p.x() - c.x(), 2*p.y() - c.y());
    }

    // -------------------------
    // ARC
    // -------------------------

    void arcAbs()
    {
        double rx = readNumber(); skipComma();
        double ry = readNumber(); skipComma();
        double rot = readNumber(); skipComma();
        int large = int(readNumber()); skipComma();
        int sweep = int(readNumber()); skipComma();

//        QPointF p(readNumber(), readNumber());

        QPointF p(readPoint());


//        double x = readNumber(); skipComma();
//        double y = readNumber(); skipComma();
//        QPointF p(x, y);


        arcToBezier(path.currentPosition(), p, rx, ry, rot, large, sweep);
        lastPoint = p;
    }

    void arcRel()
    {
//a9,9 0 0 0 -9,9

        double rx = readNumber(); skipComma();
        double ry = readNumber(); skipComma();
        double rot = readNumber(); skipComma();
        int large = int(readNumber()); skipComma();
        int sweep = int(readNumber()); skipComma();

//        QPointF dp(readNumber(), readNumber());

//        double x = readNumber(); skipComma();
//        double y = readNumber(); skipComma();
//        QPointF dp(x, y);


        QPointF dp(readPoint());


        QPointF p = lastPoint + dp;

        arcToBezier(path.currentPosition(), p, rx, ry, rot, large, sweep);
        lastPoint = p;
    }

    // -------------------------
    // ARC → BEZIER
    // -------------------------

    void arcToBezier(const QPointF& p0, const QPointF& p1,
                     double rx, double ry, double xAxisRotation,
                     bool largeArcFlag, bool sweepFlag)
    {
        if (rx == 0 || ry == 0) {
            path.lineTo(p1);
            return;
        }

        double phi = qDegreesToRadians(xAxisRotation);
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
        if (lambda > 1.0) {
            double scale = std::sqrt(lambda);
            rx *= scale;
            ry *= scale;
            rx2 = rx * rx;
            ry2 = ry * ry;
        }

        double sign = (largeArcFlag == sweepFlag) ? -1 : 1;
        double coef = sign * std::sqrt(
            std::max(0.0,
                (rx2 * ry2 - rx2 * y12 - ry2 * x12) /
                (rx2 * y12 + ry2 * x12)
            )
        );

        double cx1 = coef * (rx * y1 / ry);
        double cy1 = coef * (-ry * x1 / rx);

        double cx = cosPhi * cx1 - sinPhi * cy1 + (p0.x() + p1.x()) / 2.0;
        double cy = sinPhi * cx1 + cosPhi * cy1 + (p0.y() + p1.y()) / 2.0;

        auto angle = [](double ux, double uy, double vx, double vy) {
            double dot = ux * vx + uy * vy;
            double len = std::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
            double ang = std::acos(std::max(-1.0, std::min(1.0, dot / len)));
            if (ux * vy - uy * vx < 0)
                ang = -ang;
            return ang;
        };

        double theta1 = angle(1, 0, (x1 - cx1) / rx, (y1 - cy1) / ry);
        double dtheta = angle(
            (x1 - cx1) / rx, (y1 - cy1) / ry,
            (-x1 - cx1) / rx, (-y1 - cy1) / ry
        );

        if (!sweepFlag && dtheta > 0)
            dtheta -= 2 * M_PI;
        else if (sweepFlag && dtheta < 0)
            dtheta += 2 * M_PI;

        int segments = std::ceil(std::abs(dtheta) / (M_PI / 2));
        double delta = dtheta / segments;

        for (int i = 0; i < segments; i++) {
            double t1 = theta1 + i * delta;
            double t2 = t1 + delta;
            arcSegmentToBezier(cx, cy, rx, ry, phi, t1, t2);
        }
    }

    void arcSegmentToBezier(double cx, double cy,
                            double rx, double ry,
                            double phi,
                            double t1, double t2)
    {
        double cosPhi = std::cos(phi);
        double sinPhi = std::sin(phi);

        auto point = [&](double t) {
            double x = rx * std::cos(t);
            double y = ry * std::sin(t);
            return QPointF(
                cx + cosPhi * x - sinPhi * y,
                cy + sinPhi * x + cosPhi * y
            );
        };

        auto tangent = [&](double t) {
            double x = -rx * std::sin(t);
            double y = ry * std::cos(t);
            return QPointF(
                cosPhi * x - sinPhi * y,
                sinPhi * x + cosPhi * y
            );
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
};

//########################################################################################################################
//#
//########################################################################################################################

#if 0
QPainterPath svgToPainterPath(const QString &file)
{
    QSvgRenderer renderer(file);

    QPicture picture;
    {
        QPainter p(&picture);
        renderer.render(&p);
    }

    QPainterPath path;
    QPainterPathStroker stroker;

    for (const auto &cmd : picture.data()) 
    {
        if (cmd.type == QPicture::PdcDrawPath) {
            path.addPath(cmd.path);
        }
        else if (cmd.type == QPicture::PdcDrawLine) {
            QPainterPath tmp;
            tmp.moveTo(cmd.line.p1());
            tmp.lineTo(cmd.line.p2());
            path.addPath(stroker.createStroke(tmp));
        }
    }

    return path.simplified();
}
#endif

KRPTSceneImageCache::Image KRPTSceneImageCache::_get(uint32_t index, const QSizeF &size) noexcept
{
    Image image;

    auto find = _index.find(index);

    if(find != _index.end())
    {
        image = find->second;
    }else
    {
//        image = std::make_shared<QImage>(size.toSize(), QImage::Format_ARGB32_Premultiplied);
//        image = _index.emplace(index, image).first->second;
        image = _index.emplace(index, std::make_shared<QImage>()).first->second;
    }
//    if(image->isNull())
    {
        *image = QImage(size.toSize(), QImage::Format_ARGB32_Premultiplied);

        image->fill(Qt::transparent);

//D:\Desktop\icons\1\Expand

        QSvgRenderer renderer(QString("D:/Desktop/icons/1/Expand/22-Option.svg"));

        QPainter p(image.get());
        renderer.render(&p);
    }

    return image;
}

static const char* kPath =
"M23,5 "
"H9 "
"a9,9,0,0,0-9,9 "
"v4 "
"a9,9,0,0,0,9,9 "
"H23 "
"a9,9,0,0,0,9-9 "
"V14 "
"A9,9,0,0,0,23,5 "
"Z "

"m7,13"
"a7,7,0,0,1-7,7"
"H9"
"a7,7,0,0,1-7-7"
"V14"
"A7,7,0,0,1,9,7"
"H23 "
"a7,7,0,0,1,7,7"
"Z"

"M20,9"
"a7,7,0,1,0,7,7"
"A7,7,0,0,0,20,9"
"Z"
"m0,12"
"a5,5,0,1,1,5-5A5,5,0,0,1,20,21Z";

;

static const char* kPath1 =
"M18,32H14a1,1,0,0,1-1-1V28.64a12.8,12.8,0,0,1-3.82-1.58L7.51,28.73"
"a1,1,0,0,1-.71.29h0a1,1,0,0,1-.71-.29L3.27,25.9a1,1,0,0,1,0-1.41l1.67-1.67A12.79,12.79,0,0,1,3.36,19"
"H1a1,1,0,0,1-1-1V14a1,1,0,0,1,1-1H3.36A12.79,12.79,0,0,1,4.94,9.18L3.27,7.51a1,1,0,0,1,0-1.41L6.1,3.27a1,1,0,0,1,1.41,0L9.18,4.94"
"A12.8,12.8,0,0,1,13,3.36V1a1,1,0,0,1,1-1h4a1,1,0,0,1,1,1V3.36a12.8,12.8,0,0,1,3.82,1.58l1.67-1.67A1,1,0,0,1,25.19,3h0a1,1,0,0,1,.71.29L28.73,6.1a1,1,0,0,1,0,1.41"
"L27.06,9.18A12.79,12.79,0,0,1,28.64,13H31a1,1,0,0,1,1,1v4a1,1,0,0,1-1,1H28.64a12.79,12.79,0,0,1-1.58,3.82l1.67,1.67a1,1,0,0,1,0,1.41L25.9,28.73a1,1,0,0,1-1.41,0l-1.67-1.67"
"A12.8,12.8,0,0,1,19,28.64V31A1,1,0,0,1,18,32"
"Z"
"m-3-2"
//"M18,32"
"h2"
"V27.82"
"a1,1,0,0,1,.83-1"
"A10.85,10.85,0,0,0,22.37,25"
"a1,1,0,0,1,1.29.11"
"l1.54,1.54,1.41-1.41-1.54-1.54"
"A1,1,0,0,1,25,22.37a10.85,10.85,0,0,0,1.87-4.53,1,1,0,0,1,1-.83H30V15H27.82a1,1,0,0,1-1-.83A10.85,10.85,0,0,0,25,9.63a1,1,0,0,1,.11-1.29l1.54-1.54L25.19,5.39,23.65,6.93A1,1,0,0,1,22.37,7a10.85,10.85,0,0,0-4.53-1.87,1,1,0,0,1-.83-1V2H15V4.18a1,1,0,0,1-.83,1A10.85,10.85,0,0,0,9.63,7a1,1,0,0,1-1.29-.11L6.81,5.39,5.39,6.81,6.93,8.35A1,1,0,0,1,7,9.63a10.85,10.85,0,0,0-1.87,4.53,1,1,0,0,1-1,.83H2v2H4.18a1,1,0,0,1,1,.83A10.85,10.85,0,0,0,7,22.37a1,1,0,0,1-.11,1.29L5.39,25.19l1.41,1.41,1.54-1.54A1,1,0,0,1,9.63,25a10.85,10.85,0,0,0,4.53,1.87,1,1,0,0,1,.83,1Z"
"M16,22a6,6,0,1,1,6-6A6,6,0,0,1,16,22Zm0-10a4,4,0,1,0,4,4A4,4,0,0,0,16,12Z"
;

static const char* kPath2 =
    "M54.162,29.047c-0.214-0.236-0.855-0.552-1.583-0.78c-0.027-0.211-0.078-0.414-0.155-0.602"
	"c-0.249-0.602-0.737-0.977-1.373-1.055c-1.099-0.133-2.295,0.385-3.153,0.873c-1.731-6.04-6.791-11.056-13.552-13.168"
	"c0.494-1.039,0.778-2.195,0.778-3.421c0-4.415-3.593-8.008-8.009-8.008c-4.415,0-8.007,3.593-8.007,8.008"
	"c0,1.27,0.305,2.466,0.832,3.534c-2.295,0.76-4.461,1.857-6.335,3.262c-2.129-1.111-4.454-1.094-6.028,0.081l-0.802,0.598"
	"l2.644,3.542c-1.313,1.814-2.253,3.792-2.802,5.897H5.5c-1.71,0-3.102,1.392-3.102,3.103v8.936c0,1.711,1.392,3.103,3.102,3.103"
	"h4.425c1.297,1.635,2.871,3.068,4.692,4.271v4.18c0,1.266,0.92,2.296,2.051,2.296h3.811c1.131,0,2.051-1.03,2.051-2.296v-0.923"
	"c2.922,0.601,5.9,0.656,8.952,0.102v0.93c0,1.266,0.92,2.295,2.051,2.295h3.811c1.131,0,2.051-1.029,2.051-2.295v-3.908"
	"c5.745-3.533,9.155-9.289,9.155-15.507c0-1.226-0.15-2.428-0.41-3.601c0.781-0.48,1.892-1.006,2.788-0.891"
	"c0.268,0.033,0.445,0.164,0.558,0.41c-0.615-0.088-1.173-0.053-1.474,0.198c-0.427,0.356-0.614,0.951-0.465,1.48"
	"c0.16,0.57,0.659,0.957,1.37,1.063c0.057,0.008,0.114,0.013,0.171,0.013c0.378,0,0.73-0.188,1.005-0.541"
	"c0.203-0.26,0.348-0.586,0.43-0.935c0.425,0.142,0.783,0.302,0.9,0.427c0.187,0.205,0.502,0.22,0.706,0.034"
	"S54.348,29.252,54.162,29.047z M27.115,4.887c3.314,0,6.009,2.695,6.009,6.008c0,1.236-0.376,2.386-1.018,3.341l-0.034,0.048"
	"c-0.027,0.04-0.058,0.076-0.086,0.115c-0.115,0.159-0.237,0.312-0.365,0.457c-0.034,0.04-0.07,0.078-0.105,0.117"
	"c-0.127,0.136-0.259,0.266-0.396,0.389c-0.031,0.027-0.061,0.056-0.092,0.083c-0.351,0.301-0.731,0.562-1.138,0.773"
	"c-0.014,0.008-0.028,0.014-0.042,0.021c-0.185,0.095-0.374,0.179-0.567,0.254c-0.042,0.017-0.085,0.031-0.128,0.047"
	"c-0.168,0.062-0.339,0.114-0.513,0.16c-0.054,0.014-0.107,0.029-0.162,0.042c-0.17,0.039-0.343,0.069-0.516,0.095"
	"c-0.056,0.008-0.109,0.019-0.165,0.025c-0.226,0.025-0.453,0.041-0.683,0.041c-0.228,0-0.454-0.015-0.679-0.041"
	"c-0.047-0.005-0.093-0.015-0.141-0.021c-0.182-0.025-0.362-0.057-0.541-0.099c-0.041-0.009-0.081-0.021-0.121-0.031"
	"c-0.188-0.049-0.375-0.104-0.558-0.171c-0.025-0.01-0.052-0.019-0.077-0.028c-0.855-0.326-1.635-0.845-2.27-1.523"
	"c-0.017-0.018-0.032-0.036-0.049-0.055c-0.143-0.155-0.277-0.319-0.403-0.491c-0.031-0.043-0.065-0.085-0.096-0.128l-0.054-0.074"
	"c-0.644-0.957-1.02-2.107-1.02-3.345C21.108,7.582,23.802,4.887,27.115,4.887z M37.877,46.177l-0.533,0.309l0.05,5.023"
	"c0,0.157-0.058,0.259-0.092,0.295l-3.715,0.013c-0.028-0.019-0.105-0.126-0.105-0.308v-3.417l-1.227,0.286"
	"c-3.586,0.834-7.088,0.759-10.471-0.128l-1.254-0.328V51.4c0,0.183-0.078,0.29-0.051,0.296l-3.755,0.013"
	"c-0.028-0.019-0.106-0.126-0.106-0.309l-0.002-4.336l0.181-0.835l-0.599-0.371c-1.983-1.231-3.659-2.749-4.981-4.51l-0.3-0.399H5.5"
	"c-0.607,0-1.102-0.494-1.102-1.103v-8.936c0-0.608,0.494-1.103,1.102-1.103h2.718l0.168-0.793c0.495-2.345,1.527-4.525,3.068-6.482"
	"l0.474-0.603l-2.229-2.988c0.987-0.24,2.279,0.053,3.456,0.795l0.604,0.381l0.556-0.447c1.945-1.563,4.271-2.775,6.762-3.529"
	"c0.022,0.026,0.05,0.049,0.072,0.075c0.152,0.17,0.311,0.335,0.478,0.492c0.042,0.04,0.086,0.078,0.129,0.117"
	"c0.166,0.15,0.337,0.293,0.515,0.429c0.043,0.032,0.085,0.064,0.128,0.096c0.076,0.056,0.149,0.117,0.228,0.17"
	"c-1.312,0.332-2.58,0.797-3.768,1.4c-0.493,0.25-0.688,0.852-0.438,1.344c0.25,0.494,0.852,0.69,1.344,0.439"
	"c2.279-1.158,4.887-1.77,7.538-1.77c2.793,0,5.521,0.675,7.885,1.952c0.151,0.082,0.314,0.12,0.475,0.12"
	"c0.355,0,0.7-0.189,0.881-0.524c0.263-0.486,0.081-1.093-0.404-1.355c-1.386-0.748-2.881-1.308-4.437-1.676"
	"c0.049-0.035,0.096-0.073,0.145-0.108c0.068-0.051,0.137-0.101,0.203-0.152c0.155-0.122,0.304-0.25,0.449-0.383"
	"c0.062-0.056,0.124-0.109,0.184-0.166c0.159-0.155,0.311-0.317,0.457-0.485c0.03-0.034,0.064-0.064,0.094-0.1"
	"c7.873,2.237,13.317,8.751,13.317,16.037C46.549,37.772,43.308,43.037,37.877,46.177z M51.304,29.612"
	"c-0.061,0.076-0.14,0.151-0.242,0.153c-0.178-0.027-0.486-0.106-0.553-0.345c-0.043-0.151,0.018-0.338,0.122-0.428"
	"c0.059-0.028,0.148-0.041,0.263-0.041c0.176,0,0.407,0.03,0.658,0.081C51.501,29.265,51.412,29.472,51.304,29.612z M16.188,28.765"
	"c0,0.919-0.745,1.664-1.664,1.664s-1.664-0.745-1.664-1.664s0.745-1.664,1.664-1.664S16.188,27.846,16.188,28.765z"
    ;

QPainterPath KRPTSceneImageCache::_getPath(uint32_t index, const QSizeF &size) noexcept
{
//    QPainterPath path = SvgPathParser::fromSvgFile("D:/Desktop/icons/0.svg");
//    QPainterPath path = SvgPathParser::fromSvgFile("D:/Desktop/icons/1/Expand/22-Option.svg");



    QPainterPath path = SvgPathParser::parsePath(QString::fromLatin1(kPath2));

//QPainterPath path = SvgPathParser::parsePath(
//    "M5,1 "
//    "A4,4 0 0 1 39,5"
//);

    qDebug() << "bbox" << path.boundingRect();
    return path;
}

#endif