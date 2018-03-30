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
#include "boost/property_tree/json_parser.hpp"
#include <utility>

namespace Spacker {

namespace {

using Point = boost::gil::point2<int>;

template <class NamedImages>
auto const views(NamedImages const& images)
{
        return images
             | boost::adaptors::transformed([](auto const& image)
                                            { return image.const_view(); });
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

Image read_png(std::string const& path)
{
        Image image;
        boost::gil::png_read_image(path, image);
        return image;
}

std::string_view string_view_after(std::string_view view, char c)
{
    auto const i = view.find_last_of(c);
    return (i == std::string_view::npos)?
        view :
        view.substr(i+1);
}

std::string_view string_view_until(std::string_view view, char c)
{
    auto const i = view.find_last_of(c);
    return (i == std::string_view::npos)?
        view :
        view.substr(0, i);
}

std::string_view base_name(std::string_view path)
{
    path = string_view_after(path, '/');
    path = string_view_until(path, '.');
    return path;
}

} // Close unnamed namespace

Sprite Sprite::load(std::string const& path)
{
        return Sprite {
                std::string(base_name(path)),
                read_png(path)
        };
}

ConstView Sprite::const_view() const noexcept
{ return boost::gil::const_view(image); }

View Sprite::view() noexcept
{ return boost::gil::view(image); }

int Sprite::width() const noexcept
{ return const_view().width(); }

int Sprite::height() const noexcept
{ return const_view().height(); }

std::vector<Sprite> load_sprites(std::vector<std::string> const& paths)
{
        return Utils::range_to_vector(paths | boost::adaptors::transformed(Sprite::load));
}

SpriteSheet SpriteSheet::pack(std::vector<Sprite> const& sprites)
{
        struct PositionedSprite : Sprite {
                Point position;
        };

        // TODO We probably want to switch from using IIFEs to private member functions

        auto const positioned_sprites =
        [&]
        {
                std::vector<PositionedSprite> result;
                int x = 0;

                for (auto const& sprite : sprites) {
                        result.push_back(PositionedSprite {
                                sprite,
                                Point(x, 0)});
                        x += sprite.const_view().width();
                }

                return result;
        }();

        auto const sprite_sheet_tree =
        [&]
        {
                auto const sprite_properties =
                [](auto const& sprite)
                {
                        boost::property_tree::ptree properties;
                        properties.put("x", sprite.position.x);
                        properties.put("y", sprite.position.y);
                        properties.put("width", sprite.width());
                        properties.put("height", sprite.height());
                        return properties;
                };
                
                boost::property_tree::ptree tree;

                for (auto const& sprite : positioned_sprites)
                        tree.add_child(sprite.name, sprite_properties(sprite));

                return tree;
        }();

        auto sprite_sheet_image =
        [&]
        {
                auto const width = boost::accumulate(positioned_sprites,
                                                     0,
                                                     [](int result, auto const& sprite)
                                                     { return result + sprite.width(); });

                auto const tallest_sprite = *boost::max_element(positioned_sprites,
                                                                [](auto const& s1, auto const& s2)
                                                                { return s1.height() < s2.height(); });
                
                return Image(width, tallest_sprite.height());
        }();

        auto const sprite_sheet_view = boost::gil::view(sprite_sheet_image);

        for (auto const& sprite : positioned_sprites)
                paint_over(sprite_sheet_view, sprite.const_view(), sprite.position);

        return SpriteSheet(
                std::move(sprite_sheet_image),
                std::move(sprite_sheet_tree));
}

void SpriteSheet::write_to(std::string const& base_path) const
{
        using namespace std::string_literals;

        auto const image_path = base_path + ".png"s;
        auto const json_path = base_path + ".json"s;

        boost::gil::png_write_view(image_path, const_view());
        boost::property_tree::write_json(json_path, tree_);
}

SpriteSheet::SpriteSheet(Image image, boost::property_tree::ptree tree) noexcept
        : image_(std::move(image))
        , tree_(std::move(tree))
{}

View SpriteSheet::view() noexcept
{ return boost::gil::view(image_); }

ConstView SpriteSheet::const_view() const noexcept
{ return boost::gil::const_view(image_); }

}
