#include "mike.h"
#include "mike_sprites.h"
#include "engine/engine.h"
#include <string>



#include <iostream>






using namespace std::string_literals;
using namespace std::complex_literals;

int main()
{
        Engine::Sdl::Manager manager;
        (void)manager;

        // TODO Due to working with `unique_ptr` it turns out I have const correctness issues
        auto [window, renderer] = Engine::Sdl::create_window_and_renderer("Title"s, 500, 500);
        (void)window;

        // TODO Ditch the `load` bullshit and have Mike_sprite take a `Renderer` and `Mike`

        Game::Logic::Mike mike(0.0 + 150.0i);
        Game::Graphics::Mike_sprite mike_sprite(Game::Graphics::Mike_sprites::load(renderer), mike);
        Engine::Gameplay::Signals signals;
        Game::Logic::register_mike_keyboard_controls(mike, signals);

        auto const on_frame_advance =
        [&](Engine::Gameplay::Keyboard const&)
        {
                Engine::Sdl::render_clear(renderer);
                mike_sprite.update();
                mike_sprite.render(renderer);
                Engine::Sdl::render_present(renderer);
        };

        signals.frame_advance.connect(on_frame_advance);
        Engine::Gameplay::main_loop(signals, 60);
}

