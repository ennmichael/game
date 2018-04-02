#include "mike.h"
#include "utils.h"
#include <unordered_map>

using namespace std::string_literals;

namespace Game::Gameplay {

Mike::Mike(boost::property_tree::ptree const& config,
           Actions_durations const& actions_durations)
        : position_(config.get<int>("x"s), config.get<int>("y"s))
        , dimensions_(config.get<int>("width"s), config.get<int>("height"s))
        , sickness_(config.get<int>("sickness_increase_rate"s),
                    config.get<int>("sickness_decrease_rate"s))
        , speed_(config.get<double>("speed"s))
        , jump_speed_(config.get<double>("jump_speed"s))
        , durations_(actions_durations)
        , animations_(std::move(animations))
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
        Engine::Gameplay::update_state(state_, *this, keyboard);
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

auto Mike::expiring_state(State state, State next_state, Actions_durations const& durations) -> State
{
        auto const duration = durations.at(state.sprite_name);
        return Engine::Gameplay::expiring_state(std::move(state), std::move(next_state), duration);
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

void apply_sickness_filter(Sdl::Renderer& renderer, Mike const& mike)
{
        auto const filter_alpha =
        [&]
        {
                unsigned char const min_alpha = 50;
                unsigned char const max_alpha = 210;
                unsigned char const dynamic_alpha = max_alpha - min_alpha;
                return base_alpha + dynamic_alpha*mike.sickness_percentage() / 100;
        };

        auto const filter_color = Color::black().with_alpha(filter_alpha());
        Engine::Graphics::apply_filter(renderer, filter_color);
}

}

