#include "engine/engine.h"
#include <string>



#include <iostream>






using namespace std::string_literals;
using namespace std::complex_literals;

int main()
{
        Sdl::Manager manager;
        (void)manager;

        auto const [window, renderer] = Sdl::create_window_and_renderer("Title"s, 500, 500);

        auto spr = Engine::Graphics::Sprite::load_from_file(renderer, "../res/sprites/run"s);
        auto animation = spr.animation();

        auto position = 0.0 + 150.0i;

        auto const on_frame_advance =
        [&]()
        {
                Sdl::render_clear(renderer);
                animation.frame_advance();
                spr.render(renderer, position);

                position += 1;

                Sdl::render_present(renderer);
        };

        Engine::Gameplay::main_loop( // TODO Reorder these parameters (?)
                [&](Engine::Gameplay::Signals& signals) {
                        signals.frame_advance.connect(on_frame_advance);
                }, 30
        );
        
        (void)window; // Perhaps this should be burried somewhere inside Engine?
}

