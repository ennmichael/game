#pragma once

#include "engine/engine.h"

namespace Game::Logic {

enum class Direction {
        none,
        left,
        right
};

class Mike {
public:
        explicit Mike(Engine::Complex_number position) noexcept;

        enum class State {
                standing_still,
                running,
                jumping,
                climbing
        }; 

        bool is_standing_still() const noexcept;
        bool is_running() const noexcept;
        bool is_jumping() const noexcept;
        bool is_climbing() const noexcept;

        bool is_facing_left() const noexcept;
        bool is_facing_right() const noexcept;

        void run_left() noexcept;
        void run_right() noexcept;
        void stand_still() noexcept;
       
        Direction direction() const noexcept;
        State state() const noexcept;
        Engine::Complex_number position() const noexcept;

private:
        Engine::Complex_number position_;
        Direction direction_ = Direction::none;
        State state_ = State::standing_still;
        double speed_ = 0;
};

void register_mike_keyboard_controls(Mike& mike, Engine::Gameplay::Signals& signals); // Terrible name
bool user_has_control(Mike const& mike);

}

