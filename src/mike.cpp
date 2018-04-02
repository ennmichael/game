#include "mike.h"
#include "utils.h"
#include <unordered_map>


#include <iostream>


using namespace std::string_literals;

namespace Game::Gameplay {

Mike::Mike(boost::property_tree::ptree const& config,
           Engine::Graphics::Animations animations)
        : position_(config.get<int>("x"s), config.get<int>("y"s))
        , dimensions_ {config.get<int>("width"s), config.get<int>("height"s)}
        , sickness_(config.get<double>("sickness_increase_rate"s),
                    config.get<double>("sickness_decrease_rate"s))
        , speed_(config.get<double>("speed"s))
        , jump_speed_(config.get<double>("jump_speed"s))
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

void Mike::render(Engine::Sdl::Renderer& renderer)
{
        current_animation_.render(renderer, position_, current_flip());
}

void Mike::render_sickness_filter(Engine::Sdl::Renderer& renderer)
{
        auto const filter_alpha =
        [&]
        {
                unsigned char const min_alpha = 50;
                unsigned char const max_alpha = 210;
                unsigned char const dynamic_alpha = max_alpha - min_alpha;
                return min_alpha + dynamic_alpha*sickness_.percentage() / 100;
        };

        auto const filter_color = Engine::Color::black().with_alpha(filter_alpha());
        Engine::Graphics::apply_filter(renderer, filter_color);
}

Mike::Sickness::Sickness(double increase_rate, double decrease_rate) noexcept
        : increase_rate_(increase_rate)
        , decrease_rate_(decrease_rate)
{}

double Mike::Sickness::percentage() const noexcept
{
        return percentage_;
}

void Mike::Sickness::increase() noexcept
{
        percentage_ += increase_rate_;
        if (percentage_ > 100.f)
                percentage_ = 100.f;
}

void Mike::Sickness::decrease() noexcept
{
        percentage_ -= decrease_rate_;
        if (percentage_ < 0.0f)
                percentage_ = 0.0f;
}

auto Mike::idle() -> State
{
        return looping_animation("idle",
                                 [](Mike&, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state
                                 {
                                         if (keyboard.key_down(Engine::Sdl::Keycodes::left) ||
                                             keyboard.key_down(Engine::Sdl::Keycodes::right))
                                                 return running();

                                         if (keyboard.key_down(Engine::Sdl::Keycodes::space))
                                                 return jumping_in_place();

                                         return boost::none;
                                 });
}

auto Mike::running() -> State
{
        return looping_animation("running",
                [](Mike& mike, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state {
                        if (keyboard.key_down(Engine::Sdl::Keycodes::space))
                                return preparing_to_jump_sideways();

                        if (keyboard.key_down(Engine::Sdl::Keycodes::left)) {
                                mike.move_left();
                                return boost::none;
                        }

                        if (keyboard.key_down(Engine::Sdl::Keycodes::right)) {
                                mike.move_right();
                                return boost::none;
                        }

                        return idle();
                });
}

auto Mike::jumping_in_place() -> State
{
        return single_pass_animation("jumping_in_place", idle());
}

auto Mike::preparing_to_jump_sideways() -> State
{
        return single_pass_animation("preparing_to_jump_sideways", jumping_sideways());
}

auto Mike::jumping_sideways() -> State
{
        return single_pass_animation("jumping_sideways",
                                     [](Mike& mike, Engine::Gameplay::Keyboard const&) -> Optional_state
                                     {
                                             mike.move_in_current_direction(mike.jump_speed_);
                                             return boost::none;
                                     },
                                     landing_sideways());
}

auto Mike::landing_sideways() -> State
{
        return single_pass_animation("landing_sideways", idle());
}

auto Mike::single_pass_animation(char const* animation_name, State next_state) -> State
{
        return single_pass_animation(animation_name, State::on_update_do_nothing(), next_state);
}

auto Mike::looping_animation(char const* animation_name) -> State
{
        return looping_animation(animation_name, State::on_update_do_nothing());
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

void Mike::hard_switch_animation(std::string const& animation_name)
{
        auto& animation = animations_.at(animation_name);
        current_animation_.hard_switch(animation);
}

void Mike::soft_switch_animation(std::string const& animation_name)
{
        auto& animation = animations_.at(animation_name);
        current_animation_.soft_switch(animation);
}

Engine::Sdl::Flip Mike::current_flip() const noexcept
{
        return is_facing_left()?
                Engine::Sdl::Flip::horizontal : Engine::Sdl::Flip::none;
}

}

