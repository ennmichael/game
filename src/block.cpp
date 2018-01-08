#include "engine/utils.h"
#include "block.h"

namespace Game::Logic {

Block::Block(Engine::Complex_number position,
             Flags flags,
             int width,
             int height) noexcept
        : position_(position)
        , flags_(flags)
        , width_(width)
        , height_(height)
{}

bool Block::is_solid() const noexcept
{
        return (flags_ & Flags::solid) != Flags::none;
}

bool Block::is_movable() const noexcept
{
        return (flags_ & Flags::movable) != Flags::none;
}

Engine::Complex_number Block::position() const noexcept
{
        return position_;
}

Engine::Gameplay::Checkbox Block::checkbox() const noexcept
{
        return {
                position_,
                width_,
                height_
        };
}

Block::Flags operator&(Block::Flags f1, Block::Flags f2) noexcept
{
        return Block::Flags {
                Engine::Utils::underlying_value(f1) & Engine::Utils::underlying_value(f2)
        };
}

Block::Flags operator|(Block::Flags f1, Block::Flags f2) noexcept
{
        return Block::Flags {
                Engine::Utils::underlying_value(f1) | Engine::Utils::underlying_value(f2)
        };
}

}

