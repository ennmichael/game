#include "mike.h"
#include "mike_sprite.h"
#include "engine/engine.h"
#include <string>
#include <utility>



#include <iostream>




// TODO Try using std::string_view in interfaces
// TODO base.h should be utils.h

using namespace std::string_literals;
using namespace std::complex_literals;

int main()
{
        Engine::Sdl::Manager manager;
        (void)manager;

        auto [window, renderer] = Engine::Sdl::create_window_and_renderer("Title"s, 500, 500);
        (void)window;

        Engine::Graphics::Animated_sprites sprites = Game::Graphics::load_mike_sprites(*renderer);
        Game::Logic::Mike mike(0.0 + 150.0i, Engine::Graphics::animations_durations(sprites));
        Game::Graphics::Mike_sprite mike_sprite(sprites, mike);

        Engine::Sdl::Unique_texture block_texture = Engine::Sdl::load_texture(*renderer, "../res/sprites/block.png"s);

        Engine::Gameplay::Signals signals;
        Game::Logic::register_mike_keyboard_controls(mike, signals);

        std::cout << SDL_GetError() << '\n';

        auto const on_frame_advance =
        [&](Engine::Gameplay::Main_loop&, Engine::Gameplay::Keyboard const&)
        {
                Engine::Sdl::render_clear(*renderer);
                mike.update_position();
                Engine::Sdl::render_copy(*renderer, *block_texture, 10.0 + 100.0i);
                mike_sprite.render(*renderer);
                mike_sprite.update();
                Engine::Sdl::render_present(*renderer);
        };

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::Main_loop main_loop;
        main_loop.start(signals, 60);
}

