//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTImageCache.h"
#include "KRPTImageCacheData.h"
#include <QFile>
#include <QCryptographicHash>

//########################################################################################################################
//#
//########################################################################################################################

KRPTImageCacheKeyType KRPTImageCache::keyImpl(const QByteArray &src) noexcept
{
    QByteArray md5 = QCryptographicHash::hash(src, QCryptographicHash::Md5);
    const uint64_t *p = reinterpret_cast<const uint64_t*>(md5.constData());
    KRPTImageCacheKeyType key;
#ifdef _KRPT_IMAGE_CACHE_KEY_128
    key._l = p[0];
    key._r = p[1];
#else
    key = p[0] ^ p[1];
#endif
    if(_index.find(key) != _index.end())
    {
        key._enable = true;
        return key;
    }
    auto find = _krptImageCacheData.find(src);
    if(find != _krptImageCacheData.end())
    {
        _index.try_emplace(key, find->second, true);
        key._enable = true;
    }else
    if(QFile(src).exists())
    {
        _index.try_emplace(key, src, true);
        key._enable = true;
    }
    return key;
}

void KRPTImageCache::drawImpl(const KRPTImageCacheKeyType &key, QPainter &painter, 
    const QRectF &rect, const QColor &color) noexcept
{
    auto find = _index.find(key);
    if(find == _index.end())return;
    if(!find->second.draw(painter, rect, color))key._enable = false;
}
