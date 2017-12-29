#pragma once

#include <variant>

namespace Engine::Graphics::Utility {

class Unexpected_variant : public std::exception {};

template <class T>
struct Expect_variant_visitor {
        T& operator()(T& value) const noexcept
        {
                return value;
        }

        template <class Otherwise>
        T& operator()(Otherwise const&) const
        {
                throw Unexpected_variant();
        }
};

template <class T, class Variant>
T& expect_variant(Variant& variant)
{
        return std::visit(Expect_variant_visitor<T>(), variant);
}

}

