#include "gameplay.h"
#include "sdl++/sdl++.h"

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

void main_loop(Signal_setup_function signal_setup_function, int fps)
{
        auto const setup_signals =
        [](Signal_setup_function signal_setup_function)
        {
                Signals signals;
                signal_setup_function(signals);
                return signals;
        };

        auto const calculate_frame_delay =
        [](int fps) noexcept
        {
                auto const frames_per_millisecond = fps / 1000.0;
                return static_cast<int>(1 / frames_per_millisecond);
        };

        auto const dispatch_event =
        [](Signals const& signals, SDL_Event const& event)
        {
                auto const keyboard_signal_pointer = // Better name here please
                [](SDL_Keycode keycode) -> signals::signal<void()> Keyboard_signals::*
                {
                        switch (keycode) {
                                case SDLK_LEFT:  return &Keyboard_signals::left_arrow;
                                case SDLK_RIGHT: return &Keyboard_signals::right_arrow;
                                case SDLK_SPACE: return &Keyboard_signals::spacebar;
                        }

                        return nullptr;
                };

                if (event.type == SDL_QUIT) {
                        signals.quit();
                        return true;
                }
                
                auto const keyboard_signal = keyboard_signal_pointer(event.key.keysym.sym);

                if (keyboard_signal) {
                        if (event.type == SDL_KEYDOWN)
                                (signals.key_pressed.*keyboard_signal)();
                        else if (event.type == SDL_KEYUP)
                                (signals.key_released.*keyboard_signal)();
                }

                return false;
        };
        
        auto const signals = setup_signals(signal_setup_function);
        auto const frame_delay = calculate_frame_delay(fps);
        Timer timer(frame_delay);
        
        auto quit = false;
        while (!quit) {
                while (auto const event = Sdl::poll_event())
                        quit = dispatch_event(signals, *event);

                if (timer.ready())
                        signals.frame_advance();
        }
}

}

