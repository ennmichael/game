#pragma once

#include "utils.h"
#include <complex>

namespace Engine {

using Complex_number = std::complex<double>;

template <class T>
std::complex<T> position(std::complex<T> pos) noexcept
{
        return pos;
}

template <class T>
auto position(T const& obj) noexcept -> decltype(obj.position)
{
        return obj.position;
}

template <class T>
auto position(T const& obj) noexcept(noexcept(obj.position())) -> decltype(obj.position())
{
        return obj.position();
}

template <class T, class U>
auto distance(T const& obj1, U const& obj2) noexcept
{
        return std::abs(position(obj1) - position(obj2));
}

namespace Duration {

enum class Milliseconds : unsigned long long {};

auto constexpr operator==(Milliseconds x, Milliseconds y) noexcept
{
        return Utils::underlying_value(x) == Utils::underlying_value(y);
}

auto constexpr operator>=(Milliseconds x, Milliseconds y) noexcept
{
        return Utils::underlying_value(x) >= Utils::underlying_value(y);
}

auto constexpr operator+(Milliseconds x, Milliseconds y) noexcept
{
        return Milliseconds {
                Utils::underlying_value(x) + Utils::underlying_value(y)
        };
}

enum class Frames : int {};

auto constexpr operator+(Frames x, Frames y) noexcept
{
        return Frames {
                Utils::underlying_value(x) + Utils::underlying_value(y)
        };
}

auto constexpr operator-(Frames x, Frames y) noexcept
{
        return Frames {
                Utils::underlying_value(x) - Utils::underlying_value(y)
        };
}

auto constexpr operator*(Frames x, Frames y) noexcept
{
        return Frames { 
                Utils::underlying_value(x) * Utils::underlying_value(y)
        };
}

auto constexpr& operator--(Frames& x)
{
        x = Frames {Utils::underlying_value(x) - 1};
        return x;
}

auto constexpr operator--(Frames& x, int)
{
        Frames const temp = x;
        --x;
        return temp;
}

bool constexpr operator==(Frames x, Frames y)
{
        return Utils::underlying_value(x) == Utils::underlying_value(y);
}

auto constexpr operator>(Frames x, Frames y) noexcept
{
        return Utils::underlying_value(x) > Utils::underlying_value(y);
}

auto constexpr operator<(Frames x, Frames y) noexcept
{
        return Utils::underlying_value(x) < Utils::underlying_value(y);
}

auto constexpr operator<=(Frames x, Frames y) noexcept
{
        return x < y || x == y;
}

auto constexpr operator>=(Frames x, Frames y) noexcept
{
        return x > y || x == y;
}

} // Close namespace Duration

}

