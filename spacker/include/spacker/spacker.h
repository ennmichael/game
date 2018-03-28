#pragma once

#include "boost/gil/gil_all.hpp"
#include <string>
#include <vector>

namespace Spacker {

struct PositionedView;

using Image = boost::gil::rgba8_image_t;
using ConstView = boost::gil::rgba8c_view_t;
using View = boost::gil::rgba8_view_t;

// TODO Extend this to have a constructor that takes a std::vector of images,
// and scrap the pack_sprites function
class SpriteSheet {
public:
        static SpriteSheet pack(std::vector<Image> const& sprites);

        void write_to(std::string const& base_path) const;

private:
        SpriteSheet(Image image, std::string json) noexcept;

        View image_view() noexcept;
        ConstView const_image_view() const noexcept;

        Image image_;
        std::string json_;
};

/**
 * Currently, image loading only works with PNG images with
 * RGBA channels with 8 bits of depth.
 */
Image load_image(std::string const& path);
std::vector<Image> load_images(std::vector<char const*> const& paths);

SpriteSheet pack_sprites(std::vector<Image> const& sprites);

}

