/**
 * These two cancerous defines exist so that png_io.hpp works properly.
 * See also https://github.com/ignf/gilviewer/issues/8.
 */
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#define BOOST_GIL_USE_CONCEPT_CHECK

#include "spacker.h"
#include "utils.h"
#include "boost/gil/extension/io/png_io.hpp"
#include "boost/range/adaptors.hpp"
#include "boost/range/numeric.hpp"
#include "boost/range/algorithm/max_element.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <fstream>
#include <utility>

namespace Spacker {

namespace {

using Point = boost::gil::point2<int>;

template <class Images>
auto const views(Images const& images)
{
        return images
             | boost::adaptors::transformed([](auto const& image)
                                            { return boost::gil::const_view(image); });
}

template <class DstView, class ImmutableSrcView>
void paint_over(DstView const& dst,
                ImmutableSrcView const& src,
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

}

SpriteSheet SpriteSheet::pack(std::vector<Image> const& sprites)
{
        struct PositionedSprite {
                ConstView view;
                Point position;
        };

        auto const sprite_views = views(sprites);

        auto const positioned_sprites =
        [&]()
        {
                std::vector<PositionedSprite> result;
                int x = 0;

                for (auto const& sprite_view : sprite_views) {
                        result.push_back(PositionedSprite {
                                sprite_view,
                                Point(x, 0)});
                        x += sprite_view.width();
                }

                return result;
        }();

        auto const sprite_sheet_json =
        [&]()
        {
                return "";
        }();

        auto sprite_sheet_image =
        [&]()
        {
                auto const width = boost::accumulate(positioned_sprites,
                                                     0,
                                                     [](int result, auto const& sprite)
                                                     { return result + sprite.view.width(); });

                auto const tallest_sprite = *boost::max_element(positioned_sprites,
                                                                [](auto const& s1, auto const& s2)
                                                                { return s1.view.height() < s2.view.height(); });
                
                return Image(width, tallest_sprite.view.height());
        }();

        auto const sprite_sheet_view = boost::gil::view(sprite_sheet_image);

        for (auto const& [sprite_view, position] : positioned_sprites)
                paint_over(sprite_sheet_view, sprite_view, position);

        return SpriteSheet(
                std::move(sprite_sheet_image),
                std::move(sprite_sheet_json));
}

void SpriteSheet::write_to(std::string const& base_path) const
{
        auto const write_text =
        [](auto const& path, auto const& text)
        {
                std::ofstream f(path);
                f << text;

                /* TODO Maybe have some error handling? */
        };

        using namespace std::string_literals;

        auto const image_path = base_path + ".png"s;
        auto const json_path = base_path + ".json"s;

        boost::gil::png_write_view(image_path, const_image_view());
        write_text(json_path, json_);
}

SpriteSheet::SpriteSheet(Image image, std::string json) noexcept
        : image_(std::move(image))
        , json_(std::move(json))
{}

View SpriteSheet::image_view() noexcept
{ return boost::gil::view(image_); }

ConstView SpriteSheet::const_image_view() const noexcept
{ return boost::gil::const_view(image_); }

Image load_image(std::string const& path)
{
        Image image;
        boost::gil::png_read_image(path, image);
        return image;
}

std::vector<Image> load_images(std::vector<char const*> const& paths)
{
        return Utils::range_to_vector(paths | boost::adaptors::transformed(load_image));
}

}

