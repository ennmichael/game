#pragma once

#include "boost/gil/gil_all.hpp"
#include "boost/property_tree/ptree.hpp"
#include <string>
#include <vector>

namespace Spacker {

using Image = boost::gil::rgba8_image_t;
using ConstView = boost::gil::rgba8c_view_t;
using View = boost::gil::rgba8_view_t;

using Point = boost::gil::point2<int>;

struct Sprite {
        static Sprite load(std::string const& path);

        ConstView const_view() const noexcept;
        View view() noexcept;

        int width() const noexcept;
        int height() const noexcept;

        std::string name;
        Image image;
};

std::vector<Sprite> load_sprites(std::vector<std::string> const& paths);

class SpriteSheet {
public:
        static SpriteSheet pack(std::vector<Sprite> const& sprites);

        void write_to(std::string const& base_path) const;

private:
        struct PositionedSprite : Sprite {
                Point position;
        };

        SpriteSheet(Image image, boost::property_tree::ptree tree) noexcept;

        View view() noexcept;
        ConstView const_view() const noexcept;

        static std::vector<PositionedSprite> position_sprites(std::vector<Sprite> const& sprites);
        static boost::property_tree::ptree property_tree(std::vector<PositionedSprite> const& sprites);
        static Image sheet_image(std::vector<PositionedSprite> const& sprites);
        static int sheet_width(std::vector<PositionedSprite> const& sprites) noexcept;
        static int sheet_height(std::vector<PositionedSprite> const& sprites) noexcept;
        static void paint_sprites(std::vector<PositionedSprite> const& sprites,
                                  View const& image_view);

        Image image_;
        boost::property_tree::ptree tree_;
};

}

