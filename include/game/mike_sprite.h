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
        void switch_animation();

private:
        static current_mike_animation(Sprite_sheet& sprite_sheet, Logic::Mike const& mike);

        Engine::Graphics::Animation& current_animation() noexcept;
        Engine::Sdl::Flip current_flip() const noexcept;

        Engine::Gtaphics::Sprite_sheet* sprite_sheet_;
        Logic::Mike const* mike_;
        Engine::Graphics::Animation current_animation_;
};

}

