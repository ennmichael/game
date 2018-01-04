#include "mike.h"

using namespace std::string_literals;

namespace Game::Logic {

Mike::Mike(Engine::Complex_number position, Actions_durations const& durations) noexcept
        : position_(position)
        , durations_(durations)
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

bool Mike::is_jumping() const noexcept
{
        return state_ == State::jumping;
}

bool Mike::is_climbing() const noexcept
{
        return state_ == State::climbing;
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
        state_ = State::jumping;

        auto const callback =
        [this]
        {
                stand_still();
        };

        return Engine::Gameplay::Timed_callback(callback, durations_.jump);
}

void Mike::update_position() noexcept
{
        auto const move_forward =
        [this](double speed)
        {
                if (is_facing_left())
                        position_ -= speed;
                else if (is_facing_right())
                        position_ += speed;
        };

        if (is_running())
                move_forward(speed_);
        else if (is_jumping())
                move_forward(speed_*2);
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

void register_mike_keyboard_controls(Mike& mike, Engine::Gameplay::Signals& signals)
{
        signals.frame_advance.connect(
                [&mike](Engine::Gameplay::Main_loop& main_loop,
                        Engine::Gameplay::Keyboard const& keyboard)
                {
                        if (!user_has_control(mike))
                                return;

                        if (keyboard.key_down(Engine::Sdl::Keycodes::w) ||
                            keyboard.key_down(Engine::Sdl::Keycodes::up) ||
                            keyboard.key_down(Engine::Sdl::Keycodes::space)) {
                                auto const callback = mike.jump();
                                main_loop.register_callback(callback);
                        } else if (keyboard.key_down(Engine::Sdl::Keycodes::a) ||
                                   keyboard.key_down(Engine::Sdl::Keycodes::left)) {
                                mike.run_left();
                        } else if (keyboard.key_down(Engine::Sdl::Keycodes::d) ||
                                   keyboard.key_down(Engine::Sdl::Keycodes::right)) {
                                mike.run_right();
                        } else {
                                mike.stand_still();
                        }
                }
        );
}

bool user_has_control(Mike const& mike)
{
        return mike.is_standing_still() || mike.is_running();
}

}

