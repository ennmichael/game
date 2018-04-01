#include "mike_sprite.h"
#include "utils.h"
#include <cassert>
#include <variant>

using namespace std::string_literals;

namespace Game::Graphics {

Mike_animations::Mike_animations(Gameplay::Mike const& mike,
                                 Engine::Graphics::Animations& animations) noexcept
        : mike_(&mike)
        , animations_(&animations)
{}

void Mike_animations::render_current_animation(Engine::Sdl::Renderer& renderer)
{
        current_animation().render(renderer,
                                   mike_->position(),
                                   current_flip());
}

void Mike_animations::update_current_animation() noexcept
{
        current_animation().update();
}

Engine::Graphics::Animation& Mike_animations::current_animation() noexcept
{
        return animations_->at(mike_->current_sprite_name());
}

Engine::Sdl::Flip Mike_animations::current_flip() const noexcept
{
        return mike_->is_facing_left()?
                Engine::Sdl::Flip::horizontal : Engine::Sdl::Flip::none;
}

}

