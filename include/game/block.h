#pragma once

#include "engine/gameplay.h"
#include <vector>

namespace Game::Logic {

struct Block {
        Engine::Gameplay::Checkbox checkbox() const noexcept;

        Engine::Complex_number position;
        int width;
        int height;
};

using Blocks = std::vector<Block>;

}

