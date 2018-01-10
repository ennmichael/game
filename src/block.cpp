#include "engine/utils.h"
#include "block.h"

namespace Game::Logic {

Engine::Gameplay::Checkbox Block::checkbox() const noexcept
{
        return {
                position,
                width,
                height
        };
}

}

