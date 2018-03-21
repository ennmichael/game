#pragma once

#include "sdl++.h"
#include <unordered_map>

namespace Engine::Gameplay {

enum class Key_state {
        pressed,
        down,
        up
};

class Keyboard {
public:
        void update(Sdl::Event const& event);

        bool key_pressed(Sdl::Keycode keycode) const noexcept;
        bool key_down(Sdl::Keycode keycode) const noexcept;
        bool key_up(Sdl::Keycode keycode) const noexcept;

        Key_state key_state(Sdl::Keycode keycode) const noexcept;

private:
        using Key_states = std::unordered_map<Sdl::Keycode, Key_state>;
        Key_states key_states_;
};

}

