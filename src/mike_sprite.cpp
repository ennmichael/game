#include "mike_sprite.h"
#include "utils.h"
#include <cassert>
#include <variant>

using namespace std::string_literals;

namespace Game::Graphics {

Mike_sprite::Mike_sprite(Engine::Graphics::Sprite_sheet& sprite_sheet,
                         Logic::Mike const& mike) noexcept
        : sprite_sheet_(&sprite_sheet)
        , mike_(&mike)
{}

void Mike_sprite::render(Engine::Sdl::Renderer& renderer)
{
        auto const flip = current_flip();
        current_animation().render(renderer, mike_->position(), flip);
}

void Mike_sprite::update()
{
        current_animation().update();
        // This won't work
}

Engine::Sdl::Flip Mike_sprite::current_flip() const noexcept
{
        return mike_->is_facing_left()?
                Engine::Sdl::Flip::horizontal : Engine::Sdl::Flip::vertical;
}

}

