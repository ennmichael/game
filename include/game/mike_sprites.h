#pragma once

#include "mike.h"

namespace Game::Graphics {

struct Mike_sprites {
        static Mike_sprites load(Engine::Sdl::Renderer& renderer);
        
        Logic::Mike::Actions_durations actions_duratons() const noexcept;

        Engine::Graphics::Animated_sprite still;
        Engine::Graphics::Animated_sprite run;
        Engine::Graphics::Animated_sprite jump_preparation;
        Engine::Graphics::Animated_sprite jump;
        Engine::Graphics::Animated_sprite jump_still;
        Engine::Graphics::Animated_sprite climb;
};

class Mike_sprite {
public:
        Mike_sprite(Mike_sprites sprites, Logic::Mike const& mike) noexcept;

        void render(Engine::Sdl::Renderer& renderer);
        void update();

private:
        Engine::Graphics::Animated_sprite& current_sprite() noexcept;
        Engine::Sdl::Flip current_flip() const noexcept;

        // TODO Should actually have a current_sprite_ member
        Mike_sprites sprites_;
        Logic::Mike const* mike_;
};

}

