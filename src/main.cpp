#include "mike.h"
#include "mike_sprites.h"
#include "engine/engine.h"
#include <string>
#include <utility>



#include <iostream>




// TODO Rename mike_durations.h -> mike_duration.h

using namespace std::string_literals;
using namespace std::complex_literals;

int main()
{
        Engine::Sdl::Manager manager;
        (void)manager;

        // TODO I have const correctness issues
        auto [window, renderer] = Engine::Sdl::create_window_and_renderer("Title"s, 500, 500);
        (void)window;

        // TODO Ditch the `load` bullshit and have Mike_sprite take a `Renderer` and `Mike`
        // TODO As-is, Mike_sprite should not own Mike_sprites
        // TODO FIX SDL AND THE UNIQUE_PTR GARBAGE
        auto sprites = Game::Graphics::Mike_sprites::load(renderer);
        Game::Logic::Mike mike(0.0 + 150.0i, sprites.actions_duratons());
        Game::Graphics::Mike_sprite mike_sprite(std::move(sprites), mike); // Nasty std::move

        Engine::Gameplay::Signals signals;
        Game::Logic::register_mike_keyboard_controls(mike, signals);

        auto const on_frame_advance =
        [&](Engine::Gameplay::Main_loop&, Engine::Gameplay::Keyboard const&)
        {
                Engine::Sdl::render_clear(renderer);
                mike.update_position();
                mike_sprite.update();
                mike_sprite.render(renderer);
                Engine::Sdl::render_present(renderer);
        };

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::Main_loop main_loop;
        main_loop.start(signals, 60);
}

