#pragma once

#include "mike.h"
#include <unordered_map>
#include <string>

namespace Game::Graphics {

class Mike_sprite {
public:
        Mike_sprite(Engine::Graphics::Sprite_sheet& sprite_sheet,
                    Logic::Mike const& mike) noexcept;

        void render(Engine::Sdl::Renderer& renderer);
        void update();

private:
        Engine::Graphics::Animation& current_animation() noexcept;
        Engine::Sdl::Flip current_flip() const noexcept;

        Engine::Graphics::Sprite_sheet* sprite_sheet_;
        Logic::Mike const* mike_;
};

}

