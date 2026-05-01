//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTStaticImage.h"
#include <unordered_map>

//########################################################################################################################
//#
//########################################################################################################################

class KRPTImageCache
{
public:
    using Index = std::unordered_map<uint64_t, KRPTStaticImage>;
public:
    static KRPTImageCache& instance()
    {
        static KRPTImageCache _cache;
        return _cache;
    }
private:
    KRPTImageCache(const KRPTImageCache&) = delete;
    KRPTImageCache& operator=(const KRPTImageCache&) = delete;
    KRPTImageCache(KRPTImageCache&&) = delete;
    KRPTImageCache& operator=(KRPTImageCache&&) = delete;
    KRPTImageCache() noexcept{};
   ~KRPTImageCache() noexcept{};

//   Index _index;
};
