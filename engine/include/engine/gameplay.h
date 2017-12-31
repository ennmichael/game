#pragma once

#include "boost/signals2.hpp"
#include <functional>

namespace Engine::Gameplay {

namespace {

namespace signals = boost::signals2;

}

struct Keyboard_signals {
        signals::signal<void()> left_arrow;
        signals::signal<void()> right_arrow;
        signals::signal<void()> spacebar;
};

struct Signals {
        Keyboard_signals key_pressed;
        Keyboard_signals key_released;
        Keyboard_signals key_down;
        Keyboard_signals key_up;

        signals::signal<void()> frame_advance;
        signals::signal<void()> quit;
};

using Signal_setup_function = std::function<void(Signals&)>;

void main_loop(Signals const& signals, int fps);

}

