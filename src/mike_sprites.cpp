#include "mike_sprites.h"
#include <cassert>

using namespace std::string_literals;

namespace Game::Graphics {

// TODO Have a way to load sprites without using absolute paths
// Perhaps use a config file to specify the base of the path?

Mike_sprites Mike_sprites::load(Engine::Sdl::Renderer& renderer)
{
        return {
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/still"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/run"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/jump_preparation"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/jump"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/jump_still"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/run"s) // TODO Should be "climb"s
        };
}

Logic::Mike::Actions_durations Mike_sprites::actions_duratons() const noexcept
{
        Logic::Mike::Actions_durations durations;

        durations.jump_preparation = Engine::Duration::Frames {60};
        durations.jump = jump.animation_duration();

        return durations;
}

Mike_sprite::Mike_sprite(Mike_sprites sprites, Logic::Mike const& mike) noexcept
        : sprites_(std::move(sprites))
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
        auto const jump_sprite =
        [this]() noexcept -> Engine::Graphics::Animated_sprite&
        {
                if (mike_->is_facing_left() || mike_->is_facing_right())
                        return sprites_.jump;
                return sprites_.jump_still;
        };

        switch (mike_->state()) {
                case Logic::Mike::State::standing_still:    return sprites_.still;
                case Logic::Mike::State::running:           return sprites_.run;
                case Logic::Mike::State::preparing_to_jump: return jump_sprite();
                case Logic::Mike::State::jumping:           return jump_sprite();
                case Logic::Mike::State::climbing:          return sprites_.climb;
        }

        assert(false);
        return sprites_.still;
}

Engine::Sdl::Flip Mike_sprite::current_flip() const noexcept
{
        if (mike_->is_facing_left())
                return Engine::Sdl::Flip::horizontal;
        return Engine::Sdl::Flip::none;
}

}

