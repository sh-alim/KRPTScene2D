//######################################################################################################################################################
//
//######################################################################################################################################################

#pragma once

//######################################################################################################################################################
//
//######################################################################################################################################################

#include <type_traits>

//######################################################################################################################################################
//
//######################################################################################################################################################

template<typename T>
class KRPTFlag
{
public:
    static_assert(std::is_enum_v<T>, "T must be enum");
    using TFlag = std::underlying_type_t<T>;

    KRPTFlag()            noexcept = default;
    KRPTFlag(T other)     noexcept : _flag(static_cast<TFlag>(other)) {}
    KRPTFlag(TFlag other) noexcept : _flag(other)                     {}

    TFlag flag() const noexcept {return _flag;}

    template<typename... Args> void up  (Args... args) noexcept {_flag |=  combine(args...);}
    template<typename... Args> void down(Args... args) noexcept {_flag &= ~combine(args...);}
    template<typename... Args> constexpr bool any(Args... args) const noexcept 
    {
        return (_flag & combine(args...)) != 0;
    }
    template<typename... Args> constexpr bool all(Args... args) const noexcept 
    {
        if constexpr (sizeof...(args) == 0)return false;
        TFlag mask = combine(args...);
        return (_flag & mask) == mask;
    }
    KRPTFlag diff(KRPTFlag other) const noexcept
    {
        return KRPTFlag(static_cast<TFlag>(_flag ^ other._flag));
    }
    template<typename... Args> bool operator()(Args... args) const noexcept 
    {
        return all(args...);
    }
    bool operator[](T arg) const noexcept 
    {
        return (_flag & static_cast<TFlag>(arg)) != 0;
    }
    void operator += (T arg) noexcept {up  (arg);}
    void operator -= (T arg) noexcept {down(arg);}

    friend bool operator == (KRPTFlag lhs, KRPTFlag rhs) noexcept 
    {
        return lhs._flag == rhs._flag;
    }
    friend bool operator != (KRPTFlag lhs, KRPTFlag rhs) noexcept 
    {
        return lhs._flag != rhs._flag;
    }
    friend bool operator < (KRPTFlag lhs, KRPTFlag rhs) noexcept 
    {
        return lhs._flag < rhs._flag;
    }

    KRPTFlag& operator=(TFlag mask) noexcept 
    {
        _flag = mask;
        return *this;
    }
private:
    template<typename... Args>
    static constexpr TFlag combine(Args... args) noexcept 
    {
        return (0 | ... | static_cast<TFlag>(args));
    }
private:
    TFlag _flag = 0;
};

template<typename T, typename = std::enable_if_t<std::is_enum<T>::value && !std::is_convertible<T, int>::value>>
constexpr T operator | (T lhs, T rhs) noexcept
{
    using TFlag = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<TFlag>(lhs) | static_cast<TFlag>(rhs));
}