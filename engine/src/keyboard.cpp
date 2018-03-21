#include "keyboard.h"

namespace Engine::Gameplay {

void Keyboard::update(Sdl::Event const& event)
{
        auto const down =
        [this](SDL_KeyboardEvent key)
        {
                key_states_[key.keysym.sym] = (key.repeat) ? 
                        Key_state::down : Key_state::pressed;
        };

        auto const up =
        [this](SDL_KeyboardEvent key)
        {
                key_states_[key.keysym.sym] = Key_state::up;
        };

        switch (event.type) {
                case SDL_KEYDOWN:
                        down(event.key);
                        break;
                case SDL_KEYUP:
                        up(event.key);
                        break;
                default:
                        break;
        }
}

bool Keyboard::key_pressed(Sdl::Keycode keycode) const noexcept
{
        return key_state(keycode) == Key_state::pressed;
}

bool Keyboard::key_down(Sdl::Keycode keycode) const noexcept
{
        return key_pressed(keycode) || key_state(keycode) == Key_state::down;
}

bool Keyboard::key_up(Sdl::Keycode keycode) const noexcept
{
        return key_state(keycode) == Key_state::up;
}

Key_state Keyboard::key_state(Sdl::Keycode keycode) const noexcept
{
        try {
                return key_states_.at(keycode);
        } catch (std::out_of_range const&) {
                return Key_state::up;
        }
}

}

