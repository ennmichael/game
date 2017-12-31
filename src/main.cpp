#include "engine/engine.h"
#include <string>



#include <iostream>






using namespace std::string_literals;
using namespace std::complex_literals;

void main_function()
{
        Sdl::Manager manager;
        (void)manager;

        auto const [window, renderer] = Sdl::create_window_and_renderer("Title"s, 500, 500);
        (void)window;

        auto spr = Engine::Graphics::Sprite::load_from_file(renderer, "../res/sprites/run"s);
        auto animation = spr.animation();

        auto position = 0.0 + 150.0i;

        auto const player_config = Engine::Config::load_from_file("../res/player.config"s);
        auto const player_speed = player_config.value<double>("speed"s);

        auto const on_frame_advance =
        [&]()
        {
                Sdl::render_clear(renderer);
                animation.frame_advance();
                spr.render(renderer, position);

                position += player_speed;

                Sdl::render_present(renderer);
        };

        Engine::Gameplay::Signals signals;

        signals.key_pressed.spacebar.connect(
                []
                {
                        std::cout << "Pressed\n";
                }
        );

        signals.key_released.spacebar.connect(
                []
                {
                        std::cout << "Released\n";
                }
        );

        signals.key_pressed.left_arrow.connect(
                []
                {
                        std::cout << "Left arrow\n";
                }
        );

        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::main_loop(signals, 60);
}

int main()
{
        main_function();
}

