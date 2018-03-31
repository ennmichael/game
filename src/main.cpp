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

// TODO *_sprite and *_texture classes should take positions as parameters when rendering,
// which means Block_texture is a redundant class.
// If we don't do it this way, we end up with circular references.

// TODO Always take `Dimension`s objects in interfaces, or provide two overloads

auto constexpr fps = 60;

int main()
{
        Engine::Sdl::Manager _;

        auto window = Engine::Sdl::create_window("Title"s, 500, 500);
        auto renderer = Engine::Sdl::create_renderer(*window);
        auto sprite_sheet_config = Engine::Graphics::load_sprite_sheet_config("../res/sprites.json");
        auto animations_config = Engine::Graphics::load_animations_config("../res/animations.json");
        Engine::Graphics::Sprite_sheet sprite_sheet(*renderer,
                                                    "../res/sprites.png"s,
                                                    sprite_sheet_config,
                                                    animations_config);
                                                  
        Game::Logic::Mike mike(200.0 + 150.0i,
                               Engine::Graphics::animations_durations(animations_config),
                               sprite_sheet.animation("idle").frame_dimensions());
        
        Game::Graphics::Mike_sprite mike_sprite(sprite_sheet, mike);

        Engine::Sdl::Unique_texture block_texture = Engine::Sdl::load_texture(*renderer, "../res/sprites/block.png"s); 

        Game::Logic::Blocks blocks {
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
        [&](Engine::Gameplay::Main_loop&, Engine::Gameplay::Keyboard const&)
        {
                std::cout << mike.position() << '\n';

                Engine::Sdl::render_clear(*renderer);
                /*Engine::Sdl::render_copy(*renderer, *block_texture, blocks[0].position);
                Engine::Sdl::render_copy(*renderer, *block_texture, blocks[1].position);*/
                mike_sprite.render(*renderer);
                mike_sprite.update();
                Engine::Sdl::render_present(*renderer);
        };

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::Main_loop main_loop;
        main_loop.start(signals, fps);
}

