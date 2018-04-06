#include "engine/engine.h"

using namespace std::string_literals;

namespace {

Engine::Complex_number mouse_position()
{
        int x;
        int y;
        SDL_GetMouseState(&x, &y);
        return Complex_number(x, y);
}

}

int main()
{
        Engine::Sdl::Manager manager;
        (void)manager;

        auto window = Engine::Sdl::create_window("Title"s, 500, 500);
        auto renderer = Engine::Sdl::create_renderer(*window); 

        Engine::Gameplay::Signals signals;
        
        Sdl::Rect box {
                200, 200,
                100, 100
        };

        auto const on_frame_advance =
        [&](Engine::Gameplay::Main_loop& main_loop, Engine::Gameplay::Keyboard const& keyboard)
        {
                Engine::Sdl::render_clear(*renderer);

                auto const mouse_pos = mouse_position();
                Engine::Sdl::render_rect(*renderer, box, Engine::Sdl::Color::red);
                render_shadow(renderer, box);

                Engine::Sdl::render_present(*renderer);
        };

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::Main_loop main_loop;
        main_loop.start(signals, 60);
}

