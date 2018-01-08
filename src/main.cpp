#include "mike.h"
#include "block.h"
#include "mike_sprite.h"
#include "engine/engine.h"
#include <string>
#include <utility>



#include <iostream>




// TODO Rename Game::Logic -> Game::Gameplay

using namespace std::string_literals;
using namespace std::complex_literals;

int main()
{
        Engine::Sdl::Manager manager;
        (void)manager;

        auto window = Engine::Sdl::create_window("Title"s, 500, 500);
        auto renderer = Engine::Sdl::create_renderer(*window);

        Engine::Graphics::Animated_sprites mike_sprites = Game::Graphics::load_mike_sprites(*renderer);
        Game::Logic::Mike mike(300.0 + 150.0i,
                               Engine::Graphics::animations_durations(mike_sprites),
                               mike_sprites.at("standing_still"s).frame_width(),
                               mike_sprites.at("standing_still"s).frame_height()); // TODO These interfaces should take `dimensions` objects
        Game::Graphics::Mike_sprite mike_sprite(mike_sprites, mike);

        Engine::Sdl::Unique_texture block_texture = Engine::Sdl::load_texture(*renderer, "../res/sprites/block.png"s);

        Engine::Gameplay::Signals signals;
        Game::Logic::register_mike_keyboard_controls(mike, signals);

        Game::Logic::Block block(10.0 + 100.0i,
                                 Game::Logic::Block::Flags::solid | Game::Logic::Block::Flags::movable,
                                 Engine::Sdl::texture_width(*block_texture),
                                 Engine::Sdl::texture_height(*block_texture));

        Engine::Gameplay::Checkboxes_thunks solid_checkboxes_thunks {
                [&block] { return block.checkbox(); }
        };

        auto const on_frame_advance =
        [&](Engine::Gameplay::Main_loop&, Engine::Gameplay::Keyboard const&)
        {
                Engine::Sdl::render_clear(*renderer);
                mike.update_position(solid_checkboxes_thunks);
                Engine::Sdl::render_copy(*renderer, *block_texture, block.position());
                mike_sprite.render(*renderer);
                mike_sprite.update();
                Engine::Sdl::render_present(*renderer);
        };

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::Main_loop main_loop;
        main_loop.start(signals, 60);
}

