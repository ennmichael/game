#include "mike_sprite.h"
#include <cassert>

using namespace std::string_literals;

namespace Game::Graphics {

Engine::Graphics::Animated_sprites load_mike_sprites(Engine::Sdl::Renderer& renderer)
{
        // TODO Load sprites lazily
        // TODO In production, we'll load the entire sprites texture right away,
        // and then load the individual configs lazily

        Engine::Graphics::Animated_sprites sprites;

        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/running"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/standing_still"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/preparing_to_jump_sideways"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/jumping_sideways"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/jumping_in_place"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/landing_sideways"s);
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/pulling");
        Engine::Graphics::load_animated_sprite(sprites, renderer, "../res/sprites/pushing");

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

Engine::Graphics::Animated_sprite& Mike_sprite::current_sprite() noexcept
{
        switch (mike_->state()) {
                case Logic::Mike::State::standing_still: return sprites_->at("standing_still"s);
                case Logic::Mike::State::running: return sprites_->at("running");
                case Logic::Mike::State::jumping_in_place: return sprites_->at("jumping_in_place"s);
                case Logic::Mike::State::preparing_to_jump_sideways: return sprites_->at("preparing_to_jump_sideways"s);
                case Logic::Mike::State::jumping_sideways: return sprites_->at("jumping_sideways"s);
                case Logic::Mike::State::landing_sideways: return sprites_->at("landing_sideways"s);
                case Logic::Mike::State::climbing: return sprites_->at("climbing"s);
                case Logic::Mike::State::pulling: return sprites_->at("pulling"s);
                case Logic::Mike::State::pushing: return sprites_->at("pushing"s);
        }

        assert(false);
        return sprites_->at("standing_still"s);
}

Engine::Sdl::Flip Mike_sprite::current_flip() const noexcept
{
        if (mike_->is_facing_left())
                return Engine::Sdl::Flip::horizontal;
        return Engine::Sdl::Flip::none;
}

}

