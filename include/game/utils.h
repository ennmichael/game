#pragma once

#include <variant>
#include <complex>

namespace Game::Utils {

template <class... Lambdas>
struct Lambda_visitor : Lambdas... {
        using Lambdas::operator()...;
};

template <class Variant, class... Lambdas>
decltype(auto) lambda_visit(Variant&& variant, Lambdas&&... lambdas)
{
        return std::visit(Lambda_visitor<Lambdas...> {std::forward<Lambdas>(lambdas)...}, variant);
}

template <class T, class Variant>
bool variant_matches(Variant const& variant)
{
        return lambda_visit(variant,
                [](T const&)
                { return true; },
                [](auto const&)
                { return false; }
        );
}

}

