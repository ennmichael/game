#pragma once

#include "boost/signals2.hpp"
#include "sdl++/sdl++.h"
#include "keyboard.h"
#include <functional>
#include <unordered_map>
#include <utility>

namespace Engine::Gameplay {

class Main_loop;

struct Signals {
        boost::signals2::signal<void(Keyboard const&)> keyboard_change;
        boost::signals2::signal<void()> frame_advance;
        boost::signals2::signal<void()> quit;
};

void main_loop(Signals const& signals, int fps);

}

