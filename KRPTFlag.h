//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

template<typename T>
class KRPTFlag
{
public:
    static_assert(std::is_enum_v<T>, "type must be enum or enum class");
    using TFlag = std::conditional_t<sizeof(T) == 1, uint8_t, 
                  std::conditional_t<sizeof(T) == 2, uint16_t, 
                  std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>>>;
    KRPTFlag() : _flag(0){}
    KRPTFlag(T flag) : _flag(static_cast<TFlag>(flag)){}
    TFlag flag() const noexcept {return _flag;}
    template<typename ... Args> inline void up(Args&& ... args) noexcept
    {
        _flag |= _or(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline void down(Args&& ... args) noexcept
    {
        _flag &= ~_or(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline bool upped(Args&& ... args) const noexcept
    {
        return _flag & _or(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline bool uppedAll(Args&& ... args) const noexcept
    {
        return _flag > 0 && (_flag & _or(std::forward<Args>(args)...)) == _flag;
    }
    template<typename T> inline void operator += (T arg) noexcept
    {
        up(std::forward<T>(arg));
    }
    template<typename T> inline void operator -= (T arg) noexcept
    {
        down(std::forward<T>(arg));
    }
    template<typename ... Args> inline bool operator()(Args&& ... args) const noexcept
    {
        return uppedAll(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline bool operator[](Args&& ... args) const noexcept
    {
        return upped(std::forward<Args>(args)...);
    }
private:
    template<typename ... Args> inline TFlag _or(Args&& ... args) const noexcept
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

