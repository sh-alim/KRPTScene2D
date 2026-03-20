//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include <type_traits>
#include <cstdint>
#include <utility>

//########################################################################################################################
//#
//########################################################################################################################

template<typename T>
class KRPTFlag
{
public:
    static_assert(std::is_enum_v<T>, "type must be enum or enum class");
    using TFlag = std::underlying_type_t<T>;
    KRPTFlag() : _flag(0){}
    KRPTFlag(T flag) : _flag(static_cast<TFlag>(flag)){}
    TFlag flag() const noexcept {return _flag;}
    template<typename ... Args> constexpr inline void up(Args&& ... args) noexcept
    {_flag |= _or(std::forward<Args>(args)...);}
    template<typename ... Args> constexpr inline void down(Args&& ... args) noexcept
    {_flag &= ~_or(std::forward<Args>(args)...);}
    template<typename ... Args> constexpr inline bool upped(Args&& ... args) const noexcept
    {return _flag & _or(std::forward<Args>(args)...);}
    template<typename ... Args> constexpr inline bool uppedAll(Args&& ... args) const noexcept
    {return _flag > 0 && (_flag & _or(std::forward<Args>(args)...)) == _flag;}
    template<typename T1> constexpr inline void operator += (T1 arg) noexcept
    {up(std::forward<T1>(arg));}
    inline void operator += (KRPTFlag arg) noexcept {_flag |= arg._flag;}
    template<typename T1> constexpr inline void operator -= (T1 arg) noexcept
    {down(std::forward<T1>(arg));}
    template<typename ... Args> constexpr inline bool operator()(Args&& ... args) const noexcept
    {return uppedAll(std::forward<Args>(args)...);}
    template<typename ... Args> constexpr inline bool operator[](Args&& ... args) const noexcept
    {return upped(std::forward<Args>(args)...);}
    friend constexpr inline bool operator == (KRPTFlag lhs, KRPTFlag rhs) noexcept
    {return lhs._flag == rhs._flag;}
    friend constexpr inline bool operator < (KRPTFlag lhs, KRPTFlag rhs) noexcept
    {return lhs._flag < rhs._flag;}
private:
    template<typename ... Args> constexpr inline TFlag _or(Args&& ... args) const noexcept
    {
        static_assert((std::is_same_v<T, std::remove_reference_t<Args&&>>&& ...), 
            "argument type must be flag enum"); 
        static_assert(((sizeof(std::remove_reference_t<Args&&>) <= sizeof(T))&& ...), 
            "argument type out of range"); 
        return (0 | ... | (static_cast<TFlag>(args)));
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