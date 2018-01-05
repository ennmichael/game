#pragma once

#include "mike.h"
#include <unordered_map>
#include <string>

namespace Game::Graphics {

Engine::Graphics::Animated_sprites load_mike_sprites(Engine::Sdl::Renderer& renderer);

class Mike_sprite {
public:
        Mike_sprite(Engine::Graphics::Animated_sprites& sprites,
                    Logic::Mike const& mike) noexcept;

        void render(Engine::Sdl::Renderer& renderer);
        void update();

private:
        Engine::Graphics::Animated_sprite& current_sprite() noexcept;
        Engine::Sdl::Flip current_flip() const noexcept;

        Engine::Graphics::Animated_sprites* sprites_;
        Logic::Mike const* mike_;
};

}

