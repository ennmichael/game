#pragma once

#include "mike.h"
#include <unordered_map>
#include <string>

namespace Game::Graphics {

class Mike_animations {
public:
        Mike_animations(Gameplay::Mike const& mike,
                        Engine::Graphics::Animations& animations) noexcept;

        void render_current_animation(Engine::Sdl::Renderer& renderer);
        void update_current_animation() noexcept;

private:
        Engine::Graphics::Animation& current_animation() noexcept;
        Engine::Sdl::Flip current_flip() const noexcept;

        Engine::Graphics::Sprite_sheet* sprite_sheet_;
        Gameplay::Mike const* mike_;
        Engine::Graphics::Animations* animations_;
};

}

