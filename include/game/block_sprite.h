#pragma once

#include "engine/sdl++.h"
#include "block.h"

namespace Game::Graphics {

class Block_texture {
public:
        Block_texture(Engine::Sdl::Unique_texture texture,
                      Logic::Block const& block) noexcept;

        void render(Engine::Sdl::Renderer& renderer);

private:
        Engine::Sdl::Texture texture_;
        Logic::Block const* block_;
};

}

