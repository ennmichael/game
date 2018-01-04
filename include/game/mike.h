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
        enum class State {
                standing_still,
                running,
                jumping,
                climbing
        };

        struct Actions_durations {
                Engine::Duration::Frames jump_preparation {0};
                Engine::Duration::Frames jump {0};
        };

        Mike(Engine::Complex_number position,
             Actions_durations const& durations) noexcept;

        bool is_standing_still() const noexcept;
        bool is_running() const noexcept;
        bool is_jumping() const noexcept;
        bool is_climbing() const noexcept;

        bool is_facing_left() const noexcept;
        bool is_facing_right() const noexcept;

        void run_left() noexcept;
        void run_right() noexcept;
        void stand_still() noexcept;

        Engine::Gameplay::Timed_callback jump() noexcept;
      
        void update_position() noexcept;

        Direction direction() const noexcept;
        State state() const noexcept;
        Engine::Complex_number position() const noexcept;

private:
        bool user_has_control() const noexcept;

        Engine::Complex_number position_;
        Actions_durations durations_;
        Direction direction_ = Direction::none;
        State state_ = State::standing_still; 
        double speed_ = 0;
};

// Terrible function name
void register_mike_keyboard_controls(Mike& mike, Engine::Gameplay::Signals& signals);
bool user_has_control(Mike const& mike);

}

