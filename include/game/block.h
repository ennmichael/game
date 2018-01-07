#pragma once

#include "engine/gameplay.h"

namespace Game::Logic {

class Block {
public:
        enum class Flags {
                none = 0,
                solid = 1,
                movable = 2
        };

        Block(Engine::Complex_number position, Flags flags) noexcept;

        bool is_solid() const noexcept;
        bool is_movable() const noexcept;

        Engine::Complex_number position() const noexcept;

private:
        Engine::Complex_number position_;
        Flags flags_;
};

}

