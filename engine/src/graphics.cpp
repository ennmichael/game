#include <utility>
#include "graphics.h"

namespace Engine::Graphics {

Sprite Sprite::load_from_file(Sdl::Renderer const& renderer, 
                              std::string const& path,
                              int frame_count)
{
        auto texture = Sdl::load_texture_from_file(renderer, path);
        return Sprite(std::move(texture), frame_count);
}

Sprite::Sprite(Sdl::Texture texture, int frame_count) noexcept
        : texture_(std::move(texture))
        , frame_count_(frame_count)
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

Animation::Animation(Sprite& sprite, int frame_delay) noexcept
        : sprite_(&sprite)
        , max_frame_delay_(frame_delay)
        , remaining_frame_delay_(frame_delay)
{}

void Animation::frame_advance() noexcept
{
        --remaining_frame_delay_;
        if (remaining_frame_delay_ == 0) {
                remaining_frame_delay_ = max_frame_delay_;
                sprite_->frame_advance();
        }
}

}

