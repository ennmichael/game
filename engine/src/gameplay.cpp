#include "gameplay.h"
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
        using Keyboard = std::unordered_map<Sdl::Scancode, Key_state>;

        auto const update_keyboard =
        [](Keyboard& keyboard, Signals const& signals)
        {
                auto const update_key =
                [](Keyboard& keyboard, Sdl::Scancode scancode)
                {
                        auto const advance_key =
                        [](Keyboard& keyboard,
                           Sdl::Scancode scancode,
                           Key_state immediate_state,
                           Key_state longterm_state)
                        {
                                try {
                                        auto& state = keyboard.at(scancode);
                                        if (state == immediate_state)
                                                state = longterm_state;
                                        else if (state != longterm_state)
                                                state = immediate_state;
                                } catch (std::out_of_range const&) {
                                        keyboard[scancode] = immediate_state;
                                }
                        };

                        auto const down =
                        [&advance_key](Keyboard& keyboard, Sdl::Scancode scancode)
                        {
                                advance_key(keyboard,
                                            scancode,
                                            Key_state::Pressed,
                                            Key_state::Down);
                        };

                        auto const up =
                        [&advance_key](Keyboard& keyboard, Sdl::Scancode scancode)
                        {
                                advance_key(keyboard,
                                            scancode,
                                            Key_state::Released,
                                            Key_state::Up);
                        };

                        auto const key_is_down = SDL_GetKeyboardState(nullptr)[scancode];

                        if (key_is_down)
                                down(keyboard, scancode);
                        else
                                up(keyboard, scancode);
                };

                for (auto const& [scancode, key_states] : signals.keyboard) {
                        (void)key_states;
                        update_key(keyboard, scancode);
                }
        };

        auto const calculate_frame_delay =
        [](int fps) noexcept
        {
                auto const frames_per_millisecond = fps / 1000.0;
                return static_cast<int>(1 / frames_per_millisecond);
        };

        auto const dispatch_event =
        [](Sdl::Event const& event, Signals const& signals)
        {
                if (event.type == SDL_QUIT) {
                        signals.quit();
                        return true;
                }

                return false;
        };

        auto const dispatch_keyboard =
        [](Keyboard const& keyboard, Signals const& signals)
        {
                for (auto const& [scancode, key_state] : keyboard) {
                        try {
                                signals.keyboard.at(scancode).at(key_state)();
                        } catch (std::out_of_range const&){
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
                        
                        update_keyboard(keyboard, signals);
                        dispatch_keyboard(keyboard, signals);

                        quit = dispatch_event(*event, signals);
                }

                if (timer.ready())
                        signals.frame_advance();
        }
}

}

