#include "gameplay.h"
#include <unordered_map>
#include <optional>
#include <exception>
#include <cassert>

namespace Engine::Gameplay {

namespace {

class Timer {
public:
        explicit Timer(int delay_in_milliseconds)
                : delay_(delay_in_milliseconds)
        {}

        bool ready()
        {
                auto const ticks = Sdl::get_ticks();
                
                if (ticks >= last_ready_ + delay_) {
                        last_ready_ = ticks;
                        return true;
                }

                return false;
        }

private:
        int delay_;
        int last_ready_ = Sdl::get_ticks();
};

} // Close unnamed namespace

void main_loop(Signals const& signals, int fps)
{
        auto const calculate_frame_delay =
        [](int fps) noexcept
        {
                auto const frames_per_millisecond = fps / 1000.0;
                return static_cast<int>(1 / frames_per_millisecond);
        };

        auto const dispatch_event =
        [](Sdl::Event const& event, Keyboard& keyboard, Signals const& signals)
        {
                keyboard.update(event);

                if (event.type == SDL_QUIT) {
                        signals.quit();
                        return true;
                }

                return false;
        };

        auto const frame_delay = calculate_frame_delay(fps);
        Timer timer(frame_delay);
        Keyboard keyboard;

        auto quit = false;
        while (!quit) {
                while (auto const event = Sdl::poll_event())
                        quit = dispatch_event(*event, keyboard, signals);

                if (timer.ready()) {
                        signals.frame_advance(keyboard);
                }
        }
}

}

