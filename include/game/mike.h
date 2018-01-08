#pragma once

#include "engine/engine.h"
#include <unordered_map>

// TODO Make a difference between checkboxes and their thunks

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
                preparing_to_jump_sideways,
                jumping_sideways,
                landing_sideways,
                jumping_in_place,
                climbing
        };

        using Actions_durations = std::unordered_map<std::string, Engine::Duration::Frames>;

        Mike(Engine::Complex_number position,
             Actions_durations const& durations,
             int width,
             int height) noexcept;

        bool is_standing_still() const noexcept;
        bool is_running() const noexcept;
        bool is_preparing_to_jump_sideways() const noexcept;
        bool is_jumping_sideways() const noexcept;
        bool is_landing_sideways() const noexcept;
        bool is_jumping_in_place() const noexcept;
        bool is_jumping() const noexcept;
        bool is_climbing() const noexcept;
        bool is_in_motion() const noexcept;

        bool is_facing_left() const noexcept;
        bool is_facing_right() const noexcept;

        void run_left() noexcept;
        void run_right() noexcept;
        void stand_still() noexcept;
        Engine::Gameplay::Timed_callback jump() noexcept;
      
        void update_position(Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept;

        Direction direction() const noexcept;
        State state() const noexcept;
        Engine::Complex_number position() const noexcept;

private:
        Engine::Gameplay::Timed_callback stand_still_after(Engine::Duration::Frames duration);
        Engine::Gameplay::Checkbox checkbox() const noexcept; // This could be public, doesn't matter right now
        bool can_be_translated(Engine::Complex_number delta,
                               Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) const noexcept;
        void translate_if_possible(Engine::Complex_number delta,
                                   Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept;

        Engine::Complex_number position_;
        Actions_durations durations_;
        int width_;
        int height_;
        Direction direction_ = Direction::none;
        State state_ = State::standing_still;
        double speed_ = 0;
};

// Terrible function name
void register_mike_keyboard_controls(Mike& mike, Engine::Gameplay::Signals& signals);
bool user_has_control(Mike const& mike);

}

