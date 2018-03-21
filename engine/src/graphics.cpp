#include "graphics.h"
#include "config_loader.h"
#include "utils.h"
#include <utility>
#include <type_traits>

using namespace std::string_literals;

namespace Engine::Graphics {

Sprite_sheet Sprite_sheet::load(Sdl::Renderer& renderer,
                                std::string const& image_path,
                                std::string const& config_path)
{
        Config const config(config_path);

        auto const frame_delay = config.value<Duration::Milliseconds>("frame_delay"s);
        auto const frame_count = config.value<int>("frame_count"s);

        return Sprite_sheet(Sdl::load_texture(renderer, image_path),
                            frame_delay,
                            frame_count);
}

Sprite_sheet::Sprite_sheet(Sdl::Unique_texture texture,
                           Duration::Milliseconds frame_delay,
                           int frame_count)
        : texture_(std::move(texture))
        , timer_(frame_delay)
        , frame_count_(frame_count)
{}

void Sprite_sheet::update() noexcept
{
        if (timer_.ready())
                ++current_frame_;

        if (current_frame_ == frame_count_)
                current_frame_ = 0;
}

void Sprite_sheet::render_current_frame(Engine::Sdl::Renderer& renderer,
                                             Engine::Complex_number position,
                                             Engine::Sdl::Flip flip)
{
        Sdl::render_copy(renderer,
                         *texture_,
                         source_rect(),
                         destination_rect(position),
                         0,
                         flip);
}

Sdl::Rect Sprite_sheet::source_rect() const noexcept
{
        return {
                current_frame_ * frame_width_, 0,
                frame_width_, frame_height_
        };
}

Sdl::Rect Sprite_sheet::destination_rect(Engine::Complex_number position) const noexcept
{
        return {
                static_cast<int>(position.real()),
                static_cast<int>(position.imag()),
                frame_width_,
                frame_height_
        };
}

}

