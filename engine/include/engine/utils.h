#pragma once

#include <type_traits>
#include <complex>
#include <algorithm>
#include <variant>

namespace Engine::Utils {

template <class Enum>
auto constexpr underlying_value(Enum e) noexcept
{
        using Underlying_type = std::underlying_type_t<Enum>;
        return static_cast<Underlying_type>(e);
}

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

template <class Container, class Predicate>
Container filter(Container const& c, Predicate pred)
{
        Container result;
        std::copy_if(c.cbegin(), c.cend(), std::back_inserter(result), pred);
        return result;
}

}

