#pragma once

#include "boost/signals2.hpp"
#include "sdl++/sdl++.h"
#include <functional>
#include <unordered_map>

namespace Engine::Gameplay {

enum class Key_state {
        Pressed,
        Released,
        Down,
        Up
};

using Key_state_signals = std::unordered_map<Key_state, boost::signals2::signal<void()>>;
using Keyboard_signals = std::unordered_map<Sdl::Scancode, Key_state_signals>;

struct Signals {
        Keyboard_signals keyboard;
        boost::signals2::signal<void()> frame_advance;
        boost::signals2::signal<void()> quit;
};

using Signal_setup_function = std::function<void(Signals&)>;

void main_loop(Signals const& signals, int fps);

}

