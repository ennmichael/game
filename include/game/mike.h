#pragma once

#include "engine/engine.h"
#include <unordered_map>

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
             Actions_durations const& durations) noexcept;

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
      
        void update_position(Engine::Gameplay::Checkboxes const& solid_checkboxes) noexcept;

        Direction direction() const noexcept;
        State state() const noexcept;
        Engine::Complex_number position() const noexcept;

private:
        Engine::Gameplay::Timed_callback stand_still_after(Engine::Duration::Frames duration);
        Engine::Gameplay::Checkbox checkbox() const noexcept;
        bool can_be_translated(Engine::Complex_number delta,
                               Engine::Gameplay::Checkboxes const& solid_checkboxes) const noexcept;
        void translate_if_possible(Engine::Complex_number delta,
                                   Engine::Gameplay::Checkboxes const& solid_checkboxes) noexcept;

        Engine::Complex_number position_;
        Actions_durations durations_;
        int checkbox_width;
        int checkbox_height;
        Direction direction_ = Direction::none;
        State state_ = State::standing_still;
        double speed_ = 0;
};

// Terrible function name
void register_mike_keyboard_controls(Mike& mike, Engine::Gameplay::Signals& signals);
bool user_has_control(Mike const& mike);

}

