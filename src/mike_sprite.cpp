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
        , current_animation_(current_mike_animation(sprite_sheet, mike))
{}

void Mike_sprite::render(Engine::Sdl::Renderer& renderer)
{
        current_animation_.render(renderer,
                                  mike_->position(),
                                  current_flip());
}

void Mike_sprite::update()
{
        current_animation_.update();
}

void Mike_sprite::switch_animation()
{
        current_animation_ = current_mike_animation(*sprite_sheet_, *mike_);
}

Engine::Graphics::Animation current_mike_animation(Engine::Graphics::Sprite_sheet& sprite_sheet,
                                                   Logic::Mike const& mike)
{
        return sprite_sheet.animation(mike.current_sprite_name());
}

Engine::Sdl::Flip Mike_sprite::current_flip() const noexcept
{
        return mike_->is_facing_left()?
                Engine::Sdl::Flip::horizontal : Engine::Sdl::Flip::vertical;
}

}

