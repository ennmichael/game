#pragma once

#include "engine/engine.h"
#include "block.h"
#include "utils.h"
#include <unordered_map>
#include <variant>

namespace Game::Logic {

enum class Direction {
        none,
        left,
        right
};

// TODO Rename Standing_still -> Idle

class Mike {
public:
        struct Standing_still {};
        struct Running {};
        struct Preparing_to_jump_sideways {};
        struct Jumping_sideways {};
        struct Landing_sideways {};
        struct Jumping_in_place {};
        struct Climbing {};

        struct Holding_block {
                Blocks::iterator block;
        };

        struct Pushing_block {
                Blocks::iterator block;
        };

        struct Pulling_block {
                Blocks::iterator block;
        };

        using State = std::variant<Standing_still,
                                   Running,
                                   Preparing_to_jump_sideways,
                                   Jumping_sideways,
                                   Landing_sideways,
                                   Jumping_in_place,
                                   Climbing,
                                   Holding_block,
                                   Pushing_block,
                                   Pulling_block>;
        
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
        bool is_holding_block() const noexcept;
        bool is_pushing_block() const noexcept;
        bool is_pulling_block() const noexcept;
        bool is_in_motion() const noexcept;

        template <class T>
        bool state_is() const noexcept
        {
                return Utils::variant_matches<T>(state_);
        }

        bool is_facing_left() const noexcept;
        bool is_facing_right() const noexcept;

        void grab_nearest_block(Blocks& blocks) noexcept;
        void move_left() noexcept;
        void move_right() noexcept;
        void stand_still() noexcept;
        Engine::Gameplay::Timed_callback jump() noexcept;
      
        void update_position(Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept;

        Direction direction() const noexcept;
        State state() const noexcept;
        Engine::Complex_number position() const noexcept;
        Engine::Gameplay::Checkbox checkbox() const noexcept;

private:
        Engine::Gameplay::Timed_callback stand_still_after(Engine::Duration::Frames duration);
        bool can_be_translated(Engine::Complex_number delta,
                               Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) const noexcept;
        void translate_if_possible(Engine::Complex_number delta,
                                   Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept;
        void run_in_direction(Direction direction) noexcept;

        Engine::Complex_number position_;
        Actions_durations durations_;
        int width_;
        int height_;
        Direction direction_ = Direction::none;
        State state_ = Standing_still();
        double speed_ = 0;
};

// Doesn't matter if `register_mike_keyboard_controls` sucks now,
// the whole `Key_binding` mechanism will be more generalized later
void register_mike_keyboard_controls(Mike& mike, Blocks& blocks, Engine::Gameplay::Signals& signals);
bool user_has_control(Mike const& mike);

}

