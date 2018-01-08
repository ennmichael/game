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

        Block(Engine::Complex_number position,
              Flags flags,
              int width,
              int height) noexcept;

        bool is_solid() const noexcept;
        bool is_movable() const noexcept;

        Engine::Complex_number position() const noexcept;
        Engine::Gameplay::Checkbox checkbox() const noexcept;

private:
        Engine::Complex_number position_;
        Flags flags_;
        int width_;
        int height_;
};

Block::Flags operator&(Block::Flags f1, Block::Flags f2) noexcept;
Block::Flags operator|(Block::Flags f1, Block::Flags f2) noexcept;

}

