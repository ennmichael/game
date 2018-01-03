#include "mike_animations.h"
#include <cassert>

using namespace std::string_literals;

namespace Game::Graphics {

// TODO Have a way to load sprites without using absolute paths
// Perhaps use a config file to specify the base of the path?

Mike_animations Mike_animations::load(Engine::Sdl::Renderer& renderer)
{
        return {
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/still"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/run"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/jump"s),
                Engine::Graphics::Animated_sprite::load(renderer, "../res/sprites/run"s) // TODO Should be "climb"s
        };
}

Engine::Graphics::Animated_sprite& Mike_animations::current_sprite(Logic::Mike const& mike) noexcept
{
        switch (mike.state()) {
                case Logic::Mike::State::standing_still: return still_;
                case Logic::Mike::State::running:        return running_;
                case Logic::Mike::State::jumping:        return jump_;
                case Logic::Mike::State::climbing:       return climb_;
        }

        assert(false);
        return still_;
}

}

