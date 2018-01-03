#include "mike.h"
#include "mike_animations.h"
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

        // TODO Should be Mike_sprites
        auto mike_animations = Game::Graphics::Mike_animations::load(renderer);
        Game::Logic::Mike mike(0.0 + 150.0i);
        Engine::Gameplay::Signals signals;
        Game::Logic::register_mike_keyboard_controls(mike, signals);

        auto const on_frame_advance =
        [&]
        {
                Engine::Sdl::render_clear(renderer);

                auto& sprite = mike_animations.current_sprite(mike);
                sprite.animation.advance(); // Not going to work
                sprite.animation.render(renderer, mike.position());

                Engine::Sdl::render_present(renderer);
        };

        signals.frame_advance.connect(on_frame_advance);
        Engine::Gameplay::main_loop(signals, 60);
}

