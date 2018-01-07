#pragma once

#include <type_traits>
#include <complex>
#include <algorithm>

namespace Engine::Utils {

template <class Enum>
auto constexpr underlying_value(Enum e) noexcept
{
        using Underlying_type = std::underlying_type_t<Enum>;
        return static_cast<Underlying_type>(e);
}

template <class T>
T distance(std::complex<T> p1, std::complex<T> p2) noexcept
{
        return std::abs(p1 - p2);
}

template <class Container, class Predicate>
Container filter(Container const& c, Predicate pred)
{
        Container result;
        std::copy_if(c.cbegin(), c.cend(), std::back_inserter(result), pred);
        return result;
}

}

