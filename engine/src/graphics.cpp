#include <utility>
#include "graphics.h"
#include "config_loader.h"
#include "utility.h"

using namespace std::string_literals;

namespace Engine::Graphics {

Sprite Sprite::load_from_file(Sdl::Renderer const& renderer, 
                              std::string const& path)
{
        struct Sprite_config {
                static Sprite_config load_from_file(std::string const& path)
                {
                        return Sprite_config(load_config_file(path));
                }

                explicit Sprite_config(Config const& config)
                        : frame_count(Utility::expect_variant<int const>(config.at("frame_count"s)))
                        , frame_delay(Utility::expect_variant<int const>(config.at("frame_delay"s)))
                {}

                int frame_count;
                int frame_delay;
        };

        auto const image_path = path + ".png"s;
        auto const config_path = path + ".sheet.config"s;

        auto const sprite_config = Sprite_config::load_from_file(config_path);
        auto texture = Sdl::load_texture_from_file(renderer, image_path);

        return Sprite(std::move(texture),
                      sprite_config.frame_count,
                      sprite_config.frame_delay);
}

Sprite::Sprite(Sdl::Texture texture, int frame_count, int frame_delay) noexcept
        : texture_(std::move(texture))
        , frame_count_(frame_count)
        , frame_delay_(frame_delay)
{
        auto const [texture_width, texture_height] = Sdl::texture_dimensions(texture_);
        frame_width_ = texture_width / frame_count_;
        frame_height_ = texture_height;
}

void Sprite::frame_advance() noexcept
{
        ++current_frame_;
        if (current_frame_ == frame_count_)
                current_frame_ = 0;
}

int Sprite::frame_delay() const noexcept
{
        return frame_delay_;
}

Animation Sprite::animation() noexcept
{
        return Animation(*this);
}

void Sprite::render(Sdl::Renderer const& renderer, 
                    Engine::Complex_number position) noexcept
{
        auto const src = source_rect();
        auto const dst = destination_rect(position);
        Sdl::render_copy(renderer, texture_, src, dst);
}

Sdl::Rect Sprite::source_rect() const noexcept
{
        return {
                current_frame_ * frame_width_,
                0,
                frame_width_,
                frame_height_
        };
}

Sdl::Rect Sprite::destination_rect(Engine::Complex_number position) const noexcept
{
        return {
                static_cast<int>(position.real()),
                static_cast<int>(position.imag()),
                frame_width_,
                frame_height_
        };
}

Animation::Animation(Sprite& sprite) noexcept
        : sprite_(&sprite)
        , remaining_frame_delay_(sprite.frame_delay())
{}

void Animation::frame_advance() noexcept
{
        --remaining_frame_delay_;
        if (remaining_frame_delay_ == 0) {
                remaining_frame_delay_ = sprite_->frame_delay();
                sprite_->frame_advance();
        }
}

}

