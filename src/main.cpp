#include "mike.h"
#include "block.h"
#include "mike_sprite.h"
#include "engine/engine.h"
#include "configs.h"
#include <string>
#include <utility>



#include <iostream>

// TODO When changing states, reset the previous animation
// There might be a genuine need for this later, and we would accomplish it
// via signaling, but until then, I'll hold off on this.

using namespace std::string_literals;
using namespace std::complex_literals;

auto constexpr fps = 60;

int main()
{
        Engine::Sdl::Manager _;

        auto window = Engine::Sdl::create_window("Title"s, 500, 500);
        auto renderer = Engine::Sdl::create_renderer(*window);
        auto sprite_sheet_config = Engine::Graphics::load_sprite_sheet_config("../res/configs/sprites.json"s);
        auto animations_config = Engine::Graphics::load_animations_config("../res/configs/animations.json"s);
        Engine::Graphics::Sprite_sheet sprite_sheet(*renderer, "../res/sprites.png"s);

        auto [animations, static_sprites] = Engine::Graphics::load_resources(sprite_sheet,
                                                                             sprite_sheet_config,
                                                                             animations_config);

        auto const mike_config = Game::Configs::load_config<
                Game::Configs::Mike_config>("../res/configs/mike.json");

        Game::Gameplay::Mike mike(mike_config, Engine::Graphics::animations_durations(animations_config));
        Game::Graphics::Mike_animations mike_animations(mike, animations);

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
                mike.update(keyboard);

                Engine::Sdl::render_clear(*renderer);
                /*Engine::Sdl::render_copy(*renderer, *block_texture, blocks[0].position);
                Engine::Sdl::render_copy(*renderer, *block_texture, blocks[1].position);*/
                mike_animations.render_current_animation(*renderer);
                mike_animations.update_current_animation();
                Engine::Graphics::apply_filter(*renderer, Engine::Color::black().with_alpha(100));
                Engine::Sdl::render_present(*renderer);
        };

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::Main_loop main_loop;
        main_loop.start(signals, fps);
}

