#include "graphics.h"
#include "config_loader.h"
#include "utils.h"
#include <utility>

using namespace std::string_literals;

namespace Engine::Graphics {

Sprite::Sprite(Sdl::Unique_texture texture, int frame_count)
        : texture_(std::move(texture))
        , frame_width_(Sdl::texture_width(*texture_) / frame_count)
        , frame_height_(Sdl::texture_height(*texture_))
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

int Sprite::frame_width() const noexcept
{
        return frame_width_;
}

int Sprite::frame_height() const noexcept
{
        return frame_height_;
}

Sdl::Rect Sprite::source_rect(int frame_num) const noexcept
{
        return { 
                frame_num * frame_width_,
                0,
                frame_width_,
                frame_height_
        };
}

Sdl::Rect Sprite::destination_rect(Complex_number position) const noexcept
{
        return { 
                static_cast<int>(position.real()),
                static_cast<int>(position.imag()),
                frame_width_,
                frame_height_
        };
}

Animation::Animation(int frame_count, Duration::Frames frame_delay) noexcept
        : max_frame_delay_(frame_delay)
        , frame_count_(frame_count)
{}

void Animation::update() noexcept
{
        --remaining_frame_delay_;
        if (remaining_frame_delay_ == Duration::Frames {0}) {
                remaining_frame_delay_ = max_frame_delay_;
                update_current_frame();
        }
}

int Animation::current_frame() const noexcept
{
        return current_frame_;
}

Duration::Frames Animation::duration() const noexcept
{
        return max_frame_delay_ * Duration::Frames {frame_count_};
}

void Animation::update_current_frame() noexcept
{
        ++current_frame_;
        if (current_frame_ == frame_count_)
                current_frame_ = 0;
}

Animated_sprite::Animated_sprite(Sprite sprite, Animation const& animation) noexcept
        : sprite_(std::move(sprite))
        , animation_(animation)
{}

Animated_sprite Animated_sprite::load(Sdl::Renderer& renderer,
                                      std::string const& base_path,
                                      std::string const& image_extension,
                                      std::string const& config_extension)
{
        using namespace std::string_literals;

        auto const image_path = base_path + "."s + image_extension;
        auto const config_path = base_path + "."s + config_extension;

        Config const sprite_config(config_path);
        auto const frame_count = sprite_config.value<int>("frame_count"s);
        auto const frame_delay = sprite_config.value<int>("frame_delay"s);

        return Animated_sprite(
                Sprite(Sdl::load_texture(renderer, image_path), frame_count),
                Animation(frame_count, Duration::Frames {frame_delay})
        );
}

Duration::Frames Animated_sprite::animation_duration() const noexcept
{
        return animation_.duration();
}

void Animated_sprite::update()
{
        animation_.update();
}

void Animated_sprite::render(Sdl::Renderer& renderer,
                             Complex_number position,
                             double angle,
                             Sdl::Flip flip)
{
        sprite_.render_frame(renderer,
                             position,
                             animation_.current_frame(),
                             angle,
                             flip);
}

void load_animated_sprite(Animated_sprites& sprites, Sdl::Renderer& renderer, std::string const& path)
{
        auto const basename =
        [](std::string_view path) noexcept
        {

                auto const i = path.rfind('/');
                return (i == std::string_view::npos) ? path : path.substr(i+1);
        };

        sprites.insert_or_assign(std::string(basename(path)),
                                 Animated_sprite::load(renderer, path));
}

Animations_durations animations_durations(Animated_sprites const& sprites)
{
        Animations_durations durations;

        for (auto const& [name, spr] : sprites) {
                durations.insert_or_assign(name, spr.animation_duration());
        }

        return durations;
}

int Animated_sprite::frame_width() const noexcept
{
        return sprite_.frame_width();
}

int Animated_sprite::frame_height() const noexcept
{
        return sprite_.frame_height();
}

}

