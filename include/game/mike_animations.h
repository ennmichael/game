#pragma once

#include "mike.h"

namespace Game::Graphics {

struct Mike_animations {
public:
        static Mike_animations load(Engine::Sdl::Renderer& renderer);

        Engine::Graphics::Animated_sprite& current_sprite(Logic::Mike const& mike) noexcept;

        Engine::Graphics::Animated_sprite still_;
        Engine::Graphics::Animated_sprite running_;
        Engine::Graphics::Animated_sprite jump_;
        Engine::Graphics::Animated_sprite climb_;
};

}

