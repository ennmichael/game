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
        return Utils::lambda_visit(mike_->state(),
                [this](Logic::Mike::Standing_still const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("standing_still"s); },

                [this](Logic::Mike::Running const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("running"s); },
                
                [this](Logic::Mike::Jumping_in_place const&) -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("jumping_in_place"s); },
                
                [this](Logic::Mike::Preparing_to_jump_sideways const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("preparing_to_jump_sideways"s); },
                
                [this](Logic::Mike::Jumping_sideways const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("jumping_sideways"s); },
                
                [this](Logic::Mike::Landing_sideways const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("landing_sideways"s); },
                
                [this](Logic::Mike::Climbing const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("climbing"s); },
                
                [this](Logic::Mike::Holding_block const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("running"s); }, // TODO This is incorrect

                [this](Logic::Mike::Pulling_block const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("pulling"s); },
                
                [this](Logic::Mike::Pushing_block const&) noexcept -> Engine::Graphics::Animated_sprite&
                { return sprites_->at("pushing"s); }
        );
}

Engine::Sdl::Flip Mike_sprite::current_flip() const noexcept
{
        if (mike_->is_facing_left())
                return Engine::Sdl::Flip::horizontal;
        return Engine::Sdl::Flip::none;
}

}

