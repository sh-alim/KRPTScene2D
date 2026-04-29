#pragma once

#if 0
#include <QImage>
#include <QPixmap>
#include <QPainterPath>
#include <unordered_map>

class KRPTSceneImageCache
{
public:

using Image = std::shared_ptr<QImage>;
using Index = std::unordered_map<uint32_t, Image>;

public:
    static KRPTSceneImageCache& instance()
    {
        static KRPTSceneImageCache _cache;
        return _cache;
    }

    static Image get(uint32_t index, const QSizeF &size) noexcept
    {
        return instance()._get(index, size);
    }
    static QPainterPath getPath(uint32_t index, const QSizeF &size) noexcept
    {
        return instance()._getPath(index, size);
    }

private:
    Image _get(uint32_t index, const QSizeF &size) noexcept;
    QPainterPath _getPath(uint32_t index, const QSizeF &size) noexcept;

private:
    KRPTSceneImageCache(const KRPTSceneImageCache&) = delete;
    KRPTSceneImageCache& operator=(const KRPTSceneImageCache&) = delete;
    KRPTSceneImageCache(KRPTSceneImageCache&&) = delete;
    KRPTSceneImageCache& operator=(KRPTSceneImageCache&&) = delete;
    KRPTSceneImageCache() noexcept{};
   ~KRPTSceneImageCache() noexcept{};

   Index _index;
};
#endif