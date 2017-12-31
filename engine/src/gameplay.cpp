#include "gameplay.h"
#include "sdl++/sdl++.h"
#include <unordered_map>
#include <optional>
#include <exception>
#include <cassert>



#include <iostream> // TODO Remove this



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
        enum class Key_state {
                Pressed,
                Down,
                Released,
                Up
        };

        struct Keyboard {
                void update(Sdl::Event const& event)
                {
                        update_key_states();
                        
                        switch (event.type) {
                                case SDL_KEYDOWN:
                                        press(event.key.keysym.sym);
                                        break;
                                case SDL_KEYUP:
                                        std::cout << "Release\n";
                                        std::cout << "Pressed: " << static_cast<int>(event.key.state == SDL_PRESSED) << '\n';
                                        std::cout << "Repeat: " << static_cast<int>(event.key.repeat) << '\n';
                                        release(event.key.keysym.sym);
                                        break;
                                default:
                                        break;
                        }
                }

                void press(Sdl::Keycode keycode)
                {
                        try {
                                auto& state = key_states[keycode];
                                if (state != Key_state::Down)
                                        state = Key_state::Pressed;
                        } catch (std::out_of_range const&) {
                                key_states[keycode] = Key_state::Pressed;
                        }
                }

                void release(Sdl::Keycode keycode)
                {
                        auto& state = key_states[keycode];
                        if (state != Key_state::Up)
                                state = Key_state::Released;
                }

                void update_key_states() noexcept
                {
                        for (auto& [keycode, state] : key_states) {
                                (void)keycode;

                                if (state == Key_state::Pressed)
                                        state = Key_state::Down;
                                else if (state == Key_state::Released)
                                        state = Key_state::Up;
                        }
                }

                std::unordered_map<Sdl::Keycode, Key_state> key_states;
        };

        auto const calculate_frame_delay =
        [](int fps) noexcept
        {
                auto const frames_per_millisecond = fps / 1000.0;
                return static_cast<int>(1 / frames_per_millisecond);
        };

        auto const dispatch_event =
        [](Signals const& signals,
           Keyboard& keyboard,
           Sdl::Event const& event)
        {
                keyboard.update(event);

                if (event.type == SDL_QUIT) {
                        signals.quit();
                        return true;
                }

                return false;
        };

        auto const dispatch_keyboard =
        [](Signals const& signals, Keyboard const& keyboard)
        {
                auto const key_pointer =
                [](Sdl::Keycode keycode) noexcept -> signals::signal<void()> Keyboard_signals::*
                {
                        switch (keycode) {
                                case SDLK_LEFT:  return &Keyboard_signals::left_arrow;
                                case SDLK_RIGHT: return &Keyboard_signals::right_arrow;
                                case SDLK_SPACE: return &Keyboard_signals::spacebar;
                                default:         return nullptr;
                        }
                };

                auto const keyboard_signals_pointer =
                [](Key_state key_state) noexcept -> Keyboard_signals Signals::*
                {
                        switch (key_state) {
                                case Key_state::Up:       return &Signals::key_up;
                                case Key_state::Down:     return &Signals::key_down;
                                case Key_state::Pressed:  return &Signals::key_pressed;
                                case Key_state::Released: return &Signals::key_released;
                                default:
                                        assert(false);
                                        return nullptr;
                        }
                };

                for (auto const [keycode, state] : keyboard.key_states) {
                        auto const key = key_pointer(keycode);
                        if (key) {
                                auto const keyboard_signals = keyboard_signals_pointer(state);
                                (signals.*keyboard_signals.*key)();
                        }
                }
        };
        

        auto const frame_delay = calculate_frame_delay(fps);
        Timer timer(frame_delay);
        Keyboard keyboard;

        auto quit = false;
        while (!quit) {
                while (auto const event = Sdl::poll_event()) {
                        // TODO We're handling quitting incorrectly
                        // There should be a `main_loop` or `game` object and `.quit`

                        quit = dispatch_event(signals, keyboard, *event);
                        dispatch_keyboard(signals, keyboard);
                }

                if (timer.ready())
                        signals.frame_advance();
        }
}

}

