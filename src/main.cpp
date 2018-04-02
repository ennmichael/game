#include "mike.h"
#include "block.h"
#include "engine/engine.h"
#include <string>
#include <utility>



#include <iostream>

// TODO Is there even need to use boost::signals for the main loop (???)
// ^ Test and see

using namespace std::string_literals;
using namespace std::complex_literals;

auto constexpr fps = 60;

int main()
{
        Engine::Sdl::Manager _;

        auto window = Engine::Sdl::create_window("Title"s, 500, 500);
        auto renderer = Engine::Sdl::create_renderer(*window);
        auto sprite_sheet_config = Engine::Graphics::load_sprite_sheet_config("../res/configs/sprites.json"s);
        Engine::Graphics::Sprite_sheet sprite_sheet(*renderer, "../res/sprites.png"s);

        // FIXME In functions for loading from files, sometimes I use "load" in names and sometimes "read"
        
        /**
         * TODO Here's an idea: mike.json contains the animations, load_animations_config
         * loads configs from a property tree.
         */

        auto const mike_animations_config = Engine::Graphics::load_animations_config(
                        "../res/configs/animations/mike.json");
        auto const mike_config = Engine::Utils::read_json_tree("../res/configs/mike.json"s);
        Game::Gameplay::Mike mike(mike_config,
                                  Engine::Graphics::animations(sprite_sheet,
                                                               sprite_sheet_config,
                                                               mike_animations_config));

        Engine::Sdl::Unique_texture block_texture = Engine::Sdl::load_texture(*renderer, "../res/sprites/block.png"s); 

        Game::Gameplay::Blocks blocks {
                {
                        0.0 + 100.0i,
                        Engine::Sdl::texture_dimensions(*block_texture)
                },
                {
                        400.0 + 100.0i,
                        Engine::Sdl::texture_dimensions(*block_texture)
                }
        };

        Engine::Gameplay::Const_checkboxes_pointers solid_checkboxes { &blocks[0], &blocks[1] };

        Engine::Gameplay::Signals signals;

        auto const on_frame_advance =
        [&](Engine::Gameplay::Main_loop&, Engine::Gameplay::Keyboard const& keyboard)
        {
                Engine::Sdl::render_clear(*renderer);
                /*Engine::Sdl::render_copy(*renderer, *block_texture, blocks[0].position);
                Engine::Sdl::render_copy(*renderer, *block_texture, blocks[1].position);*/
                mike.update(keyboard);
                mike.render(*renderer);
                mike.render_sickness_filter(*renderer);
                Engine::Graphics::apply_filter(*renderer, Engine::Color::black().with_alpha(100));
                Engine::Sdl::render_present(*renderer);
        };

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::Main_loop main_loop;
        main_loop.start(signals, fps);
}

