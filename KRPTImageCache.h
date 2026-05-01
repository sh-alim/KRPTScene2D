//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTStaticImage.h"
#include <unordered_map>

#define _KRPT_IMAGE_CACHE_KEY_128

//########################################################################################################################
//#
//########################################################################################################################

#ifdef _KRPT_IMAGE_CACHE_KEY_128
class KRPTImageCacheKey
{
friend class KRPTImageCache;
public:
    KRPTImageCacheKey()                                   noexcept : _l(0), _r(0), _enable(false) {}
    bool operator == (const KRPTImageCacheKey &key) const noexcept {return _l == key._l && _r == key._r;}
    bool enable      ()                             const noexcept {return _enable;}
private:
    struct KRPTImageCacheKeyHash
    {
        size_t operator()(const KRPTImageCacheKey &k) const noexcept
        {
            uint64_t h = k._l;
            h ^= k._r + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
            return size_t(h);
        }
    };
    uint64_t     _l     ;
    uint64_t     _r     ;
    mutable bool _enable;
};
using KRPTImageCacheKeyType = KRPTImageCacheKey;
#else
using KRPTImageCacheKeyType = uint64_t;
#endif

//########################################################################################################################
//#
//########################################################################################################################

class KRPTImageCache
{
public:
#ifdef _KRPT_IMAGE_CACHE_KEY_128
    using Index = std::unordered_map<KRPTImageCacheKey, KRPTStaticImage, KRPTImageCacheKey::KRPTImageCacheKeyHash>;
#else
    using Index = std::unordered_map<KRPTImageCacheKeyType, KRPTStaticImage>;
#endif
public:
    static KRPTImageCacheKeyType key(const QByteArray &src)                                 noexcept
    {
        return instance().keyImpl(src);
    }
    static void draw(const KRPTImageCacheKeyType &key, QPainter &painter, 
                     const QRectF &rect, const QColor &color)                               noexcept
    {
        instance().drawImpl(key, painter, rect, color);
    }
private:
    KRPTImageCacheKeyType keyImpl (const QByteArray &src)                                   noexcept;
    void                  drawImpl(const KRPTImageCacheKeyType &key, QPainter &painter, 
                                   const QRectF &rect, const QColor &color)                 noexcept;
private:
    KRPTImageCache()                                                                        noexcept{};
   ~KRPTImageCache()                                                                        noexcept{};
    KRPTImageCache(const KRPTImageCache&)            = delete;
    KRPTImageCache(KRPTImageCache&&)                 = delete;
    KRPTImageCache& operator=(const KRPTImageCache&) = delete;
    KRPTImageCache& operator=(KRPTImageCache&&)      = delete;
private:
    static KRPTImageCache& instance()
    {
        static KRPTImageCache _cache;
        return _cache;
    }
private:
   Index _index;
};
