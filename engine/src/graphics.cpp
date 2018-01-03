#include <utility>
#include "graphics.h"
#include "config_loader.h"

using namespace std::string_literals;

namespace Engine::Graphics {

Sprite::Sprite(Sdl::Texture& texture, int frame_count)
        : texture_(&texture)
        , frame_count_(frame_count)
{}

void Sprite::render_frame(Sdl::Renderer& renderer,
                          Complex_number position,
                          int frame_num,
                          double angle,
                          Sdl::Flip flip)
{
        auto const src = source_rect(frame_num);
        auto const dst = destination_rect(position);

        Sdl::render_copy(renderer,
                         *texture_,
                         src,
                         dst,
                         angle,
                         flip);
}

int Sprite::frame_count() const noexcept
{
        return frame_count_;
}

Sdl::Rect Sprite::source_rect(int frame_num) const noexcept
{
        return {
                frame_width_,
                frame_height_,
                frame_num * frame_width_,
                0
        };
}

Sdl::Rect Sprite::destination_rect(Complex_number position) const noexcept
{
        return {
                frame_width_,
                frame_height_,
                static_cast<int>(position.real()),
                static_cast<int>(position.imag())
        };
}

Animation::Animation(Sprite& sprite, int frame_delay) noexcept
        : sprite_(&sprite)
        , remaining_frame_delay_(frame_delay)
        , max_frame_delay_(frame_delay)
{}

void Animation::advance() noexcept
{
        --remaining_frame_delay_;
        if (remaining_frame_delay_ == 0) {
                remaining_frame_delay_ = max_frame_delay_;
                advance_frame_num();
        }
}

void Animation::render(Sdl::Renderer& renderer,
                       Complex_number position,
                       double angle,
                       Sdl::Flip flip)
{
        sprite_->render_frame(renderer, position, frame_num_, angle, flip);
}

void Animation::advance_frame_num() noexcept
{
        ++frame_num_;
        if (frame_num_ == sprite_->frame_count())
                frame_num_ = 0;
}

Animated_sprite Animated_sprite::load(Sdl::Renderer& renderer,
                                      std::string const& base_path,
                                      std::string const& image_extension,
                                      std::string const& config_extension)
{
        using namespace std::string_literals;

        auto const image_path = base_path + "."s + image_extension;
        auto const config_path = base_path + "."s + config_extension;

        // TODO Rename these in Sdl and Config

        auto const sprite_config = Config::load(config_path);
        auto const frame_count = sprite_config.value<int>("frame_count"s);
        auto const frame_delay = sprite_config.value<int>("frame_delay"s);

        auto texture = Sdl::load_texture(renderer, image_path);

        Sprite sprite(texture, frame_count);
        Animation animation(sprite, frame_delay);

        return {sprite, animation};
}

}

