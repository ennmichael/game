#include "mike.h"
#include "utils.h"
#include <unordered_map>

using namespace std::string_literals;

namespace Game::Logic {

Mike::Mike(Configs::Mike_config const& mike_config,
           Actions_durations const& durations) noexcept
        : Mike(mike_config.starting_position, mike_config.dimensions, mike_config.speed, durations)
{}

Mike::Mike(Engine::Complex_number position,
           Engine::Dimensions dimensions,
           double speed,
           Actions_durations const& durations) noexcept
        : position_(position)
        , dimensions_(dimensions)
        , speed_(speed)
        , durations_(durations)
{}

auto Mike::idle() -> State
{
        return {
                [](Mike&, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state {
                        if (keyboard.key_down(Engine::Sdl::Keycodes::left) ||
                            keyboard.key_down(Engine::Sdl::Keycodes::right))
                                return running();
                        return boost::none;
                },
                "idle"s
        };
}

auto Mike::running() -> State
{
        return {
                [](Mike& mike, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state {
                        if (keyboard.key_down(Engine::Sdl::Keycodes::left)) {
                                mike.move_left();
                                return boost::none;
                        }

                        if (keyboard.key_down(Engine::Sdl::Keycodes::right)) {
                                mike.move_right();
                                return boost::none;
                        }

                        return idle();
                },
                "running"s
        };
}

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
        return {
                position_,
                dimensions_
        };
}

std::string const& Mike::current_sprite_name() const noexcept
{
        return state_.sprite_name;
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
        direction_ = direction;
        position_ = Engine::Gameplay::translated_position(position_,
                                                          direction_,
                                                          speed_);
}

}

