#include <utility>
#include "graphics.h"
#include "config_loader.h"

using namespace std::string_literals;

namespace Engine::Graphics {

Frame::Frame(Sdl::Rect source_rect, Sdl::Texture& texture) noexcept
        : source_rect_(source_rect)
        , texture_(&texture)
{}

void Frame::render(Sdl::Renderer& renderer,
                   Complex_number position,
                   double angle,
                   Sdl::Flip flip)
{
        Sdl::Rect const destination_rect = {
                source_rect_.w,
                source_rect_.h,
                static_cast<int>(position.real()),
                static_cast<int>(position.imag())
        };

        Sdl::render_copy(renderer,
                         *texture_,
                         source_rect_,
                         destination_rect,
                         angle,
                         flip);
}

Sprite create_sprite(Sdl::Texture& texture, int frame_count)
{
        auto const sprite_dimensions =
        [](Sdl::Texture& texture, int frame_count)
        {
                auto const [width, height] = Sdl::texture_dimensions(texture);
                return Sdl::Dimensions {
                        width / frame_count,
                        height
                };
        };

        auto const [width, height] = sprite_dimensions(texture, frame_count);
        Sprite sprite;

        for (auto i = 0; i < frame_count; ++i) {
                Sdl::Rect const src {
                        width * i, 0,
                        width, height
                };

                sprite.push(Frame(
                        src,
                        texture
                ));
        }

        return sprite;
}

Animation::Animation(Sprite const& sprite, int frame_delay) noexcept
        : sprite_(&sprite)
        , remaining_frame_delay_(frame_delay)
        , max_frame_delay_(frame_delay)
{}

void Animation::advance() noexcept
{
        --remaining_frame_delay_;
        if (remaining_frame_delay_ == 0) {
                remaining_frame_delay_ = max_frame_delay_;
                advance_sprite_frame();
        }
}

void Animation::render(Sdl::Renderer& renderer,
                       Complex_number position,
                       double angle=0,
                       Sdl::Flip flip)
{
        current_frame_->render(renderer, position, angle, flip);
}

void Animation::advance_frame() noexcept
{
        ++current_frame_;
        if (current_frame_ == sprite_->cend())
                current_frame_ = sprite_->cbegin();
}

AnimatedSprite AnimatedSprite::load(std::string const& base_path
                                    std::string const& image_extension,
                                    std::string const& config_extension)
{
        using namespace std::string_literals;

        auto const sprite_properties =
        [](Config const& config)
        {
                return std::pair(
                        config.value<int>("frame_count"s),
                        config.value<int>("frame_delay"s)
                );
        };

        auto const frames =
        [](Sdl::Texture& texture, int frame_count)
        {
                
        };

        auto const image_path = base_path + "."s + image_extension;
        auto const config_path = path + "."s + config_extension;

        // TODO Rename these in Sdl and Config

        auto const sprite_config = Config::load(config_path);
        auto const [frame_count, frame_delay] = sprite_properties(sprite_config);
        auto texture = Sdl::load_texture(renderer, image_path);

        return Sprite(std::move(texture),
                      frame_count,
                      frame_delay);
}

}

