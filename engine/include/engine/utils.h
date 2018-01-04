#pragma once

#include <type_traits>

namespace Engine::Utils {

template <class Enum>
auto underlying_value(Enum e) noexcept
{
        using Underlying_type = std::underlying_type_t<Enum>;
        return static_cast<Underlying_type>(e);
}

}

