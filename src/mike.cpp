#include "mike.h"
#include <unordered_map>

using namespace std::string_literals;

namespace Game::Logic {

Mike::Mike(Engine::Complex_number position,
           Actions_durations const& durations,
           int width,
           int height) noexcept
        : position_(position)
        , durations_(durations)
        , width_(width)
        , height_(height)
{
        auto const config = Engine::Config::load("../res/mike.config"s);
        config.unpack_value("speed"s, speed_);
}

bool Mike::is_standing_still() const noexcept
{
        return state_ == State::standing_still;
}

bool Mike::is_running() const noexcept
{
        return state_ == State::running;
}

bool Mike::is_preparing_to_jump_sideways() const noexcept
{
        return state_ == State::preparing_to_jump_sideways;
}

bool Mike::is_jumping_sideways() const noexcept
{
        return state_ == State::jumping_sideways;
}

bool Mike::is_landing_sideways() const noexcept
{
        return state_ == State::landing_sideways;
}

bool Mike::is_jumping_in_place() const noexcept
{
        return state_ == State::jumping_in_place;
}

bool Mike::is_jumping() const noexcept
{
        return is_jumping_sideways() || is_jumping_in_place();
}

bool Mike::is_climbing() const noexcept
{
        return state_ == State::climbing;
}

bool Mike::is_in_motion() const noexcept
{
        return is_jumping_sideways() || is_running();
}

bool Mike::is_facing_left() const noexcept
{
        return direction_ == Direction::left;
}

bool Mike::is_facing_right() const noexcept
{
        return direction_ == Direction::right;
}

void Mike::run_left() noexcept
{
        direction_ = Direction::left;
        state_ = State::running;
}

void Mike::run_right() noexcept
{
        direction_ = Direction::right;
        state_ = State::running;
}

void Mike::stand_still() noexcept
{
        direction_ = Direction::none;
        state_ = State::standing_still;
}

Engine::Gameplay::Timed_callback Mike::jump() noexcept
{
        auto const jump_in_place =
        [this]
        {
                state_ = State::jumping_in_place;
                return stand_still_after(durations_.at("jumping_in_place"s));
        };

        auto const land_sideways =
        [this]
        {
                state_ = State::landing_sideways;
                return stand_still_after(durations_.at("landing_sideways"s));
        };

        auto const jump_sideways =
        [this, land_sideways]
        {
                state_ = State::preparing_to_jump_sideways;

                return Engine::Gameplay::Timed_callback(
                        [this, land_sideways]
                        {
                                state_ = State::jumping_sideways;
                                return Engine::Gameplay::Timed_callback(land_sideways, durations_.at("jumping_sideways"s));
                        },
                        durations_.at("preparing_to_jump_sideways"s)
                );
        };

        if (is_standing_still())
                return jump_in_place();
        else
                return jump_sideways();
}

void Mike::update_position(Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept
{
        auto const move_forward =
        [this](Engine::Gameplay::Checkboxes_thunks solid_checkboxes_thunks)
        {
                if (is_facing_left())
                        translate_if_possible(-speed_, solid_checkboxes_thunks);
                else if (is_facing_right())
                        translate_if_possible(speed_, solid_checkboxes_thunks);
        };

        if (is_in_motion())
                move_forward(solid_checkboxes_thunks);
}

Direction Mike::direction() const noexcept
{
        return direction_;
}

auto Mike::state() const noexcept -> State
{
        return state_;
}

Engine::Complex_number Mike::position() const noexcept
{
        return position_;
}

Engine::Gameplay::Timed_callback Mike::stand_still_after(Engine::Duration::Frames duration)
{
        return Engine::Gameplay::Timed_callback(
                [this]{ stand_still(); },
                duration
        );
}

Engine::Gameplay::Checkbox Mike::checkbox() const noexcept
{
        return {
                position_,
                width_,
                height_
        };
}

bool Mike::can_be_translated(Engine::Complex_number delta,
                             Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) const noexcept
{
        return checkbox().can_be_translated(delta, solid_checkboxes_thunks);
}

void Mike::translate_if_possible(Engine::Complex_number delta,
                                 Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept
{
        if (can_be_translated(delta, solid_checkboxes_thunks))
                position_ += delta;
}

void register_mike_keyboard_controls(Mike& mike, Engine::Gameplay::Signals& signals)
{
        // TODO Generalize the key binding system

        struct Key_binding { // We could use function pointers here, if that matters
                std::function<bool(Engine::Gameplay::Keyboard const&)> checker;
                std::function<void(Engine::Gameplay::Main_loop&)> callback;
        };

        std::vector key_bindings {
                Key_binding {
                        [](Engine::Gameplay::Keyboard const& keyboard) noexcept
                        {
                                return keyboard.key_down(Engine::Sdl::Keycodes::a) ||
                                       keyboard.key_down(Engine::Sdl::Keycodes::left);
                        },
                        [&](Engine::Gameplay::Main_loop&)
                        {
                                mike.run_left();
                        }
                },
                Key_binding {
                        [](Engine::Gameplay::Keyboard const& keyboard) noexcept
                        {
                                return keyboard.key_down(Engine::Sdl::Keycodes::d) ||
                                       keyboard.key_down(Engine::Sdl::Keycodes::right);
                        },
                        [&](Engine::Gameplay::Main_loop&)
                        {
                                mike.run_right();
                        }
                },
                Key_binding {
                        [](Engine::Gameplay::Keyboard const& keyboard) noexcept
                        {
                                return keyboard.key_pressed(Engine::Sdl::Keycodes::w) ||
                                       keyboard.key_pressed(Engine::Sdl::Keycodes::up) ||
                                       keyboard.key_pressed(Engine::Sdl::Keycodes::space);
                        },
                        [&](Engine::Gameplay::Main_loop& main_loop)
                        {
                                main_loop.register_callback(mike.jump());
                        }
                }
        };

        signals.frame_advance.connect(
                [&mike, key_bindings = std::move(key_bindings)](Engine::Gameplay::Main_loop& main_loop,
                                                                Engine::Gameplay::Keyboard const& keyboard)
                {
                        if (!user_has_control(mike))
                                return;

                        bool no_key_pressed = true;

                        for (auto const& [checker, callback] : key_bindings) {
                                if (checker(keyboard)) {
                                        callback(main_loop);
                                        no_key_pressed = false;
                                }
                        }

                        if (no_key_pressed)
                                mike.stand_still();
                }
        );
}

bool user_has_control(Mike const& mike)
{
        return mike.is_standing_still() || mike.is_running();
}

}

