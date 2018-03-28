#pragma once

#include <vector>

namespace Spacker::Utils {

template <class Range>
auto range_to_vector(Range const& range)
{
        // TODO Why can't we use the deduction guide here?
        return std::vector<typename Range::value_type>(range.begin(), range.end());
}

}

