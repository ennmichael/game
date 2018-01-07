#include "block_sprite.h"
#include <utility>

namespace Game::Graphics {

Block_texture::Block_texture(Engine::Sdl::Unique_texture texture,
                             Logic::Block const& block) noexcept
        : texture_(std::move(texture))
        , block_(&block)
{}

void Block_texture::render(Engine::Sdl::Renderer& renderer)
{
        Engine::Sdl::render_copy(renderer, *texture_, block_->position);
}

}

