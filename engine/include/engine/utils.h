#pragma once

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <type_traits>
#include <complex>
#include <vector>

namespace Engine::Utils {

template <class Enum>
auto constexpr underlying_value(Enum e) noexcept
{
        using Underlying_type = std::underlying_type_t<Enum>;
        return static_cast<Underlying_type>(e);
}

template <class String>
boost::property_tree::ptree read_json_tree(const String& path)
{
        boost::property_tree::ptree result;
        boost::property_tree::read_json(path, result);
        return result;
}

template <class Range>
auto to_vector(Range const& range)
{
        return std::vector<typename Range::value_type>(range.begin(), range.end());;
}

}

