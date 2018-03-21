#include "mike_sprite.h"
#include "utils.h"
#include <cassert>
#include <variant>

using namespace std::string_literals;

namespace Game::Graphics {

Engine::Graphics::Animated_sprites load_mike_sprites(Engine::Sdl::Renderer& renderer)
{
        // In production, we'll load the entire sprites texture right away,
        // and then load the individual configs lazily. Lazily loading the configs helps
        // because then I don't have to repeat any code.

        Engine::Graphics::Animated_sprites sprites;

        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/running"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/standing_still"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/preparing_to_jump_sideways"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/jumping_sideways"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/jumping_in_place"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/landing_sideways"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/pulling"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/pushing"s);

        return sprites;
}

Mike_sprite::Mike_sprite(Engine::Graphics::Animated_sprites& sprites,
                         Logic::Mike const& mike) noexcept
        : sprites_(&sprites)
        , mike_(&mike)
{}

void Mike_sprite::render(Engine::Sdl::Renderer& renderer)
{
        auto const flip = current_flip();
        current_sprite().render(renderer, mike_->position(), 0, flip);
}

void Mike_sprite::update()
{
        current_sprite().update();
}

Engine::Sdl::Flip Mike_sprite::current_flip() const noexcept
{
        if (mike_->is_facing_left())
                return Engine::Sdl::Flip::horizontal;
        return Engine::Sdl::Flip::none;
}

}

