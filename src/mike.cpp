#include "mike.h"
#include "utils.h"
#include <unordered_map>

using namespace std::string_literals;

namespace Game::Gameplay {

Mike::Mike(Configs::Mike_config const& mike_config,
           Actions_durations const& durations) noexcept
        : Mike(mike_config.starting_position,
               mike_config.dimensions,
               mike_config.speed,
               mike_config.jump_speed,
               durations)
{}

Mike::Mike(Engine::Complex_number position,
           Engine::Dimensions dimensions,
           double speed,
           double jump_speed,
           Actions_durations const& durations) noexcept
        : position_(position)
        , dimensions_(dimensions)
        , speed_(speed)
        , jump_speed_(jump_speed)
        , durations_(durations)
{}

bool Mike::is_facing_left() const noexcept
{
        return direction_ == Engine::Gameplay::Direction::left;
}

bool Mike::is_facing_right() const noexcept
{
        return direction_ == Engine::Gameplay::Direction::right;
}

void Mike::snap_to(Engine::Gameplay::Checkbox checkbox) noexcept
{
        auto const new_real =
        [this](Engine::Gameplay::Checkbox checkbox) noexcept
        {
                return (position_.real() > checkbox.position.real()) ?
                        checkbox.position.real() + checkbox.dimensions.width :
                        checkbox.position.real() - dimensions_.width;
        };

        position_.real(new_real(checkbox));
}

void Mike::update(Engine::Gameplay::Keyboard const& keyboard)
{
        auto const new_state = state_.updater(*this, keyboard);
        if (new_state)
                state_ = *new_state;
}

Engine::Gameplay::Direction Mike::direction() const noexcept
{
        return direction_;
}

Engine::Complex_number Mike::position() const noexcept
{
        return position_;
}

Engine::Gameplay::Checkbox Mike::checkbox() const noexcept
{
        return {position_, dimensions_};
}

std::string const& Mike::current_sprite_name() const noexcept
{
        return state_.sprite_name;
}

auto Mike::idle() -> State
{
        return {
                "idle"s,

                [](Mike& mike, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state {
                        if (keyboard.key_down(Engine::Sdl::Keycodes::left) ||
                            keyboard.key_down(Engine::Sdl::Keycodes::right))
                                return running();

                        if (keyboard.key_down(Engine::Sdl::Keycodes::space))
                                return jumping_in_place(mike.durations_);

                        return boost::none;
                }
        };
}

auto Mike::running() -> State
{
        return {
                "running"s,

                [](Mike& mike, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state {
                        if (keyboard.key_down(Engine::Sdl::Keycodes::space))
                                return preparing_to_jump_sideways(mike.durations_);

                        if (keyboard.key_down(Engine::Sdl::Keycodes::left)) {
                                mike.move_left();
                                return boost::none;
                        }

                        if (keyboard.key_down(Engine::Sdl::Keycodes::right)) {
                                mike.move_right();
                                return boost::none;
                        }

                        return idle();
                }
        };
}

auto Mike::jumping_in_place(Actions_durations const& durations) -> State
{
        return expiring_state({"jumping_in_place"s}, idle(), durations);
}

auto Mike::preparing_to_jump_sideways(Actions_durations const& durations) -> State
{
        return expiring_state({"preparing_to_jump_sideways"s}, jumping_sideways(durations), durations);
}

auto Mike::jumping_sideways(Actions_durations const& durations) -> State
{
        State state {
                "jumping_sideways"s,

                [](Mike& mike, Engine::Gameplay::Keyboard const&) -> Optional_state
                {
                        mike.move_in_current_direction(mike.jump_speed_);
                        return boost::none;
                }
        };

        return expiring_state(state, landing_sideways(durations), durations);
}

auto Mike::landing_sideways(Actions_durations const& durations) -> State
{
        return expiring_state({"landing_sideways"s}, idle(), durations);
}

auto Mike::expiring_state(State state,
                          State next_state,
                          Engine::Duration::Frames duration) -> State
{
        return {
                state.sprite_name,

                [frame_timer = Engine::Gameplay::Frame_timer(duration),
                 state       = std::move(state),
                 next_state  = std::move(next_state)]
                (Mike& mike, Engine::Gameplay::Keyboard const& keyboard) mutable -> Optional_state
                {
                        frame_timer.update();
                        auto const new_state = state.updater(mike, keyboard);

                        if (new_state)
                                return new_state;

                        if (frame_timer.ready())
                                return next_state;

                        return boost::none;
                }
        };
}

auto Mike::expiring_state(State state, State next_state, Actions_durations const& durations) -> State
{
        auto const duration = durations.at(state.sprite_name);
        return expiring_state(state, next_state, duration);
}

void Mike::move_left() noexcept
{
        move_in_direction(Engine::Gameplay::Direction::left);
}

void Mike::move_right() noexcept
{
        move_in_direction(Engine::Gameplay::Direction::right);
}

void Mike::move_in_direction(Engine::Gameplay::Direction direction) noexcept
{
        move_in_direction(direction, speed_);
}

void Mike::move_in_direction(Engine::Gameplay::Direction direction, double speed) noexcept
{
        direction_ = direction;
        move_in_current_direction(speed);
}

void Mike::move_in_current_direction(double speed) noexcept
{
        position_ = Engine::Gameplay::translated_position(position_,
                                                          direction_,
                                                          speed);
}

}

