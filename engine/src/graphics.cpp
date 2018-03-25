#include "graphics.h"
#include "config_loader.h"
#include "utils.h"
#include <utility>
#include <type_traits>

using namespace std::string_literals;

namespace Engine::Graphics {

Sprite_sheet::Sprite::Sprite(Sprite_sheet& sprite_sheet,
                             Sdl::Rect source) noexcept
        : sprite_sheet_(sprite_sheet)
        , source_(source)
{}

void Sprite_sheet::Sprite::render(Sdl::Renderer& renderer,
                                  Complex_number position,
                                  Sdl::Flip flip)
{
        auto const dst = Sdl::make_rect(Sdl::Dimensions(source_), position);
        sprite_sheet_.render(renderer, source_, dst, flip);
}

Sprite_sheet::Sprite_sheet(Sdl::Unique_texture texture)
        : texture_(std::move(texture))
{}

Sprite_sheet::Animation::Animation(Sprite_sheet& sprite_sheet,
                                        Sdl::Rect first_frame_source,
                                        Duration::Milliseconds frame_delay,
                                        int frame_count) noexcept
        : sprite_sheet_(sprite_sheet)
        , source_(first_frame_source)
        , timer_(frame_delay)
        , frame_count_(frame_count)
{}

void Sprite_sheet::Animation::update() noexcept
{
        if (timer_.ready())
                ++current_frame_;

        if (current_frame_ == frame_count_)
                current_frame_ = 0;
}

void Sprite_sheet::Animation::render(Sdl::Renderer& renderer,
                                     Complex_number position,
                                     Sdl::Flip flip)
{
        auto const dst = make_rect(Sdl::Dimensions(source_), position);
        sprite_sheet_.render(renderer, source_, dst, flip);
}

Sprite Sprite_sheet::sprite(Sdl::Rect source) noexcept
{
        return Sprite(*this, source);
}

void Sprite_sheet::render(Sdl::Renderer& renderer,
                          Sdl::Rect source,
                          Sdl::Rect destination,
                          Sdl::Flip flip)
{
        Sdl::render_copy(renderer,
                         *texture_,
                         source,
                         destination,
                         0,
                         flip);
}

}

