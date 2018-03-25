// These two cancerous defines exist so that png_io.hpp works properly.
// See also https://github.com/ignf/gilviewer/issues/8.
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#define BOOST_GIL_USE_CONCEPT_CHECK

#include "boost/gil/gil_all.hpp"
#include "boost/gil/extension/io/png_io.hpp"
#include <iostream>

namespace gil = boost::gil;

template <class SrcView, class MutableDstView, class Point>
void paint_over(SrcView const& src,
                MutableDstView const& dst,
                Point position)
{
        auto const paint_row =
        [&](auto const row)
        {
                auto const src_start = src.row_begin(row);
                auto const src_end = src_start + src.width();
                auto const dst_iter = dst.x_at(position.x, position.y + row);
                std::copy(src_start, src_end, dst_iter);
        };

        for (int row = 0; row < src.height(); ++row)
                paint_row(row);
}

gil::rgba8_image_t read_png(std::string const& filename)
{
        // TODO Using strictly gil::rgba8_image_t here might lead to
        // portability issues if we change our sprite format.
        // I should fix this to use any_image<...>

        gil::rgba8_image_t result;
        gil::png_read_image(filename, result);
        return result;
}

int main()
{
        auto const img = read_png("/home/haskellcurry/test.png");
        gil::rgba8_image_t output(500, 500);
        auto const output_view = gil::view(output);
        gil::rgba8_pixel_t red_pixel(255, 0, 0, 255);
        std::fill(output_view.begin(), output_view.end(), red_pixel);
        gil::point2 position(50, 50);
        paint_over(gil::const_view(img), output_view, position);
        gil::png_write_view("test-out.png", output_view);
}

